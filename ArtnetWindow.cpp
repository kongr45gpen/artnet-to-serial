#include "ArtnetWindow.h"

#include <imgui.h>
#include <sstream>
#include <boost/log/trivial.hpp>
#include <boost/thread/lock_guard.hpp>

using boost::lock_guard;
using boost::mutex;

ArtnetWindow::ArtnetWindow() : receivingLED({0.2f, 1.0f, 0, 0.7f}, boost::chrono::milliseconds(50), "Receiving") {
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
    ImGui::SameLine(0, 300);
    receivingLED.draw();

    ImGui::Separator();

    auto oldSelection = getSelection();
    bool selected;

    ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 0.78f, 0.58f, 1.0f});
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

void ArtnetWindow::pushController(const std::string &address) {
    lock_guard<mutex> guard(devices_mtx_);
    pendingDevices.push(address);
}

void ArtnetWindow::setDeviceCallback(const std::function<void(bool, const std::string &)> &deviceCallback) {
    lock_guard<mutex> guard(device_callback_mtx_);
    ArtnetWindow::deviceCallback = deviceCallback;
}
