#ifndef DEMO_DMXBUCKET_H
#define DEMO_DMXBUCKET_H

#include <array>
#include <boost/thread/mutex.hpp>

class DMXBucket {
    std::array<uint8_t, 512> data;

    boost::mutex data_mtx_;
public:
    template<class InputIt> void setData (const InputIt &first, const InputIt &last) {
        std::copy(first, last, data.begin());
    };

    std::array<uint8_t, 512> getData();

    DMXBucket();
};


#endif //DEMO_DMXBUCKET_H
