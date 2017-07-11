#ifndef DEMO_ACTIVITYLED_H
#define DEMO_ACTIVITYLED_H

#include <boost/thread/mutex.hpp>
#include <boost/optional.hpp>
#include <imgui.h>

class ActivityLED {
    boost::optional<boost::chrono::steady_clock::time_point> timer;

    bool enabled = false;
    bool visible = false;
    boost::mutex announce_mtx_;

    ImVec4 colour;
    boost::chrono::duration<int, boost::milli> duration;
    const char* label;
public:
    void draw();

    /**
     * Enable the LED for a short amount of time
     */
    void announce();

    ActivityLED(const ImVec4 &colour, const boost::chrono::duration<int, boost::milli> &duration, const char *label);
};


#endif //DEMO_ACTIVITYLED_H
