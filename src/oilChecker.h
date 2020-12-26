// File:  oilChecker.h
// Date:  12/22/2020
// Auth:  K. Loux
// Desc:  Oil checker class.

#ifndef OIL_CHECKER_H_
#define OIL_CHECKER_H_

// Local headers
#include "oilCheckerConfig.h"
#include "utilities/uString.h"
#include "email/emailSender.h"

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
	static const std::string oilLogFileName;
	static const std::string temperatureLogFileName;
	static const std::string oilLogCreatedDateFileName;
	static const std::string temperatureLogCreatedDateFileName;
	
	OilCheckerConfig config;
	UString::OStream& log;
	
	std::chrono::system_clock::time_point oilLogCreatedDate;
	std::chrono::system_clock::time_point temperatureLogCreatedDate;

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
	bool SendLowOilLevelEmail(const double& volumeRemaining) const;
	bool SendNewLogFileEmail(const std::string& oldLogFileName) const;

	bool WriteOilLogData(const VolumeDistance& values) const;
	bool WriteTemperatureLogData(const double& temperature) const;

	template<typename T>
	struct DataPoint
	{
		DataPoint() = default;
		DataPoint(const std::chrono::system_clock::time_point& t, T& v) : t(t), v(v) {}

		std::chrono::system_clock::time_point t;
		T v;
	};

	typedef DataPoint<double> TemperatureDataPoint;
	typedef DataPoint<VolumeDistance> OilDataPoint;

	std::vector<TemperatureDataPoint> temperatureData;
	std::vector<OilDataPoint> oilData;

	void BuildEmailEssentials(EmailSender::LoginInfo& loginInfo, std::vector<EmailSender::AddressInfo>& recipients) const;
	
	static std::string GetTimestamp();
	static std::string GetTimestamp(const std::chrono::system_clock::time_point& now);
	static std::chrono::system_clock::time_point ReadLogCreatedDate(const std::string& fileName, UString::OStream& log);
	static bool WriteLogCreatedDate(const std::string& fileName, UString::OStream& log);
};

#endif// OIL_CHECKER_H_
