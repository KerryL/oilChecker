// File:  oilCheckerConfigFile.cpp
// Date:  12/22/2020
// Auth:  K. Loux
// Desc:  Config file class for oil level checker application.

// Local headers
#include "oilCheckerConfigFile.h"

OilCheckerConfigFile::OilCheckerConfigFile(UString::OStream& outStream) : ConfigFile(outStream)
{
}

void OilCheckerConfigFile::BuildConfigItems()
{
	AddConfigItem(_T("LOW_LEVEL_THRESHOLD"), config.lowLevelThreshold);

	AddConfigItem(_T("TANK_WIDTH"), config.tankDimensions.width);
	AddConfigItem(_T("TANK_HEIGHT"), config.tankDimensions.height);
	AddConfigItem(_T("TANK_LENGTH"), config.tankDimensions.length);
	AddConfigItem(_T("TANK_HEIGHT_OFFSET"), config.tankDimensions.heightOffset);

	AddConfigItem(_T("TEMP_PERIOD"), config.temperatureMeasurementPeriod);
	AddConfigItem(_T("OIL_PERIOD"), config.oilMeasurementPeriod);
	AddConfigItem(_T("SUMMARY_PERIOD"), config.summaryEmailPeriod);
	AddConfigItem(_T("NEW_LOG_PERIOD"), config.logFileRestartPeriod);

	AddConfigItem(_T("EMAIL_SENDER"), config.email.sender);
	AddConfigItem(_T("EMAIL"), config.email.recipients);

	AddConfigItem(_T("OATH2_CLIENT_ID"), config.email.oAuth2ClientID);
	AddConfigItem(_T("OATH2_CLIENT_SECRET"), config.email.oAuth2ClientSecret);
	
	AddConfigItem(_T("PING_TRIGGER_PIN"), config.ping.triggerPin);
	AddConfigItem(_T("PING_ECHO_PIN"), config.ping.echoPin);
}

void OilCheckerConfigFile::AssignDefaults()
{
	// Done in structure definition
}

bool OilCheckerConfigFile::ConfigIsOK()
{
	bool ok(true);

	if (config.lowLevelThreshold <= 0.0)
	{
		outStream << GetKey(config.lowLevelThreshold) << " must be strictly positive" << std::endl;
		return false;
	}

	if (config.tankDimensions.height <= 0.0)
	{
		outStream << GetKey(config.tankDimensions.height) << " must be strictly positive" << std::endl;
		ok = false;
	}

	if (config.tankDimensions.width <= 0.0)
	{
		outStream << GetKey(config.tankDimensions.width) << " must be strictly positive" << std::endl;
		ok = false;
	}

	if (config.tankDimensions.length <= 0.0)
	{
		outStream << GetKey(config.tankDimensions.length) << " must be strictly positive" << std::endl;
		ok = false;
	}

	if (config.oilMeasurementPeriod == 0)
	{
		outStream << GetKey(config.oilMeasurementPeriod) << " must be strictly positive" << std::endl;
		ok = false;
	}
	
	if (config.email.oAuth2ClientID.empty())
	{
		outStream << GetKey(config.email.oAuth2ClientID) << " must be specified" << std::endl;
		ok = false;
	}
	
	if (config.email.oAuth2ClientSecret.empty())
	{
		outStream << GetKey(config.email.oAuth2ClientSecret) << " must be specified" << std::endl;
		ok = false;
	}

	if (config.email.sender.empty())
	{
		outStream << GetKey(config.email.sender) << " must be specified" << std::endl;
		ok = false;
	}
	
	if (config.email.recipients.empty())
	{
		outStream << "At least one " << GetKey(config.email.recipients) << " must be specified" << std::endl;
		ok = false;
	}
	
	if (config.ping.triggerPin < 0)
	{
		outStream << GetKey(config.ping.triggerPin) << " must be specified" << std::endl;
		ok = false;
	}
	
	if (config.ping.echoPin < 0)
	{
		outStream << GetKey(config.ping.echoPin) << " must be specified" << std::endl;
		ok = false;
	}

	return ok;
}
