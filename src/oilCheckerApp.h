// File:  oilCheckerApp.h
// Date:  12/22/2020
// Auth:  K. Loux
// Desc:  Oil checker class.

#ifndef OIL_CHECKER_APP_H_
#define OIL_CHECKER_APP_H_

// Standard C++ headers
#include <string>

// Local forward declarations
struct OilCheckerConfig;

class OilCheckerApp
{
public:
	int Run(int argc, char* argv[]);

private:
	static const std::string oAuth2TokenURL;
	static const std::string oAuth2DeviceTokenURL;
	static const std::string oAuth2AuthenticationURL;
	static const std::string oAuth2DeviceAuthenticationURL;
	static const std::string oAuth2ClientID;
	static const std::string oAuth2ClientSecret;
	static const std::string redirectURI;

	void PrintUsage(const std::string& calledAs);
	
	bool SetupOAuth2Interface(const OilCheckerConfig& config);
};

#endif// OIL_CHECKER_APP_H_
