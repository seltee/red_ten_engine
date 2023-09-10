#include "profilerController.h"

ProfilerController::ProfilerController(LogController *logController)
{
    frameTracker = addTracker("FPS");
    this->logController = logController;
}

int ProfilerController::addTracker(std::string name)
{
    auto now = std::chrono::system_clock::now();
    long long time = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();

    Tracker *newTracker = new Tracker();
    newTracker->id = trackers.size();
    newTracker->bIsActive = true;
    newTracker->startTime = time;
    newTracker->accumulated = 0;
    newTracker->name = name;

    trackers.push_back(newTracker);
    return newTracker->id;
}

void ProfilerController::enableTrackingAvgFrame(int secondsPerRecord)
{
    trackingType = TrackingType::Frame;
    trackingEnabled = true;
    this->secondsPerRecord = secondsPerRecord;
    currentRecordNumber = 0;

    auto now = std::chrono::system_clock::now();
    long long time = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    trackingTimeframeStart = time;
}

void ProfilerController::enableTrackingAvgSecond(int secondsPerRecord)
{
    trackingType = TrackingType::Second;
    trackingEnabled = true;
    this->secondsPerRecord = secondsPerRecord;
    currentRecordNumber = 0;

    auto now = std::chrono::system_clock::now();
    long long time = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    trackingTimeframeStart = time;
}

TrackingType ProfilerController::getTrackingType()
{
    return trackingType;
}

void ProfilerController::frameSync()
{
    framesCounted++;
    if (trackingEnabled)
    {
        auto now = std::chrono::system_clock::now();
        long long time = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();

        if (time - trackingTimeframeStart > secondsPerRecord * 1000000)
        {
            trackingTimeframeStart = time;
            avgFPS = framesCounted / secondsPerRecord;
            currentRecordNumber++;
            logController->logff("--- profiler time frame %i (%s) ---", currentRecordNumber, stateName.c_str());
            logController->logff("--- average fps %i ---", avgFPS);

            for (auto &it : trackers)
            {
                if (it->bIsActive)
                {
                    if (trackingType == TrackingType::Second)
                        logController->logff("%s %ims (%ius) per second", it->name.c_str(), (it->accumulated / secondsPerRecord) / 1000, it->accumulated / secondsPerRecord);
                    else
                        logController->logff("%s %ims (%ius) per frame", it->name.c_str(), (it->accumulated / framesCounted) / 1000, it->accumulated / framesCounted);
                }
                it->accumulated = 0;
            }

            logController->logff("---\n");
            framesCounted = 0;
        }
    }
    else
    {
        auto fpsTracker = getTracker(frameTracker);
        auto now = std::chrono::system_clock::now();
        long long time = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
        long long difference = time - fpsTracker->startTime;
        if (difference > 1000000)
        {
            avgFPS = framesCounted;
            framesCounted = 0;
            fpsTracker->startTime += 1000000;
            logController->logConsole("FPS %i", avgFPS);
        }
    }
}

int ProfilerController::getRecordNumber()
{
    return currentRecordNumber;
}

int ProfilerController::getFPS()
{
    return avgFPS;
}

Tracker *ProfilerController::getTracker(int id)
{
    return trackers.at(id);
}

void ProfilerController::setInactive(int id)
{
    auto tracker = getTracker(id);
    if (tracker)
        tracker->bIsActive = false;
}

void ProfilerController::setActive(int id)
{
    auto tracker = getTracker(id);
    if (tracker)
        tracker->bIsActive = true;
}

bool ProfilerController::isActive(int id)
{
    auto tracker = getTracker(id);
    if (tracker)
        return tracker->bIsActive;
    return false;
}

void ProfilerController::setStateName(std::string stateName)
{
    this->stateName = stateName;
}

std::string ProfilerController::getStateName()
{
    return stateName;
}