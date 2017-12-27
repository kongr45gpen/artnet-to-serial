#ifndef DEMO_ARTNETWINDOW_H
#define DEMO_ARTNETWINDOW_H

#include <set>
#include <string>
#include <boost/chrono/system_clocks.hpp>
#include <boost/optional.hpp>
#include <boost/thread/mutex.hpp>
#include <queue>
#include <functional>
#include "gui/ActivityLED.h"

class ArtnetWindow {
    std::map<std::string, std::string> controllers; // name -> description map
    std::string selectedInterface;
    bool anySelected = true;

    inline void changeTriggered();

    std::queue<std::string> pendingDevices;
    std::function<void(bool, const std::string&)> deviceCallback;

    ActivityLED receivingLED;

    boost::mutex devices_mtx_;
    boost::mutex device_callback_mtx_;
    ArtnetWindow(const ArtnetWindow&); // no copy constructor, due to mutexes
public:
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
};


#endif //DEMO_ARTNETWINDOW_H
