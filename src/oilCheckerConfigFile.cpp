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

	AddConfigItem(_T("TANK_WIDTH"), config.tankDimensions.tankWidth);
	AddConfigItem(_T("TANK_HEIGHT"), config.tankDimensions.tankHeight);
	AddConfigItem(_T("TANK_LENGTH"), config.tankDimensions.tankLength);
	AddConfigItem(_T("TANK_HEIGHT_OFFSET"), config.tankDimensions.heightOffset);

	AddConfigItem(_T("TEMP_PERIOD"), config.temperatureMeasurementPeriod);
	AddConfigItem(_T("OIL_PERIOD"), config.oilMeasurementPeriod);
	AddConfigItem(_T("SUMMARY_PERIOD"), config.summaryEmailPeriod);
	AddConfigItem(_T("NEW_LOG_PERIOD"), config.logFileRestartPeriod);

	AddConfigItem(_T("EMAIL_SENDER"), config.emailRecipients);
	AddConfigItem(_T("STMP_URL"), config.stmpUrl);
	AddConfigItem(_T("EMAIL"), config.emailRecipients);
	AddConfigItem(_T("LOG_PATH"), config.logFilePath);
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

	if (config.tankDimensions.tankHeight <= 0.0)
	{
		outStream << GetKey(config.tankDimensions.tankHeight) << " must be strictly positive" << std::endl;
		ok = false;
	}

	if (config.tankDimensions.tankWidth <= 0.0)
	{
		outStream << GetKey(config.tankDimensions.tankWidth) << " must be strictly positive" << std::endl;
		ok = false;
	}

	if (config.tankDimensions.tankLength <= 0.0)
	{
		outStream << GetKey(config.tankDimensions.tankLength) << " must be strictly positive" << std::endl;
		ok = false;
	}

	if (config.oilMeasurementPeriod == 0)
	{
		outStream << GetKey(config.oilMeasurementPeriod) << " must be strictly positive" << std::endl;
		ok = false;
	}

	if (config.stmpUrl.empty())
	{
		outStream << GetKey(config.stmpUrl) << " must be specified" << std::endl;
		ok = false;
	}
	
	if (config.emailSender.empty())
	{
		outStream << GetKey(config.emailSender) << " must be specified" << std::endl;
		ok = false;
	}
	
	if (config.emailRecipients.empty())
	{
		outStream << "At least one " << GetKey(config.emailRecipients) << " must be specified" << std::endl;
		ok = false;
	}

	return ok;
}
