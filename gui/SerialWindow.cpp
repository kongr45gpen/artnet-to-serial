#include <boost/algorithm/string/join.hpp>
#include <imgui.h>
#include <boost/log/trivial.hpp>
#include "SerialWindow.h"
#include "../SerialInterface.h"

SerialWindow::SerialWindow() : sendingLED({1.0f, 0.6f, 0.1f, 0.8f}, boost::chrono::milliseconds(200), "Sending") {
    refreshInterfaces();

    // Select the last interface
    serialInterface.emplace(devices.back());
    item = (int) (devices.size() - 1);
}

void SerialWindow::draw() {
    ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Arduino Serial Port");

    if (ImGui::Button("Refresh")) {
        refreshInterfaces();
    }
    ImGui::SameLine();
    if (ImGui::Button("Test")) {
        if (serialInterface.is_initialized()) {
            serialInterface->test();
            sendingLED.announce();
        } else {
            BOOST_LOG_TRIVIAL(warning) << "No selected serial interface to test.";
        }
    }
    ImGui::SameLine(0, 300);
    sendingLED.draw();

    ImGui::Text("Select your Arduino's serial port:");

    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.5f);
    if (ImGui::Combo("", &item, ifaces.c_str())) { // Combo using values packed in a single constant string (for really quick combo)
        // combo value changed
        BOOST_LOG_TRIVIAL(trace) << "Selected serial interface #" << item << " (" << devices[item] << ") from list";
        if (serialInterface.is_initialized()) {
            // Close the old serial interface
            serialInterface.reset();
        }
        serialInterface.emplace(devices[item]);
    }
    ImGui::SameLine(0, 50);
    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
//    ImGui::SliderInt("baud rate", &baud, baud, baud);
    ImGui::TextDisabled("baud rate: 230400");

    ImGui::End();

}

void SerialWindow::refreshInterfaces() {
    devices = SerialInterface::listInterfaces();
    ifaces = boost::algorithm::join(devices, std::string("\0", 1)) + '\0';
    // the trailing NULL byte (double \0) is necessary

    BOOST_LOG_TRIVIAL(trace) << "Found " << devices.size() << " functional interfaces";

    std::transform(devices.begin(), devices.end(), devices.begin(), [](std::string device)->std::string {
        return "/dev/" + device;
    });
}
