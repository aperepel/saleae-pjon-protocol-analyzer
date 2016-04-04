#ifndef PJON_ANALYZER_RESULTS
#define PJON_ANALYZER_RESULTS

#include <AnalyzerResults.h>

enum PJONFrameType {
    Sync = 1,
    Data = 2,
    Error = -1
};

class PJONAnalyzer;
class PJONAnalyzerSettings;

class PJONAnalyzerResults : public AnalyzerResults
{
public:
	PJONAnalyzerResults( PJONAnalyzer* analyzer, PJONAnalyzerSettings* settings );
	virtual ~PJONAnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

protected: //functions

protected:  //vars
	PJONAnalyzerSettings* mSettings;
	PJONAnalyzer* mAnalyzer;
    
private:
	struct UILabel {
		std::string tiny;
		std::string medium;
		std::string full;
	};
    
    UILabel GetAckNackLabels(U64 frame_index);
};

#endif //PJON_ANALYZER_RESULTS
