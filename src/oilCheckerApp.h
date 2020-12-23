// File:  oilCheckerApp.h
// Date:  12/22/2020
// Auth:  K. Loux
// Desc:  Oil checker class.

#ifndef OIL_CHECKER_APP_H_
#define OIL_CHECKER_APP_H_

// Standard C++ headers
#include <string>

class OilCheckerApp
{
public:
	int Run(int argc, char* argv[]);

private:
	void PrintUsage(const std::string& calledAs);
};

#endif// OIL_CHECKER_APP_H_
