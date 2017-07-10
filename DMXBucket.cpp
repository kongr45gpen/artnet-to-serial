#include "DMXBucket.h"

#include <algorithm>

std::array<uint8_t, 512> DMXBucket::getData() {
    return data;
}

DMXBucket::DMXBucket() {
    data = {0}; // Initialize all DMX values to zero
}
