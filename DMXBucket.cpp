#include "DMXBucket.h"

#include <algorithm>
#include <boost/thread/lock_guard.hpp>

std::array<uint8_t, 512> DMXBucket::getData() {
    boost::lock_guard<boost::mutex> guard(data_mtx_);
    return data;
}

DMXBucket::DMXBucket() {
    data = {0}; // Initialize all DMX values to zero
}
