#include "DataStatistics.h"

DataStatistics::DataStatistics(const boost::chrono::duration<int, boost::milli> &duration) : duration(duration), timer(boost::chrono::steady_clock::now()) {}

float DataStatistics::getAverage() {
    if (boost::chrono::steady_clock::now() - timer > duration) {
        boost::lock_guard<boost::mutex> guard(mtx_);

        if (duration == boost::chrono::seconds(1)) {
            oldAverage = (float) sum;
        } else if (duration.count() == 0) {
            oldAverage = 0.0f;
        } else {
            oldAverage = (float) sum / (duration.count() / 1000.0f);
        }

        sum = 0;
        timer = boost::chrono::steady_clock::now();
    }

    return oldAverage;
}
