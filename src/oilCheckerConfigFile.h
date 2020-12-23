// File:  oilCheckerConfigFile.h
// Date:  12/22/2020
// Auth:  K. Loux
// Desc:  Config file class for oil level checker application.

#ifndef OIL_CHECKER_CONFIG_FILE_H_
#define OIL_CHECKER_CONFIG_FILE_H_

// Local headers
#include "utilities/configFile.h"
#include "oilCheckerConfig.h"

class OilCheckerConfigFile : public ConfigFile
{
public:
	OilCheckerConfigFile(UString::OStream& outStream = Cout);

	OilCheckerConfig GetConfiguration() const { return config; }

private:
	void BuildConfigItems() override;
	void AssignDefaults() override;
	bool ConfigIsOK() override;

	OilCheckerConfig config;

	// Checks to make sure the directory is valid
	bool DirectoryExists(UString::String Path);
};

#endif// OIL_CHECKER_CONFIG_FILE_H_
