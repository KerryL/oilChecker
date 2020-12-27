// File:  oilChecker.cpp
// Date:  12/22/2020
// Auth:  K. Loux
// Desc:  Oil checker class.

// Local headers
#include "oilChecker.h"
#include "tankGeometry.h"
#include "rpi/ds18b20Sensor.h"
#include "rpi/pingSensor.h"
#include "email/oAuth2Interface.h"

// Standard C++ headers
#include <filesystem>
#include <iomanip>

const std::string OilChecker::oilLogFileName("oilHistory.csv");
const std::string OilChecker::temperatureLogFileName("temperatureHistory.csv");
const std::string OilChecker::oilLogCreatedDateFileName(".oilLogCreatedDate");
const std::string OilChecker::temperatureLogCreatedDateFileName(".temperatureLogCreatedDate");

OilChecker::~OilChecker()
{
	stopThreads = true;
	if (oilMeasurementThread.joinable())
		oilMeasurementThread.join();

	if (temperatureMeasurementThread.joinable())
		temperatureMeasurementThread.join();

	if (summaryUpdateThread.joinable())
		summaryUpdateThread.join();
}

void OilChecker::Run()
{
	if (!std::filesystem::exists(oilLogCreatedDateFileName))
		WriteLogCreatedDate(oilLogCreatedDateFileName, log);
	if (!std::filesystem::exists(temperatureLogCreatedDateFileName))
		WriteLogCreatedDate(temperatureLogCreatedDateFileName, log);

	oilLogCreatedDate = ReadLogCreatedDate(oilLogCreatedDateFileName, log);
	temperatureLogCreatedDate = ReadLogCreatedDate(temperatureLogCreatedDateFileName, log);
	
	oilMeasurementThread = std::thread(&OilChecker::OilMeasurementThreadEntry, this);
	temperatureMeasurementThread = std::thread(&OilChecker::TemperatureMeasurementThreadEntry, this);
	summaryUpdateThread = std::thread(&OilChecker::SummaryUpdateThreadEntry, this);

	std::unique_lock<std::mutex> lock(stopMutex);
	stopCondition.wait(lock, [this] { return !stopThreads; });
}

void OilChecker::OilMeasurementThreadEntry()
{
	while (!stopThreads)
	{
		const std::chrono::steady_clock::duration period(std::chrono::minutes(config.oilMeasurementPeriod));
		const auto wakeTime(std::chrono::steady_clock::now() + period);

		{
			std::unique_lock<std::mutex> lock(activityMutex);

			VolumeDistance values;
			if (!GetRemainingOilVolume(values))
			{
				log << "Failed to get remaining oil volume" << std::endl;
				stopThreads = true;
				stopCondition.notify_all();
				break;
			}

			if (!WriteOilLogData(values))
				log << "Failed to log oil data (v = " << values.volume << " gal, d = " << values.distance << " in)" << std::endl;

			if (values.volume < config.lowLevelThreshold)
			{
				log << "Low oil level detected!" << std::endl;
				if (!SendLowOilLevelEmail(values.volume))
					log << "Failed to send low oil warning email" << std::endl;
			}

			oilData.push_back(OilDataPoint(std::chrono::system_clock::now(), values));

			if (std::chrono::system_clock::now() > oilLogCreatedDate + std::chrono::minutes(config.logFileRestartPeriod * 24 * 60))
			{
				std::string newFileName(oilLogFileName + '_' + GetTimestamp());
				std::filesystem::rename(oilLogFileName, newFileName);
				SendNewLogFileEmail(newFileName);
				WriteLogCreatedDate(oilLogCreatedDateFileName, log);
				oilLogCreatedDate = ReadLogCreatedDate(oilLogCreatedDateFileName, log);
			}
		}

		std::unique_lock<std::mutex> lock(stopMutex);
		stopCondition.wait_until(lock, wakeTime);
	}
}

void OilChecker::TemperatureMeasurementThreadEntry()
{
	while (!stopThreads)
	{
		const std::chrono::steady_clock::duration period(std::chrono::minutes(config.temperatureMeasurementPeriod));
		const auto wakeTime(std::chrono::steady_clock::now() + period);

		{
			std::unique_lock<std::mutex> lock(activityMutex);

			double temperature;
			if (!GetTemperature(temperature))
			{
				log << "Failed to get temperature" << std::endl;
				stopThreads = true;
				stopCondition.notify_all();
				break;
			}

			if (!WriteTemperatureLogData(temperature))
				log << "Failed to log temperature data (T = " << temperature << " deg F)" << std::endl;

			temperatureData.push_back(TemperatureDataPoint(std::chrono::system_clock::now(), temperature));

			if (std::chrono::system_clock::now() > temperatureLogCreatedDate + std::chrono::minutes(config.logFileRestartPeriod * 24 * 60))
			{
				std::string newFileName(temperatureLogFileName + '_' + GetTimestamp());
				std::filesystem::rename(temperatureLogFileName, newFileName);
				SendNewLogFileEmail(newFileName);
				WriteLogCreatedDate(temperatureLogCreatedDateFileName, log);
				temperatureLogCreatedDate = ReadLogCreatedDate(temperatureLogCreatedDateFileName, log);
			}
		}

		std::unique_lock<std::mutex> lock(stopMutex);
		stopCondition.wait_until(lock, wakeTime);
	}
}

