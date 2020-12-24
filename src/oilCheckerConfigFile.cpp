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

	AddConfigItem(_T("EMAIL_SENDER"), config.email.sender);
	AddConfigItem(_T("EMAIL"), config.email.recipients);
	
	AddConfigItem(_T("STMP_URL"), config.email.stmpUrl);
	AddConfigItem(_T("OAUTH2_TOKEN_URL"), config.email.oAuth2TokenURL);
	AddConfigItem(_T("OAUTH2_DEVICE_TOKEN_URL"), config.email.oAuth2DeviceTokenURL);
	AddConfigItem(_T("OAUTH2_AUTH_URL"), config.email.oAuth2AuthenticationURL);
	AddConfigItem(_T("OAUTH2_AUTH_DEVICE_URL"), config.email.oAuth2DeviceAuthenticationURL);
	AddConfigItem(_T("OATH2_CLIENT_ID"), config.email.oAuth2ClientID);
	AddConfigItem(_T("OATH2_CLIENT_SECRET"), config.email.oAuth2ClientSecret);
	AddConfigItem(_T("OAUTH2_REDIRECT_URI"), config.email.redirectURI);
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

	if (config.email.stmpUrl.empty())
	{
		outStream << GetKey(config.email.stmpUrl) << " must be specified" << std::endl;
		ok = false;
	}
	
	if (config.email.oAuth2TokenURL.empty())
	{
		outStream << GetKey(config.email.oAuth2TokenURL) << " must be specified" << std::endl;
		ok = false;
	}
	
	if (config.email.oAuth2DeviceTokenURL.empty())
	{
		outStream << GetKey(config.email.oAuth2DeviceTokenURL) << " must be specified" << std::endl;
		ok = false;
	}
	
	if (config.email.oAuth2AuthenticationURL.empty())
	{
		outStream << GetKey(config.email.oAuth2AuthenticationURL) << " must be specified" << std::endl;
		ok = false;
	}
	
	if (config.email.oAuth2DeviceAuthenticationURL.empty())
	{
		outStream << GetKey(config.email.oAuth2DeviceAuthenticationURL) << " must be specified" << std::endl;
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
	
	if (config.email.redirectURI.empty())
	{
		outStream << GetKey(config.email.redirectURI) << " must be specified" << std::endl;
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

	return ok;
}
