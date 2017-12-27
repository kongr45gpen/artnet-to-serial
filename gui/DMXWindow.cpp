#include "DMXWindow.h"
#include <imgui.h>
#include <iomanip>

void DMXWindow::draw() {
    ImGui::SetNextWindowSize(ImVec2(520, 800), ImGuiSetCond_FirstUseEver);
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

        float brightness = 0.8f * data[i-1]/255.0f;

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
	}
	ImGui::SameLine();
	if (ImGui::Button("Randomise")) {
		dmxBucket.setRandomData();
	}
	
	ImGui::NextColumn();
	ImGui::Text("Selected: Channel %d", selectedChannel + 1);
	ImGui::Text("Value: %3d  (%3d%%)", data[selectedChannel], (100*data[selectedChannel])/255 );

	ImGui::NextColumn();
	ImGui::PushItemWidth(100);
	ImGui::SliderInt("", &selectedChannelValueSlider, 0, 255);
	ImGui::SameLine();
	if (ImGui::Button("Set")) {
		dmxBucket.setOneChannel(selectedChannel, static_cast<uint8_t>(selectedChannelValueSlider));
	}

	if (ImGui::Button("Off")) {
		dmxBucket.setOneChannel(selectedChannel, static_cast<uint8_t>(selectedChannelValueSlider = 0));
	}
	ImGui::SameLine();
	if (ImGui::Button("1/2")) {
		dmxBucket.setOneChannel(selectedChannel, static_cast<uint8_t>(selectedChannelValueSlider = 127));
	}
	ImGui::SameLine();
	if (ImGui::Button("Full")) {
		dmxBucket.setOneChannel(selectedChannel, static_cast<uint8_t>(selectedChannelValueSlider = 255));
	}
	
	ImGui::Columns(1);

    ImGui::End();
}

DMXWindow::DMXWindow(DMXBucket &dmxBucket) : dmxBucket(dmxBucket) {}