void OilChecker::SummaryUpdateThreadEntry()
{
	auto startTime(std::chrono::steady_clock::now());

	while (!stopThreads)
	{
		const std::chrono::steady_clock::duration period(std::chrono::minutes(config.summaryEmailPeriod * 24 * 60));
		const auto wakeTime(startTime + period);

		std::unique_lock<std::mutex> stopLock(stopMutex);
		stopCondition.wait_until(stopLock, wakeTime);
		startTime = std::chrono::steady_clock::now();

		{
			std::unique_lock<std::mutex> lock(activityMutex);

			if (!SendSummaryEmail())
				log << "Failed to send summary email" << std::endl;

			temperatureData.clear();
			oilData.clear();
		}	
	}
}

bool OilChecker::GetRemainingOilVolume(VolumeDistance& values) const
{
	log << "Reading distance sensor" << std::endl;
	
	PingSensor ping(config.ping.triggerPin, config.ping.echoPin);
	double distance;
	if (!ping.GetDistance(distance))
		return false;
		
	values.distance = distance / 2.54;// [in]
		
	VerticalTankGeometry tank(config.tankDimensions);
	values.volume = tank.ComputeRemainingVolume(values.distance);
	
	return true;
}

bool OilChecker::GetTemperature(double& temperature) const
{
	// We do this in a "lazy" way:
	// 1. Check to see if any sensors are connected
	// 2. If exactly one sensor is connected, continue and use this sensor
	// 3. Else, return an error
	
	log << "Checking for connected temperature sensors..." << std::endl;
	auto connectedSensors(DS18B20::GetConnectedSensors());
	if (connectedSensors.size() != 1)
	{
		log << "Found " << connectedSensors.size() << " sensor(s), expected 1" << std::endl;
		return false;
	}
	
	log << "Reading temperature from sensor " << connectedSensors.front() << std::endl;
	DS18B20 tempSensor(connectedSensors.front(), log);
	if (!tempSensor.GetTemperature(temperature))
		return false;
		
	temperature = temperature * 1.8 + 32.0;// Convert C to deg F
	log << "Measured temperature of " << temperature << " deg F" << std::endl;

	return true;
}

bool OilChecker::SendSummaryEmail() const
{
	log << "Sending summary email" << std::endl;
	UString::OStringStream ss;
	ss << "Summary for oil level and outside temperature for the last " << config.summaryEmailPeriod << " days:\n\n"
		<< "Date/Time         Temperature (deg F)  Remaining Oil (gal)\n";
	// Column widths are 16, 19, and 19 with two spaces between each column
		
	unsigned int oilI(0), tempI(0);
	while (oilI < oilData.size() || tempI < temperatureData.size())
	{
		if (oilI >= oilData.size() || temperatureData[tempI].t < oilData[oilI].t)
		{
			ss << GetTimestamp(temperatureData[tempI].t) <<std::string(19 + 2 + 2, ' ') << std::setfill(' ') << std::setw(19) << std::fixed << static_cast<int>(temperatureData[tempI].v + 0.5) << '\n';
			++tempI;
		}
		else if (tempI >= temperatureData.size() || oilData[oilI].t < temperatureData[tempI].t)
		{
			ss << GetTimestamp(oilData[oilI].t) << "  " << std::setfill(' ') << std::setw(19) << static_cast<int>(oilData[oilI].v.volume + 0.5) << '\n';
			++oilI;
		}
		else
		{
			ss << GetTimestamp(oilData[oilI].t) << "  " << std::setfill(' ') << std::setw(19) << static_cast<int>(oilData[oilI].v.volume + 0.5) << "  " << std::setfill(' ') << std::setw(19) << static_cast<int>(temperatureData[tempI].v + 0.5) << '\n';
			++oilI;
			++tempI;
		}
	}
	
	EmailSender::LoginInfo loginInfo;
	std::vector<EmailSender::AddressInfo> recipients;
	BuildEmailEssentials(loginInfo, recipients);
	EmailSender sender("Oil Level Summary", ss.str(), std::string(), recipients, loginInfo, true, false, log);
	if (!sender.Send())
		return false;

	log << "Successfully sent summary email" << std::endl;
	return true;
}

