#include "PJONAnalyzerResults.h"
#include "PJONAnalyzer.h"
#include "PJONAnalyzerSettings.h"
#include "PJONPacketState.h"
#include <AnalyzerHelpers.h>
#include <iostream>
#include <fstream>

#define BROADCAST_ADDRESS 124

PJONAnalyzerResults::PJONAnalyzerResults( PJONAnalyzer* analyzer, PJONAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

PJONAnalyzerResults::~PJONAnalyzerResults()
{
}

void PJONAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );
 
    char buf[50];
	char number_str[128];
    switch (frame.mType) {
        case PJONFrameType::Sync: {
            char* str = PJONPacketState::asDisplayString(frame.mFlags);
            AddResultString("s");
            AddResultString("Sync");
            AddResultString("Sync ", str);
            break;
        }
            
        case PJONFrameType::Data: {
            GetAckNackString(buf, frame_index);
            AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
            AddResultString("d");
            AddResultString(PJONPacketState::asDisplayString(frame.mFlags));
            AddResultString("Data ", buf, " ", number_str);
            break;
        }
            
        case PJONFrameType::Error: {
            AddResultString("e");
            AddResultString("Error");
        }
            
        default: {
           	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
            AddResultString( number_str );
            break;
        }
    }

}

/*
    ACK, NACK or display the value otherwise
 */
void PJONAnalyzerResults::GetAckNackString(char* str, U64 frame_index)
{
    Frame frame = GetFrame( frame_index );
    
    // special treatment for ACK/NACK
    if (frame.mFlags & PJONPacketState::FLAG_ACK_NACK) {
        U64 val = frame.mData1;
        switch (val) {
            case PJONPacketState::PJON_ACK_VALUE:
                strcpy(str, "ACK");
                break;
                
            case PJONPacketState::PJON_NACK_VALUE:
                strcpy(str, "NACK");
                break;
                
            default:
                strcpy(str, " WARNING protocol error ");
                strcat(str, PJONPacketState::asDisplayString(frame.mFlags));
                break;
        }
      } else {
        strcpy(str, PJONPacketState::asDisplayString(frame.mFlags));
    }
}

void PJONAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	std::ofstream file_stream( file, std::ios::out );

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	file_stream << "Time [s],Value" << std::endl;

	U64 num_frames = GetNumFrames();
	for( U32 i=0; i < num_frames; i++ )
	{
		Frame frame = GetFrame( i );
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

		char number_str[128];
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );

		file_stream << time_str << "," << number_str << std::endl;

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			file_stream.close();
			return;
		}
	}

	file_stream.close();
}

void PJONAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
	Frame frame = GetFrame( frame_index );
    ClearTabularText();
   
    char buf[50];
   	char number_str[128];

    switch (frame.mType) {
        case PJONFrameType::Sync: {
            char* str = PJONPacketState::asDisplayString(frame.mFlags);
            AddTabularText("Sync ", str);
            break;
        }
            
        case PJONFrameType::Data: {
            GetAckNackString(buf, frame_index);
            AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
            AddTabularText("Data ", buf, " ", number_str);
            break;
        }
            
        case PJONFrameType::Error: {
            AddTabularText("Error");
            break;
        }
            
        default: {
           	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
            AddTabularText(number_str );
            break;
        }
    }

}

void PJONAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}

void PJONAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}