#pragma once
#include "common/utils.h"
#include "controller/logController.h"
#include <string>
#include <vector>
#include <chrono>

enum class TrackingType
{
    None = 0,
    Second = 1,
    Frame = 2
};

struct Tracker
{
    std::string name;
    unsigned long long startTime;
    unsigned long long accumulated;
    int id;
    bool bIsActive;
};

class ProfilerController
{
public:
    ProfilerController(LogController *logController);

    EXPORT int addTracker(std::string name);
    EXPORT void enableTrackingAvgFrame(int secondsPerRecord);
    EXPORT void enableTrackingAvgSecond(int secondsPerRecord);
    EXPORT TrackingType getTrackingType();
    EXPORT void frameSync();
    EXPORT int getRecordNumber();
    EXPORT int getFPS();
    EXPORT Tracker *getTracker(int id);

    EXPORT void setInactive(int id);
    EXPORT void setActive(int id);
    EXPORT bool isActive(int id);

    EXPORT void setStateName(std::string stateName);
    EXPORT std::string getStateName();

    EXPORT inline void startTracking(int trackId)
    {
        if (!trackingEnabled)
            return;

        auto tracker = getTracker(trackId);
        if (tracker)
        {
            auto now = std::chrono::system_clock::now();
            long long time = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();

            tracker->startTime = time;
        }
    }

    EXPORT inline void stopTracking(int trackId)
    {
        if (!trackingEnabled)
            return;

        auto tracker = getTracker(trackId);
        if (tracker)
        {
            auto now = std::chrono::system_clock::now();
            long long time = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();

            tracker->accumulated += (time - tracker->startTime);
        }
    }

protected:
    std::vector<Tracker *> trackers;
    int frameTracker = 0;
    int secondsPerRecord = 0;
    int secondsPassed = 0;
    bool trackingEnabled = false;
    int currentRecordNumber = 0;
    int avgFPS = 0;
    int framesCounted = 0;
    TrackingType trackingType = TrackingType::None;
    long long trackingTimeframeStart = 0;

    std::string stateName = "global";

private:
    LogController *logController = nullptr;
};
