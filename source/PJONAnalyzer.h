#ifndef PJON_ANALYZER_H
#define PJON_ANALYZER_H

#include <Analyzer.h>
#include "PJONAnalyzerResults.h"
#include "PJONSimulationDataGenerator.h"

class PJONAnalyzerSettings;
class ANALYZER_EXPORT PJONAnalyzer : public Analyzer2
{
public:
	PJONAnalyzer();
	virtual ~PJONAnalyzer();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();
    
    virtual void SetupResults();

	U64 SamplesToMicros(U64 samples) { return (samples * 1000000) / mSampleRateHz; }

protected:
    //vars
	std::auto_ptr< PJONAnalyzerSettings > mSettings;
	std::auto_ptr< PJONAnalyzerResults > mResults;
    AnalyzerChannelData* mPJON;

	PJONSimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitialized;

	U32 mSampleRateHz;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //PJON_ANALYZER_H
