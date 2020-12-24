// File:  oilCheckerApp.h
// Date:  12/22/2020
// Auth:  K. Loux
// Desc:  Oil checker class.

#ifndef OIL_CHECKER_APP_H_
#define OIL_CHECKER_APP_H_

// Standard C++ headers
#include <string>

// Local headers
#include "utilities/uString.h"

// Local forward declarations
struct EmailConfig;

class OilCheckerApp
{
public:
	int Run(int argc, char* argv[]);

private:
	static const std::string oAuthTokenFileName;
	
	void PrintUsage(const std::string& calledAs);
	
	bool SetupOAuth2Interface(const EmailConfig& email, UString::OStream& log);
};

#endif// OIL_CHECKER_APP_H_
