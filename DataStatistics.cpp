#include "DataStatistics.h"

DataStatistics::DataStatistics(const boost::chrono::duration<int, boost::milli> &duration) : duration(duration), timer(boost::chrono::steady_clock::now()) {}

float DataStatistics::getAverage() {
    updateMetrics();

    return oldAverage;
}

void DataStatistics::updateMetrics() {
    // We assume that exactly duration time has passed since the last check
    if (boost::chrono::steady_clock::now() - timer > duration) {
        boost::lock_guard<boost::mutex> guard(mtx_);

        if (duration == boost::chrono::seconds(1)) {
            oldAverage = (float) sum;
        } else if (duration.count() == 0) {
            oldAverage = 0.0f;
        } else {
            oldAverage = (float) sum / (duration.count() / 1000.0f);
        }

        // Reset values; start counting again
        sum = 0;
        count = 0;
        timer = boost::chrono::steady_clock::now();
    }
}
