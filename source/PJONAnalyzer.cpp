#include "PJONAnalyzer.h"
#include "PJONAnalyzerSettings.h"
#include "PJONPacketState.h"
#include <AnalyzerChannelData.h>
#include <AnalyzerHelpers.h>

enum class PJONState {
    SyncExpected,
    ZeroPadExpected,
    DataExpected,
    Unknown
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
PJONAnalyzer::PJONAnalyzer()
:	Analyzer2(),
	mSettings( new PJONAnalyzerSettings() ),
	mSimulationInitialized( false )
{
	SetAnalyzerSettings( mSettings.get() );
}

PJONAnalyzer::~PJONAnalyzer()
{
	KillThread();
}

void PJONAnalyzer::SetupResults()
{
    mResults.reset( new PJONAnalyzerResults( this, mSettings.get() ) );
    SetAnalyzerResults( mResults.get() );
    mResults->AddChannelBubblesWillAppearOn( mSettings->mInputChannel );
}

void PJONAnalyzer::WorkerThread()
{


	mSampleRateHz = GetSampleRate();
    mPJON = GetAnalyzerChannelData( mSettings->mInputChannel );
    ClockGenerator clock_generator;
    clock_generator.Init( mSettings->mBitRate, mSampleRateHz );
    mPJON->TrackMinimumPulseWidth();


	U32 samples_per_bit = mSampleRateHz / mSettings->mBitRate;
    U32 half_samples_per_bit = samples_per_bit / 2;
    U32 tolerance_samples = ((double) mSettings->mTolerance / mSettings->mBitWidth) * samples_per_bit;

    double spacer_ratio = (double) mSettings->mBitSpacer / mSettings->mBitWidth;


    PJONState current_state = PJONState::Unknown;

    PJONPacketState packet_state;

    U16 bits_read_count = 0;
    // start and end boundaries for reading a byte (used by markers)
    U64 data_sample_start = 0;
    U64 data_sample_end = 0;
    U64 sync_sample_start = 0;
    U64 data = 0xFF;
    short payload_bytes_to_come = 1;
    DataBuilder data_builder;

    if ( mPJON->GetBitState() == BIT_LOW ) {
        mPJON->AdvanceToNextEdge();
    }

	while (true)
    {
        //fprintf(stderr, "raw - %s\n", PJONPacketState::asDisplayString(packet_state.asDisplayFlag()));
        switch (current_state) {
            case PJONState::Unknown: {

                if (mPJON->WouldAdvancingCauseTransition(samples_per_bit + tolerance_samples)) {
                    current_state = PJONState::DataExpected;
                } else if (mPJON->WouldAdvancingCauseTransition(samples_per_bit * spacer_ratio + tolerance_samples)) {
                    current_state = PJONState::SyncExpected;
                } else {
                    U64 next = mPJON->GetSampleOfNextEdge();
                    U64 current = mPJON->GetSampleNumber();

                    U32 bitCount = (next - current) / samples_per_bit;

                    // TODO revisit this number
                    if (bitCount < 16) {
//                        mResults->AddMarker(mPJON->GetSampleNumber() + half_samples_per_bit, AnalyzerResults::Dot, mSettings->mInputChannel);
                        //mPJON->Advance(samples_per_bit + tolerance_samples);
                        mPJON->Advance(samples_per_bit);
                    } else {
                        mPJON->AdvanceToNextEdge();
                    }
                }

                break;
            }
            case PJONState::SyncExpected: {
                U64 start = mPJON->GetSampleNumber();

                if (mPJON->GetBitState() == BIT_LOW) {
                    // advance a little, seeking for a rising sync, but no more than a bit width
                    U64 rising_edge = mPJON->GetSampleOfNextEdge();
                    // seek ahead only for spacer + zero bit width, otherwise timeout
                    if ((rising_edge - start) < (samples_per_bit + samples_per_bit * spacer_ratio)) {
                        mPJON->AdvanceToNextEdge();
                        start = mPJON->GetSampleNumber();
                    } else {
                        mPJON->Advance(half_samples_per_bit);
//                        mResults->AddMarker(mPJON->GetSampleNumber(), AnalyzerResults::X, mSettings->mInputChannel);
                        mPJON->Advance(half_samples_per_bit);
                    }
                }

                // save for later, still need a trailing zero
                sync_sample_start = start;
                U64 end = mPJON->GetSampleOfNextEdge();


                if (mPJON->GetBitState() == BIT_HIGH) {
                    U64 center = start + (end - start) / 2;
                    mResults->AddMarker(center, AnalyzerResults::Start, mSettings->mInputChannel);
                }

                mPJON->AdvanceToAbsPosition(end);
                current_state = PJONState::ZeroPadExpected;

                break;
            }
            case PJONState::ZeroPadExpected: {
                BitState bit = mPJON->GetBitState();
                U64 center = mPJON->GetSampleNumber() + half_samples_per_bit;
                if (bit == BIT_LOW) {
                    // expecting a zero bit after a longer 1
                    mResults->AddMarker(center, AnalyzerResults::X, mSettings->mInputChannel);

                    // TODO extract this if/else into a function
                    if (mPJON->WouldAdvancingCauseTransition(samples_per_bit + tolerance_samples)) {
                        mPJON->AdvanceToNextEdge();
                    } else {
                        mPJON->Advance(samples_per_bit);
                    }

                    Frame f;
                    f.mFlags = 0;
                    f.mStartingSampleInclusive = sync_sample_start;
                    f.mEndingSampleInclusive = mPJON->GetSampleNumber();
                    f.mType = PJONFrameType::Sync;
                    //fprintf(stderr, "%s\n", PJONPacketState::asDisplayString(packet_state.asDisplayFlag()));
                    f.mFlags = packet_state.asDisplayFlag();
                    mResults->AddFrame(f);

                    current_state = PJONState::DataExpected;
                } else {
                    // unexpected 1
                    U64 errorMarkerPos = sync_sample_start + (mPJON->GetSampleNumber() - sync_sample_start) / 2;
                    mResults->AddMarker(errorMarkerPos, AnalyzerResults::ErrorX, mSettings->mInputChannel);
//                    mPJON->Advance(samples_per_bit); // TODO advance with tolerance check

                    Frame f;
                    // TODO still display the packet type?
                    f.mFlags = DISPLAY_AS_ERROR_FLAG;
                    f.mStartingSampleInclusive = sync_sample_start;
                    f.mEndingSampleInclusive = mPJON->GetSampleNumber();
                    f.mType = PJONFrameType::Error;
                    mResults->AddFrame(f);

                    packet_state.reset();
                    current_state = PJONState::Unknown;

                    mPJON->Advance(1);
                    mResults->CancelPacketAndStartNewPacket();
                }
                break;

            }
            case PJONState::DataExpected: {
                if (bits_read_count == 0) {
                    data_builder.Reset(&data, AnalyzerEnums::LsbFirst, 8);
                    data_sample_start = mPJON->GetSampleNumber();
                }

                BitState bit = mPJON->GetBitState();
                data_builder.AddBit(bit);
                bits_read_count++;

                // bit_width * (bit_acceptance / bit_width)
                // TODO recalculate based on user-provided settings
                if (mPJON->WouldAdvancingCauseTransition(samples_per_bit * 1.8)) {
//                if (mPJON->WouldAdvancingCauseTransition(samples_per_bit + tolerance_samples)) {
                    U64 next_edge = mPJON->GetSampleOfNextEdge();
                    U64 center = mPJON->GetSampleNumber() + (next_edge - mPJON->GetSampleNumber()) / 2;
                    mResults->AddMarker(center, AnalyzerResults::Dot, mSettings->mInputChannel);
                    mPJON->AdvanceToNextEdge();
                } else {
                    mResults->AddMarker(mPJON->GetSampleNumber() + half_samples_per_bit, AnalyzerResults::Dot, mSettings->mInputChannel);
                    mPJON->Advance(samples_per_bit);
                    // avoid a 'center' point drift for extended transactions, add tolerance only every other time
                    //mPJON->Advance(samples_per_bit + ((bits_read_count % 2) ? tolerance_samples : 0));
                }

                if (bits_read_count == 8) {
                    data_sample_end = mPJON->GetSampleNumber();
                    Frame f;
                    f.mStartingSampleInclusive = data_sample_start;
                    f.mEndingSampleInclusive = data_sample_end;
                    f.mFlags = packet_state.asDisplayFlag();
                    f.mType = PJONFrameType::Data;
                    f.mData1 = data;
                    mResults->AddFrame(f);

                    bits_read_count = 0;

                    mResults->CommitPacketAndStartNewPacket();

                    switch (packet_state.current()) {
                        case PJONPacketState::Packet::Length: {
                            payload_bytes_to_come = data - PJONPacketState::PJON_PACKET_OVERHEAD;
                            break;
                        }

                        case PJONPacketState::Packet::Payload: {
                            payload_bytes_to_come--;
                            break;
                        }

                        /*
                         // a debugging breakpoint, nice place to investigate a 'delayed' ACK
                         case PJONPacketState::Packet::Checksum: {
                            // TODO remove this case statement, just here for debugging
                            bits_read_count;
                            break;
                        }*/

                        case PJONPacketState::Packet::AckNack: {
                            mResults->AddMarker(mPJON->GetSampleNumber(), AnalyzerResults::Stop, mSettings->mInputChannel);
                            mPJON->AdvanceToNextEdge();
                            break;
                        }

                        default: {
                            // just continue
                        }
                    }

                    if (PJONPacketState::Packet::Payload == packet_state.current()) {
                        if (payload_bytes_to_come == 0) {
                            // we have consumed all of the payload, transition and reset state
                            packet_state.next();
                            payload_bytes_to_come = 1;
                            current_state = PJONState::Unknown;
                        } else {
                            // keep consuming the multi-byte payload
                            current_state = PJONState::SyncExpected;
                        }
                    } else if (PJONPacketState::Packet::Checksum == packet_state.current()) {
                        packet_state.next();
                        // correct some protocol jitter found under load testing, don't seek & guess
                        current_state = PJONState::SyncExpected;
                    } else {
                        // this is not a payload sequence, normal transition
                        packet_state.next();
                        current_state = PJONState::Unknown;
                    }

                } else {
                    current_state = PJONState::DataExpected;
                }
                break;

            }
            default: {
                // should never fall through here, but let's future-proof
                current_state = PJONState::Unknown;
                mPJON->AdvanceToNextEdge();
            }
        }

        mResults->CommitResults();
        ReportProgress(mPJON->GetSampleNumber());

        CheckIfThreadShouldExit();
    }
}


bool PJONAnalyzer::NeedsRerun() {
    // let's see if we should change the bit rate, base on mShortestActivePulse

    U64 shortest_pulse = mPJON->GetMinimumPulseWidthSoFar();

    fprintf( stderr, "Shortest pulse: %llu\n", shortest_pulse );

    if ( shortest_pulse == 0 )
    {
        AnalyzerHelpers::Assert( "Problem, shortest_pulse was 0" );
    }

    U32 computed_bit_rate = U32( double( mSampleRateHz ) / double( shortest_pulse ));

    if ( computed_bit_rate > mSampleRateHz )
    {
        AnalyzerHelpers::Assert( "Problem, computed_bit_rate is higher than sample rate" );
    }

    if ( computed_bit_rate > (mSampleRateHz / 4))
    {
        return false; //the baud rate is too fast.
    }
    if ( computed_bit_rate == 0 )
    {
        //bad result, this is not good data, don't bother to re-run.
        return false;
    }

    fprintf( stderr, "Computed bit rate: %u\n", computed_bit_rate );

    U32 specified_bit_rate = mSettings->mBitRate;
    
    double error = double( AnalyzerHelpers::Diff32( computed_bit_rate, specified_bit_rate ) ) / double( specified_bit_rate );
    
    if ( error > 0.1 )
    {
        mSettings->mBitRate = computed_bit_rate;
        mSettings->UpdateInterfacesFromSettings();
        return true;
    } else
    {
        return false;
    }
    
}


U32 PJONAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
    if( mSimulationInitialized == false )
    {
        mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
        mSimulationInitialized = true;
    }

    return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 PJONAnalyzer::GetMinimumSampleRateHz()
{
    return 1000000;
}

const char* PJONAnalyzer::GetAnalyzerName() const
{
    return "PJON";
}

const char* GetAnalyzerName()
{
    return "PJON";
}

Analyzer* CreateAnalyzer()
{
    return new PJONAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
    delete analyzer;
}
#pragma clang diagnostic pop