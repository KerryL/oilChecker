// File:  oilCheckerApp.cpp
// Date:  12/22/2020
// Auth:  K. Loux
// Desc:  Oil checker class and application entry point.

// Local headers
#include "oilCheckerApp.h"
#include "oilCheckerConfigFile.h"
#include "oilChecker.h"
#include "logging/logger.h"
#include "logging/combinedLogger.h"
#include "email/oAuth2Interface.h"

// Standard C++ headers
#include <iostream>
#include <fstream>

const std::string OilCheckerApp::oAuthTokenFileName(".oilCheckerOAuth");

int OilCheckerApp::Run(int argc, char* argv[])
{
	if (argc != 2)
	{
		PrintUsage(argv[0]);
		return 1;
	}

	const std::string logFileName("oilChecker.log");// TODO:  Time-stamp the log so we don't overwrite
	UString::OFStream logFile(logFileName);
	if (!logFile.is_open())
	{
		std::cerr << "Failed to open '" << logFileName << "' for output\n";
		return 1;
	}

	CombinedLogger<UString::OStream> log;
	log.Add(std::make_unique<Logger>(logFile));
	log.Add(std::make_unique<Logger>(Cout));

	OilCheckerConfigFile configFile(log);
	if (!configFile.ReadConfiguration(UString::ToStringType(argv[1])))
		return 1;
		
	if (!SetupOAuth2Interface(configFile.GetConfiguration().email, log))
		return 1;
	
	OilChecker checker(configFile.GetConfiguration(), log);
	checker.Run();

	return 0;
}

void OilCheckerApp::PrintUsage(const std::string& calledAs)
{
	std::cout << "Usage:  " << calledAs << " <config file name>" << std::endl;
}

bool OilCheckerApp::SetupOAuth2Interface(const EmailConfig& email, UString::OStream& log)
{
	OAuth2Interface::Get().SetClientID(email.oAuth2ClientID);
	OAuth2Interface::Get().SetClientSecret(email.oAuth2ClientSecret);
	OAuth2Interface::Get().SetVerboseOutput(false);
	if (!email.caCertificatePath.empty())
		OAuth2Interface::Get().SetCACertificatePath(email.caCertificatePath);

	// Originally, this was for windows only, but device access does not
	// support full access to e-mail.  So the user has some typing to do...
#if 1//#ifdef _WIN32
	OAuth2Interface::Get().SetTokenURL(email.oAuth2TokenURL);
	OAuth2Interface::Get().SetAuthenticationURL(email.oAuth2AuthenticationURL);
	OAuth2Interface::Get().SetResponseType("code");
	OAuth2Interface::Get().SetRedirectURI(email.redirectURI);
	OAuth2Interface::Get().SetLoginHint(email.sender);
	OAuth2Interface::Get().SetGrantType("authorization_code");
	OAuth2Interface::Get().SetScope("https://mail.google.com/");
#else
	OAuth2Interface::Get().SetTokenURL(email.oAuth2DeviceTokenURL);
	OAuth2Interface::Get().SetAuthenticationURL(email.oAuth2DeviceAuthenticationURL);
	OAuth2Interface::Get().SetGrantType("http://oauth.net/grant_type/device/1.0");
	OAuth2Interface::Get().SetScope("https://www.googleapis.com/auth/gmail.compose");
#endif

	// Set the refresh token (one will be created, if this is the first login)
	std::string oAuth2Token;
	{
		std::ifstream tokenFile(oAuthTokenFileName);
		if (tokenFile.is_open())// If it's not found, no error since that just means we haven't logged in yet
			std::getline(tokenFile, oAuth2Token);
		else
			log << "Could not open '" << oAuthTokenFileName << "' for input; will request new token..." << std::endl;
	}
	
	OAuth2Interface::Get().SetRefreshToken(oAuth2Token);
	if (OAuth2Interface::Get().GetRefreshToken() != oAuth2Token)
	{
		oAuth2Token = OAuth2Interface::Get().GetRefreshToken();
		std::ofstream tokenFile(oAuthTokenFileName);
		if (tokenFile.is_open())
		{
			tokenFile << oAuth2Token;
			log << "Updated OAuth2 refresh token written to " << oAuthTokenFileName << std::endl;
		}
		else
			log << "Failed to write updated OAuth2 refresh token to " << oAuthTokenFileName << std::endl;
	}

	if (OAuth2Interface::Get().GetRefreshToken().empty())
	{
		log << "Failed to obtain refresh token" << std::endl;
		return false;
	}

	return true;
}

int main(int argc, char* argv[])
{
	OilCheckerApp app;
	return app.Run(argc, argv);
}
