#include "ArtnetWindow.h"

#include <imgui.h>
#include <sstream>
#include <boost/log/trivial.hpp>
#include <boost/thread/lock_guard.hpp>

using boost::lock_guard;
using boost::mutex;

ArtnetWindow::ArtnetWindow() : health_mtx_() {
    controllers.insert("127.0.0.1");
}

void ArtnetWindow::draw() {
    // Handle new entries in the queue
    devices_mtx_.lock();
    while (!pendingDevices.empty()) {
        std::string &device = pendingDevices.front();
        controllers.insert(device);
        pendingDevices.pop();
    }
    devices_mtx_.unlock();

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("ArtNet");

    ImGui::Text("ArtNet Controllers:");

    bool enabled;

    // Show the healthy LED for a number of milliseconds
    health_mtx_.lock();
    if (healthy) {
        if (ledTimer.is_initialized()) {
            if (boost::chrono::steady_clock::now() - ledTimer.get() > boost::chrono::milliseconds(50)) {
                ledTimer.reset();
                healthy = false;
            }
        } else {
            ledTimer = boost::chrono::steady_clock::now();
        }
    }
    enabled = healthy;
    health_mtx_.unlock();

    ImGui::SameLine(0, 300);
    ImGui::PushStyleColor(ImGuiCol_CheckMark, {0.2, 1.0, 0, 0.7});
    ImGui::Checkbox("Receiving", &enabled); // TODO: Make this non-selectable
    ImGui::PopStyleColor();

    ImGui::Separator();

    auto oldSelection = getSelection();
    bool selected;

    ImGui::PushStyleColor(ImGuiCol_Text, {1.0, 0.78, 0.58, 1.0});
    if (ImGui::Selectable("Any\n ", &anySelected) && !anySelected) {
        anySelected = true; // don't allow deselecting
    }
    ImGui::PopStyleColor();

    for (const auto &controller : controllers) {
        std::ostringstream ss;
        ss << controller << "\n ";

        selected = (selectedInterface == controller && !anySelected);

        if (ImGui::Selectable(ss.str().c_str(), &selected) && selected) {
            selectedInterface = controller;
            anySelected = false;
        }
    }

    if (oldSelection != getSelection()) {
        changeTriggered();
    }

    ImGui::End();
}

inline void ArtnetWindow::changeTriggered() {
    lock_guard<mutex> guard(device_callback_mtx_);
    BOOST_LOG_TRIVIAL(debug) << "Interface changing to: " << ((anySelected) ? "ANY" : selectedInterface);
    if (deviceCallback) {
        deviceCallback(anySelected, selectedInterface);
    }
}

std::pair<bool, std::string> ArtnetWindow::getSelection() {
    return std::pair<bool, std::string>(anySelected, selectedInterface);
}

void ArtnetWindow::notifyPacketReceived() {
    lock_guard<mutex> guard(health_mtx_); // lock the health variable before writing to it
    healthy = true;
}

void ArtnetWindow::pushController(const std::string &address) {
    lock_guard<mutex> guard(devices_mtx_);
    pendingDevices.push(address);
}

void ArtnetWindow::setDeviceCallback(const std::function<void(bool, const std::string &)> &deviceCallback) {
    lock_guard<mutex> guard(device_callback_mtx_);
    ArtnetWindow::deviceCallback = deviceCallback;
}
