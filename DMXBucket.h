#ifndef DEMO_DMXBUCKET_H
#define DEMO_DMXBUCKET_H

#include <array>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>

class DMXBucket {
    std::array<uint8_t, 512> data;

    boost::mutex data_mtx_;
public:
    template<class InputIt> void setData (const InputIt &first, const InputIt &last) {
        boost::lock_guard<boost::mutex> guard(data_mtx_);
        std::copy(first, last, data.begin());
    }
    std::array<uint8_t, 512> getData();

	void clearData();
	void setOneChannel(int channel, uint8_t value);
	void setRandomData();

    DMXBucket();
};


#endif //DEMO_DMXBUCKET_H
