#ifndef DEMO_ARTNETWINDOW_H
#define DEMO_ARTNETWINDOW_H

#include <set>
#include <string>
#include <boost/chrono/system_clocks.hpp>
#include <boost/optional.hpp>
#include <boost/thread/mutex.hpp>
#include <queue>
#include <functional>

class ArtnetWindow {
    std::set<std::string> controllers;
    std::string selectedInterface;
    bool anySelected = true;

    inline void changeTriggered();

    boost::optional<boost::chrono::steady_clock::time_point> ledTimer;
    bool healthy = false;

    std::queue<std::string> pendingDevices;
    std::function<void(bool, const std::string&)> deviceCallback;

    boost::mutex health_mtx_;
    boost::mutex devices_mtx_;
    boost::mutex device_callback_mtx_;
    ArtnetWindow(const ArtnetWindow&); // no copy constructor, due to mutexes
public:
    ArtnetWindow();
    void draw();

    void notifyPacketReceived();
    void pushController(const std::string &address);

    /**
     * @return A pair of (anySelected, selectedInterface)
     */
    std::pair<bool, std::string> getSelection();

    void setDeviceCallback(const std::function<void(bool, const std::string &)> &deviceCallback);
};


#endif //DEMO_ARTNETWINDOW_H
