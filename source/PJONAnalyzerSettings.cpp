#include "PJONAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


PJONAnalyzerSettings::PJONAnalyzerSettings()
:	mInputChannel( UNDEFINED_CHANNEL ),
	mBitSpacer( 110 ),
	mBitWidth( 44 ),
	mAcceptance( 35 ),
	mReadDelay( 4 ),
	mTolerance( 5 ),
	mBitRate(14400)
{
	mInputChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mInputChannelInterface->SetTitleAndTooltip( "Channel", "PJON Communications channel" );
	mInputChannelInterface->SetChannel( mInputChannel );

	mBitSpacerInterface.reset( new AnalyzerSettingInterfaceInteger() );
	mBitSpacerInterface->SetTitleAndTooltip( "Bit Spacer µS",  "Specify the leading spacer bit width in microseconds" );
	mBitSpacerInterface->SetMax( 150 );
	mBitSpacerInterface->SetMin( 1 );
	mBitSpacerInterface->SetInteger( mBitSpacer );

	mBitWidthInterface.reset( new AnalyzerSettingInterfaceInteger() );
	mBitWidthInterface->SetTitleAndTooltip( "Bit Width µS",  "Specify the bit width in microseconds" );
	mBitWidthInterface->SetMax( 50 );
	mBitWidthInterface->SetMin( 1 );
	mBitWidthInterface->SetInteger( mBitWidth );

	mAcceptanceInterface.reset( new AnalyzerSettingInterfaceInteger() );
	mAcceptanceInterface->SetTitleAndTooltip( "Acceptance µS",  "Acceptance length in microseconds" );
	mAcceptanceInterface->SetMax( 50 );
	mAcceptanceInterface->SetMin( 1 );
	mAcceptanceInterface->SetInteger( mAcceptance );

	mReadDelayInterface.reset( new AnalyzerSettingInterfaceInteger() );
	mReadDelayInterface->SetTitleAndTooltip( "Read Delay µS",  "Specify the read delay in microseconds" );
	mReadDelayInterface->SetMax( 20 );
	mReadDelayInterface->SetMin( 1 );
	mReadDelayInterface->SetInteger( mReadDelay );

	mToleranceInterface.reset( new AnalyzerSettingInterfaceInteger() );
	mToleranceInterface->SetTitleAndTooltip( "Tolerance µS",  "Max values jitter in microseconds" );
	mToleranceInterface->SetMax( 5 );
	mToleranceInterface->SetMin( 1 );
	mToleranceInterface->SetInteger( mTolerance );

    mBitRateInterface.reset( new AnalyzerSettingInterfaceInteger() );
	mBitRateInterface->SetTitleAndTooltip( "Bit rate",  "" );
	mBitRateInterface->SetMax( 100000 );
	mBitRateInterface->SetMin( 1 );
	mBitRateInterface->SetInteger( mBitRate );

	AddInterface( mInputChannelInterface.get() );
	AddInterface( mBitSpacerInterface.get() );
	AddInterface( mBitWidthInterface.get() );
	AddInterface( mAcceptanceInterface.get() );
	AddInterface( mReadDelayInterface.get() );
	AddInterface( mToleranceInterface.get() );
	AddInterface( mBitRateInterface.get() );

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel( mInputChannel, "PJON", false );
}

PJONAnalyzerSettings::~PJONAnalyzerSettings()
{
}

bool PJONAnalyzerSettings::SetSettingsFromInterfaces()
{
	mInputChannel = mInputChannelInterface->GetChannel();
	mBitSpacer = mBitSpacerInterface->GetInteger();
	mBitWidth = mBitWidthInterface->GetInteger();
	mAcceptance = mAcceptanceInterface->GetInteger();
	mReadDelay = mReadDelayInterface->GetInteger();
	mTolerance = mToleranceInterface->GetInteger();

	ClearChannels();
	AddChannel( mInputChannel, "PJON", true );

	return true;
}

void PJONAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mInputChannelInterface->SetChannel( mInputChannel );
	mBitSpacerInterface->SetInteger( mBitSpacer );
	mBitWidthInterface->SetInteger( mBitWidth );
	mAcceptanceInterface->SetInteger( mAcceptance );
	mReadDelayInterface->SetInteger( mReadDelay );
	mToleranceInterface->SetInteger( mTolerance );
	mBitRateInterface->SetInteger( mBitRate );
}

void PJONAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mInputChannel;
	text_archive >> mBitSpacer;
	text_archive >> mBitWidth;
	text_archive >> mAcceptance;
	text_archive >> mReadDelay;
	text_archive >> mTolerance;

	ClearChannels();
	AddChannel( mInputChannel, "PJON", true );

	UpdateInterfacesFromSettings();
}

const char* PJONAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mInputChannel;
	text_archive << mBitSpacer;
	text_archive << mBitWidth;
	text_archive << mAcceptance;
	text_archive << mReadDelay;
	text_archive << mTolerance;

	return SetReturnString( text_archive.GetString() );
}
