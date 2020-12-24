// File:  oilCheckerConfig.h
// Date:  12/22/2020
// Auth:  K. Loux
// Desc:  Configuration options for oil checker application.

#ifndef OIL_CHECKER_CONFIG_H_
#define OIL_CHECKER_CONFIG_H_

// Standard C++ headers
#include <string>
#include <vector>

struct TankDimensions
{
	double tankHeight = -1.0;// [in]
	double tankWidth = -1.0;// [in] (equal to diameter of rounded top/bottom)
	double tankLength = -1.0;// [in]
	double heightOffset = 0.0;// [in]
};

struct EmailConfig
{
	std::string sender;
	std::vector<std::string> recipients;
	
	std::string stmpUrl;
	std::string oAuth2TokenURL;
	std::string oAuth2DeviceTokenURL;
	std::string oAuth2AuthenticationURL;
	std::string oAuth2DeviceAuthenticationURL;
	std::string oAuth2ClientID;
	std::string oAuth2ClientSecret;
	std::string redirectURI;
	std::string caCertificatePath;
};

struct OilCheckerConfig
{
	double lowLevelThreshold = -1.0;// [gal]

	TankDimensions tankDimensions;

	unsigned int temperatureMeasurementPeriod = 30;// [min]
	unsigned int oilMeasurementPeriod = 120;// [min]
	unsigned int summaryEmailPeriod = 7;// [days]
	unsigned int logFileRestartPeriod = 365;// [days]

	EmailConfig email;
};

#endif// OIL_CHECKER_CONFIG_H_
