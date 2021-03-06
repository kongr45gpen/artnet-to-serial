#include "DMXWindow.h"
#include <imgui.h>
#include <iomanip>
#include <boost/log/trivial.hpp>

/**
 * Double exponential seat-style easing
 * Takes two easing parameters (a,b) for the left and right parts of the input
 * All inputs should be between 0 and 1
 * Source: http://www.flong.com/texts/code/shapers_exp/
 */
float DMXWindow::doubleExponentialSeat(float x, float a, float b)
{
    // Clamp a, b values
    float epsilon = 0.00001;
    float min_param_a = 0.0 + epsilon;
    float max_param_a = 1.0 - epsilon;
    a = std::min(max_param_a, std::max(min_param_a, a));
    b = std::min(max_param_a, std::max(min_param_a, b));

    if (x <= 0.5f) {
        return (powf(2.0f * x, 1 - a)) / 2.0;
    } else {
        return 1.0f - (powf(2.0f * (1.0f - x), 1 - b)) / 2.0f;
    }
}

void DMXWindow::draw() {
    ImGui::SetNextWindowSize(ImVec2(520, 700), ImGuiCond_FirstUseEver);
    ImGui::Begin("DMX Output");

	// ImGuiWindowFlags_HorizontalScrollbar
	ImGui::BeginChild("Sub1", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowHeight() - 85), false, 0);
    ImGui::Columns(11, "mycolumns3", false);  // 3-ways, no border
    ImGui::NextColumn();
    ImGui::PushStyleColor(ImGuiCol_Text, { 0.85f, 0.95f, 1.0f, 1.0f });
    for (int i = 0; i < 10; i++) {
        ImGui::Text("%d", i);
        ImGui::NextColumn();
    }
    ImGui::PopStyleColor();
    ImGui::Separator();

    std::array<uint8_t, 512> data = dmxBucket.getData();

    for (int i = 0; i < 512; i++) {
        if (i % 10 == 0) {
            ImGui::PushStyleColor(ImGuiCol_Text, { 0.85f, 0.95f, 1.0f, 1.0f });
            ImGui::Text("%d",i);
            ImGui::PopStyleColor();
            ImGui::NextColumn();

            if (i == 0) {
                ImGui::NextColumn();
                i++;
            }
        }

        std::ostringstream ss;
        ss << std::setw(3) << std::setfill('0') << (int) (data[i-1]);

        float brightness = data[i-1]/255.0f; // Brightness in a scale from 0 - 1
        brightness = doubleExponentialSeat(brightness, 0.7, 0.1);
        brightness = 0.8f * brightness; // Brightness normalised for display

		if (selectedChannel == i - 1) {
			// Linear interpolation to increase the brightness so that we can see the button is selected
			// even if the DMX output is off
			brightness = 0.5f + 0.5f * brightness;
			ImGui::PushStyleColor(ImGuiCol_Button, (const ImVec4)ImColor::HSV(0.41f, 0.7f, brightness));
		} else {
			ImGui::PushStyleColor(ImGuiCol_Button, (const ImVec4)ImColor::HSV(0.3f, 0.8f, brightness));
		}
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (const ImVec4)ImColor::HSV(0.15f, 0.9f, std::max(0.6f,brightness)));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (const ImVec4) ImColor::HSV(0.1f, 1.0f, std::max(0.6f,brightness)));
		ImGui::PushID(i);
		if (ImGui::Button(ss.str().c_str())) {
			// The button was pressed; update the selection
			selectedChannel = i - 1;
		}
		ImGui::PopID();
        ImGui::PopStyleColor(3);

        ImGui::NextColumn();
    }
    ImGui::Columns(1);
	ImGui::EndChild();

	ImGui::Separator();

	ImGui::Columns(3, "options");
	if (ImGui::Button("Reset All")) {
        dmxBucket.clearData();
        BOOST_LOG_TRIVIAL(debug) << "Cleared all data";
        if (serialUpdater) {
            serialUpdater->announceDataReady();
        }
	}
	ImGui::SameLine();
	if (ImGui::Button("Randomise")) {
		dmxBucket.setRandomData();
        BOOST_LOG_TRIVIAL(debug) << "Randomised all data";
        if (serialUpdater) {
            serialUpdater->announceDataReady();
        }
	}
    if (ImGui::Button("Force Update")) {
        if (serialUpdater) {
            serialUpdater->announceDataReady();
            BOOST_LOG_TRIVIAL(debug) << "Force updating all data";
        }
    }
	
	ImGui::NextColumn();
	ImGui::Text("Selected: Channel %d", selectedChannel + 1);
	ImGui::Text("Value: %3d  (%3d%%)", data[selectedChannel], (100*data[selectedChannel])/255 );

    bool updated = false;
	ImGui::NextColumn();
	ImGui::PushItemWidth(100);
	if (ImGui::SliderInt("", &selectedChannelValueSlider, 0, 255)) {
        updated = true;
    }
	ImGui::SameLine();
	if (ImGui::Button("Set")) {
        updated = true;
	}
	if (ImGui::Button("Off")) {
		selectedChannelValueSlider = 0;
        updated = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("1/4")) {
		selectedChannelValueSlider = 63;
        updated = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Full")) {
        selectedChannelValueSlider = 255;
        updated = true;
	}

    if (updated) {
        dmxBucket.setOneChannel(selectedChannel, static_cast<uint8_t>(selectedChannelValueSlider));
        if (serialUpdater) {
            serialUpdater->announceDataReady();
        }
        BOOST_LOG_TRIVIAL(debug) << "Manually setting DMX channel " << selectedChannel + 1 << " to " << selectedChannelValueSlider;
    }
	
	ImGui::Columns(1);

    ImGui::End();
}

DMXWindow::DMXWindow(DMXBucket &dmxBucket) : dmxBucket(dmxBucket) {}

void DMXWindow::setSerialUpdater(const std::shared_ptr<SerialThread::Updater> &serialUpdater) {
	this->serialUpdater = serialUpdater;
}
