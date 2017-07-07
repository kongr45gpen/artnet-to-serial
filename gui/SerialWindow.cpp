#include <boost/algorithm/string/join.hpp>
#include <imgui.h>
#include <boost/log/trivial.hpp>
#include "SerialWindow.h"
#include "../SerialInterface.h"

void SerialWindow::init() {
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
        } else {
            BOOST_LOG_TRIVIAL(warning) << "No selected serial interface to test.";
        }
    }

    ImGui::Text("Select your Arduino's serial port:");

    static int baud = 230400;

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
    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
    ImGui::SliderInt("baud rate", &baud, baud, baud);

    ImGui::End();

}

void SerialWindow::refreshInterfaces() {
    devices = SerialInterface::listInterfaces();
    ifaces = boost::algorithm::join(devices, std::string("\0", 1));

    std::transform(devices.begin(), devices.end(), devices.begin(), [](std::string device)->std::string {
        return "/dev/" + device;
    });
}
