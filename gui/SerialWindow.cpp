#include <boost/algorithm/string/join.hpp>
#include <imgui.h>
#include <boost/log/trivial.hpp>
#include "SerialWindow.h"
#include "../SerialInterface.h"

SerialWindow::SerialWindow(std::shared_ptr<SerialInterface> serialInterface) : serialInterface(serialInterface) {
        refreshInterfaces();

        // Connect to the last interface
        if (!devices.empty()) {
            serialInterface->connect(devices.back().first, 460800);
        }
        item = (int) (devices.size() - 1);

        ImVec4 ledColour = {1.0f, 0.6f, 0.1f, 0.8f };
        sendingLED = std::make_shared<ActivityLED>(ledColour, boost::chrono::milliseconds(200), "Sending");
        serialInterface->setLed(sendingLED);
}

void SerialWindow::draw() {
    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Arduino Serial Port");

    if (ImGui::Button("Refresh")) {
        refreshInterfaces();
    }
	ImGui::SameLine();
	if (ImGui::Button("Try Reconnecting")) {
		serialInterface->disconnect();

		try {
			if (item >= devices.size()) {
				// Reset if we're too far away
				item = (int)(devices.size() - 1);
			}
			serialInterface->connect(devices.at(item).first);
		} catch (std::out_of_range) {
			BOOST_LOG_TRIVIAL(error) << "Unable to find a serial interface to reconnect to.";
		}
	}
    ImGui::SameLine();
    if (ImGui::Button("Test")) {
        serialInterface->test();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset Error")) {
        serialInterface->resetError();
    }
    ImGui::SameLine(0, 300);
    sendingLED->draw();

    ImGui::Text("Select your Arduino's serial port:");

    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.5f);
    if (ImGui::Combo("", &item,
                     ifaces.c_str())) { // Combo using values packed in a single constant string (for really quick combo)
        // combo value changed
        BOOST_LOG_TRIVIAL(trace) << "Selected serial interface #" << item << " (" << devices[item].first << ") from list";
        // TODO: Connect from another thread
        serialInterface->connect(devices[item].first);
    }
    ImGui::SameLine(0, 50);
    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
    ImGui::TextDisabled("baud rate: %d", serialInterface->getBaudRate());

	bool checkbox = true;
	if (serialInterface->isConnected()) {
		ImGui::PushStyleColor(ImGuiCol_CheckMark, {0.2f, 0.9f, 0.1f, 1.0f });
		ImGui::RadioButton("(connected)", &checkbox);
		ImGui::PopStyleColor();
	} else {
		ImGui::PushStyleColor(ImGuiCol_CheckMark, { 0.8f, 0.3f, 0.1f, 1.0f });
		ImGui::RadioButton("(error)", &checkbox);
		ImGui::PopStyleColor();
	}

    ImGui::Text("Data rate: %d bits/s", (int) (8 * serialInterface->getStats().getAverage()));

    ImGui::End();

}

void SerialWindow::refreshInterfaces() {
    devices = SerialInterface::listInterfaces();
	
	// Create list of devices compatible with imgui
	std::ostringstream ss;
	for (auto it : devices) {
		ss << it.second << std::string("\0", 1);
	}
	ifaces = ss.str();
	// the trailing NULL byte (double \0) is necessary
	
    BOOST_LOG_TRIVIAL(trace) << "Found " << devices.size() << " functional interfaces";
}
