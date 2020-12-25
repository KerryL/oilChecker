// File:  oilChecker.cpp
// Date:  12/22/2020
// Auth:  K. Loux
// Desc:  Oil checker class.

// Local headers
#include "oilChecker.h"
#include "rpi/ds18b20Sensor.h"
#include "email/oAuth2Interface.h"

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

			oilData.push_back(OilDataPoint(std::chrono::steady_clock::now(), values));

			// TODO:  If it's time to start new log file, email log file
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

			temperatureData.push_back(TemperatureDataPoint(std::chrono::steady_clock::now(), temperature));

			// TODO:  If it's time to start new log file, email log file
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
	values.volume = 20.0;
	// TODO
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
	// TODO
	return false;
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
	EmailSender sender("Low Oil Level Detected", ss.str(), std::string(), recipients, loginInfo, false, true, log);
	if (!sender.Send())
		return false;

	log << "Successfully sent low-level warning email" << std::endl;
	return true;
}

bool OilChecker::SendNewLogFileEmail() const
{
	log << "Sending log file complete email" << std::endl;
	// TODO
	return false;
}

bool OilChecker::WriteOilLogData(const VolumeDistance& values) const
{
	log << "Adding oil data to log" << std::endl;
	// TODO
	return false;
}

bool OilChecker::WriteTemperatureLogData(const double& temperature) const
{
	log << "Adding temperature data to log" << std::endl;
	// TODO
	return false;
}

void OilChecker::BuildEmailEssentials(EmailSender::LoginInfo& loginInfo, std::vector<EmailSender::AddressInfo>& recipients) const
{
	loginInfo.smtpUrl = config.email.stmpUrl;
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
