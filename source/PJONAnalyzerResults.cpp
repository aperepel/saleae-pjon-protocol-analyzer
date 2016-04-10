#include "PJONAnalyzerResults.h"
#include "PJONAnalyzer.h"
#include "PJONAnalyzerSettings.h"
#include "PJONPacketState.h"
#include <AnalyzerHelpers.h>
#include <iostream>
#include <fstream>

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
 
	char number_str[128];
    switch (frame.mType) {
        case Sync: {
            char* str = PJONPacketState::asDisplayString(frame.mFlags);
            AddResultString("s");
            AddResultString("Sync");
            AddResultString("Sync ", str);
            break;
        }
            
        case Data: {
            UILabel label = GetAckNackLabels(frame_index);
            AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
            AddResultString(label.tiny.c_str());
            AddResultString(label.medium.c_str());
            AddResultString("Data ", label.full.c_str(), " ", number_str);
            break;
        }
            
        case Error: {
            AddResultString("e");
            AddResultString("Error");
            break;
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
PJONAnalyzerResults::UILabel PJONAnalyzerResults::GetAckNackLabels(U64 frame_index)
{
    Frame frame = GetFrame( frame_index );
    UILabel label;
    char *frameTypeStr = PJONPacketState::asDisplayString(frame.mFlags);

    // special treatment for ACK/NACK
    if (frame.mFlags & PJONPacketState::FLAG_ACK_NACK) {
        U64 val = frame.mData1;
        switch (val) {
            case PJONPacketState::PJON_ACK_VALUE:
                label.tiny = "A";
                label.medium = "ACK";
                label.full = "ACK"; // will be concatenated next
                break;
                
            case PJONPacketState::PJON_NACK_VALUE:
                label.tiny = "N";
                label.medium = "NACK";
                label.full = "NACK"; // will be concatenated next
                break;

            default:
                label.tiny = "W";
                label.medium = "WARN";
                label.full = "WARNING non-protocol " + std::string(frameTypeStr);
                break;
        }
      } else {
        label.tiny = "d";
        label.medium = frameTypeStr;
        label.full = frameTypeStr;
    }

    return label;
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
        case Sync: {
            char* str = PJONPacketState::asDisplayString(frame.mFlags);
            AddTabularText("Sync ", str);
            break;
        }
            
        case Data: {
            UILabel label = GetAckNackLabels(frame_index);

            AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
            AddTabularText("Data ", label.full.c_str(), " ", number_str);
            break;
        }
            
        case Error: {
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