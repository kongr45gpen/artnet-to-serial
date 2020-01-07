#ifndef DEMO_ARTNETWINDOW_H
#define DEMO_ARTNETWINDOW_H

#include <set>
#include <string>
#include <boost/chrono/system_clocks.hpp>
#include <boost/optional.hpp>
#include <boost/thread/mutex.hpp>
#include <queue>
#include <functional>
#include "ActivityLED.h"
#include "../DataStatistics.h"

class ArtnetWindow {
    std::map<std::string, std::string> controllers; // name -> description map
    std::string selectedInterface;
    bool anySelected = true;
    bool dmxEnabled = true;

    inline void changeTriggered();

    std::queue<std::string> pendingDevices;
    std::function<void(bool, const std::string&)> deviceCallback;
    std::function<void(bool)> enabledCallback;

    ActivityLED receivingLED;

    boost::mutex devices_mtx_;
    boost::mutex callback_mtx_;
    boost::mutex statistics_mtx_;
    ArtnetWindow(const ArtnetWindow&); // no copy constructor, due to mutexes
public:
    DataStatistics statistics;

    ArtnetWindow();
    void draw();

    inline void notifyPacketReceived() {
        receivingLED.announce();
    };
    void pushController(const std::string &address);
    void pushControllerDescription(const std::string &address, const std::string &description);

    /**
     * @return A pair of (anySelected, selectedInterface)
     */
    std::pair<bool, std::string> getSelection();

    void setDeviceCallback(const std::function<void(bool, const std::string &)> &deviceCallback);
    void setEnabledCallback(const std::function<void(bool)> &enabledCallback);
};


#endif //DEMO_ARTNETWINDOW_H
