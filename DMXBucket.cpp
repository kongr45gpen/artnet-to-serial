#include "DMXBucket.h"

#include <algorithm>
#include <boost/thread/lock_guard.hpp>
#include <random>

std::array<uint8_t, 512> DMXBucket::getData() {
    boost::lock_guard<boost::mutex> guard(data_mtx_);
    return data;
}

DMXBucket::DMXBucket() {
	clearData(); // Initialize all DMX values to zero
}

void DMXBucket::clearData() {
	boost::lock_guard<boost::mutex> guard(data_mtx_);
	for (auto &datum : data) {
		datum = 0;
	}
}

void DMXBucket::setOneChannel(int channel, uint8_t value)
{
	boost::lock_guard<boost::mutex> guard(data_mtx_);

	if (channel < 0 || channel >= data.size()) {
		throw std::out_of_range("Specified channel is not a valid DMX channel");
	}

	data[channel] = value;
}

void DMXBucket::setRandomData()
{
	// A bit of an overkill
	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(0, 255);

	for (auto &datum : data) {
		datum = uni(rng);
	}
}