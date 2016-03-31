#ifndef PJON_SIMULATION_DATA_GENERATOR
#define PJON_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class PJONAnalyzerSettings;

class PJONSimulationDataGenerator
{
public:
	PJONSimulationDataGenerator();
	~PJONSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, PJONAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	PJONAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	void CreateSerialByte();
	std::string mSerialText;
	U32 mStringIndex;

	SimulationChannelDescriptor mSerialSimulationData;

};
#endif //PJON_SIMULATION_DATA_GENERATOR