bool OilChecker::SendLowOilLevelEmail(const double& volumeRemaining) const
{
	log << "Sending low-level warning email" << std::endl;
	UString::OStringStream ss;
	ss << "Only " << volumeRemaining << " gal of oil remains in the tank, which is less than the threshold of "
		<< config.lowLevelThreshold << " gal.";
	
	EmailSender::LoginInfo loginInfo;
	std::vector<EmailSender::AddressInfo> recipients;
	BuildEmailEssentials(loginInfo, recipients);
	EmailSender sender("Low Oil Level Detected", ss.str(), std::string(), recipients, loginInfo, false, false, log);
	if (!sender.Send())
		return false;

	log << "Successfully sent low-level warning email" << std::endl;
	return true;
}

bool OilChecker::SendNewLogFileEmail(const std::string& oldLogFileName) const
{
	log << "Sending log file complete email for '" << oldLogFileName << "'" << std::endl;
	UString::OStringStream ss;
	ss << "Log file '" << oldLogFileName << "' reached maximum duration of " << config.logFileRestartPeriod << " days.  The old log file has been stored.  It is attached here for reference.";

	EmailSender::LoginInfo loginInfo;
	std::vector<EmailSender::AddressInfo> recipients;
	BuildEmailEssentials(loginInfo, recipients);
	EmailSender sender("Log File Reached Maximum Duration", ss.str(), oldLogFileName, recipients, loginInfo, true, false, log);
	if (!sender.Send())
		return false;

	log << "Successfully sent log file complete email" << std::endl;
	return true;
}

bool OilChecker::WriteOilLogData(const VolumeDistance& values) const
{
	log << "Adding oil data to log" << std::endl;
	const bool needsHeader(!std::filesystem::exists(oilLogFileName));
	std::ofstream file(oilLogFileName, std::ios::app);
	if (!file.is_open())
	{
		log << "Failed to open '" << oilLogFileName << "' for output" << std::endl;
		return false;
	}
	
	if (needsHeader)
		file << "Time,Distance (in),Volume (gal)\n";
	
	file << GetTimestamp() << ',' << values.distance << ',' << values.volume << '\n';
	return true;
}

bool OilChecker::WriteTemperatureLogData(const double& temperature) const
{
	log << "Adding temperature data to log" << std::endl;
	const bool needsHeader(!std::filesystem::exists(temperatureLogFileName));
	std::ofstream file(temperatureLogFileName, std::ios::app);
	if (!file.is_open())
	{
		log << "Failed to open '" << temperatureLogFileName << "' for output" << std::endl;
		return false;
	}
	
	if (needsHeader)
		file << "Time,Temperature (deg F)\n";
	
	file << GetTimestamp() << ',' << temperature << '\n';
	return true;
}

void OilChecker::BuildEmailEssentials(EmailSender::LoginInfo& loginInfo, std::vector<EmailSender::AddressInfo>& recipients) const
{
	loginInfo.smtpUrl = "smtp.gmail.com:587";
	loginInfo.localEmail = config.email.sender;
	loginInfo.oAuth2Token = OAuth2Interface::Get().GetRefreshToken();
	loginInfo.useSSL = true;
	loginInfo.caCertificatePath = config.email.caCertificatePath;

	recipients.resize(config.email.recipients.size());
	for (unsigned int i = 0; i < recipients.size(); ++i)
	{
		recipients[i].address = config.email.recipients[i];
		recipients[i].displayName = config.email.recipients[i];
	}
}

std::string OilChecker::GetTimestamp()
{
	auto now(std::chrono::system_clock::now());
	return GetTimestamp(now);
}

std::string OilChecker::GetTimestamp(const std::chrono::system_clock::time_point& now)
{
	std::time_t now_c(std::chrono::system_clock::to_time_t(now));
	const std::tm now_tm(*std::localtime(&now_c));
	const size_t timeSize(17);
	char timeString[timeSize];
	strftime(timeString, timeSize, "%Y-%m-%d_%H:%M", &now_tm);
	return std::string(timeString, timeSize - 1);
}

std::chrono::system_clock::time_point OilChecker::ReadLogCreatedDate(const std::string& fileName, UString::OStream& log)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		log << "Failed to open '" << fileName << "' for input" << std::endl;
		return std::chrono::system_clock::now();
	}
	
	std::string timeString;
	file >> timeString;

	std::tm tm{};
	strptime(timeString.c_str(), "%Y-%m-%d_%H:%M", &tm);
	
	// Note the result of this conversion can be off by one hour due to daylight savings time.
	// I haven't found a way to say "just interpret this string as the time in the local time
	// zone" and have it give the expected results.  I can change the time zone before making
	// the conversion, which will work if I print the time then, but when I reset the time
	// zone (to avoid screwing up the other time_points used in this program), it breaks the
	// time read here again.  So we're going to live with it being off by an hour (maybe) for
	// now (TODO).
	return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

bool OilChecker::WriteLogCreatedDate(const std::string& fileName, UString::OStream& log)
{
	std::ofstream file(fileName);
	if (!file.is_open())
	{
		log << "Failed to open '" << fileName << "' for output" << std::endl;
		return false;
	}
	
	file << GetTimestamp();
	return true;
}
