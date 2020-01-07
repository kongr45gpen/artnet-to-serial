#ifndef DEMO_DATASTATISTICS_H
#define DEMO_DATASTATISTICS_H

#include <boost/chrono.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>

class DataStatistics {
    float oldAverage = 0.0f;


    int sum = 0;
    int count = 0;

    boost::chrono::steady_clock::time_point timer;
    boost::chrono::duration<int, boost::milli> duration;

    boost::mutex mtx_;

    void updateMetrics();
public:
    explicit DataStatistics(const boost::chrono::duration<int, boost::milli> &duration = boost::chrono::seconds(1));

    inline void addOne() {
        boost::lock_guard<boost::mutex> guard(mtx_);
        sum++;
        count++;
    };
    inline void add(int number) {
        boost::lock_guard<boost::mutex> guard(mtx_);
        sum += number;
        count++;
    };

    float getAverage();
};


#endif //DEMO_DATASTATISTICS_H
