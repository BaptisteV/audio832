#pragma once
#include <Arduino.h>

class PerfWatcher
{
public:
    PerfWatcher() : startTime(0),
                    lastDurationUs(0),
                    totalDurationUs(0),
                    sampleCount(0)
    {
    }

    inline void start()
    {
        startTime = micros();
    }

    inline void stop()
    {
        unsigned long endTime = micros();
        lastDurationUs = endTime - startTime;
        totalDurationUs += lastDurationUs;
        sampleCount++;
    }

    // Convert to milliseconds
    float getLastMs() const
    {
        return lastDurationUs / 1000.0f;
    }

    float getMeanMs() const
    {
        if (sampleCount == 0)
            return 0;
        return (totalDurationUs / 1000.0f) / (float)sampleCount;
    }

    float getAverageMs() const
    {
        return getMeanMs(); // alias for readability
    }

    String printMeanAndAverage() const
    {
        String s;
        s += "Last: " + String(getLastMs(), 3) + " ms, ";
        s += "Mean: " + String(getMeanMs(), 3) + " ms, ";
        s += "Average: " + String(getAverageMs(), 3) + " ms, ";
        s += "Samples: " + String(sampleCount);
        return s;
    }

    unsigned long getSampleCount()
    {
        return sampleCount;
    }

private:
    unsigned long startTime;
    unsigned long lastDurationUs;  // stored in microseconds
    unsigned long totalDurationUs; // accumulated microseconds
    unsigned long sampleCount;
};
