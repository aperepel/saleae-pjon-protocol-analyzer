#ifndef PJON_ANALYZER_SETTINGS
#define PJON_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class PJONAnalyzerSettings : public AnalyzerSettings
{
public:
	PJONAnalyzerSettings();
	virtual ~PJONAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel mInputChannel;
	U32 mBitSpacer;
	U32 mBitWidth;
	U32 mAcceptance;
	U32 mReadDelay;
	U32 mTolerance;
	U32 mBitRate;

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mInputChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceInteger >	mBitSpacerInterface;
	std::auto_ptr< AnalyzerSettingInterfaceInteger >	mBitWidthInterface;
	std::auto_ptr< AnalyzerSettingInterfaceInteger >	mAcceptanceInterface;
	std::auto_ptr< AnalyzerSettingInterfaceInteger >	mReadDelayInterface;
	std::auto_ptr< AnalyzerSettingInterfaceInteger >	mToleranceInterface;
	std::auto_ptr< AnalyzerSettingInterfaceInteger >	mBitRateInterface;
};

#endif //PJON_ANALYZER_SETTINGS
