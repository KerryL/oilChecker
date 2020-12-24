// File:  oilChecker.cpp
// Date:  12/22/2020
// Auth:  K. Loux
// Desc:  Oil checker class.

// Local headers
#include "oilChecker.h"

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
				if (!SendLowOilLevelEmail())
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
	// TODO
	return false;
}

bool OilChecker::GetTemperature(double& temperature) const
{
	log << "Reading temperature sensor" << std::endl;
	// TODO
	return false;
}

bool OilChecker::SendSummaryEmail() const
{
	log << "Sending summary email" << std::endl;
	// TODO
	return false;
}

bool OilChecker::SendLowOilLevelEmail() const
{
	log << "Sending low level warning email" << std::endl;
	// TODO
	return false;
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
