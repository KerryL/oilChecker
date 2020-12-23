// File:  oilChecker.h
// Date:  12/22/2020
// Auth:  K. Loux
// Desc:  Oil checker class.

#ifndef OIL_CHECKER_H_
#define OIL_CHECKER_H_

// Local headers
#include "oilCheckerConfig.h"

// Standard C++ headers
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>

class OilChecker
{
public:
	OilChecker(const OilCheckerConfig& config, UString::OStream& log) : config(config), log(log) {}
	~OilChecker();

	void Run();

private:
	OilCheckerConfig config;
	UString::OStream& log;

	std::thread oilMeasurementThread;
	std::thread temperatureMeasurementThread;
	std::thread summaryUpdateThread;
	std::mutex activityMutex;

	std::mutex stopMutex;
	std::condition_variable stopCondition;
	std::atomic<bool> stopThreads = false;

	void SignalStop();

	// TODO:  Stop check duration so we don't wait days to quit in the event of an error (or can we use conditional event based on two conditions?)

	void OilMeasurementThreadEntry();
	void TemperatureMeasurementThreadEntry();
	void SummaryUpdateThreadEntry();

	struct VolumeDistance
	{
		double volume;// [gal]
		double distance;// [in]
	};

	bool GetRemainingOilVolume(VolumeDistance& values) const;
	bool GetTemperature(double& temperature) const;
	bool SendSummaryEmail() const;
	bool SendLowOilLevelEmail() const;
	bool SendNewLogFileEmail() const;

	bool WriteOilLogData(const VolumeDistance& values) const;
	bool WriteTemperatureLogData(const double& temperature) const;

	template<typename T>
	struct DataPoint
	{
		DataPoint() = default;
		DataPoint(const std::chrono::steady_clock::time_point& t, T& v) : t(t), v(v) {}

		std::chrono::steady_clock::time_point t;
		T v;
	};

	typedef DataPoint<double> TemperatureDataPoint;
	typedef DataPoint<VolumeDistance> OilDataPoint;

	std::vector<TemperatureDataPoint> temperatureData;
	std::vector<OilDataPoint> oilData;

	// TODO:  Event log file for errors, etc
};

#endif// OIL_CHECKER_H_
