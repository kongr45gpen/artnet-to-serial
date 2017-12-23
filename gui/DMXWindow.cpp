#include "DMXWindow.h"
#include <imgui.h>
#include <iomanip>

void DMXWindow::draw() {
    ImGui::SetNextWindowSize(ImVec2(520, 800), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("DMX Output");

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

    for (int i = 0; i < 512; i++)
    {
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

        ImGui::PushStyleColor(ImGuiCol_Button, (const ImVec4) ImColor::HSV(0.3f, 0.8f, brightness));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (const ImVec4)ImColor::HSV(0.15f, 0.9f, std::max(0.6f,brightness)));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (const ImVec4) ImColor::HSV(0.1f, 1.0f, std::max(0.6f,brightness)));
        ImGui::Button(ss.str().c_str());
        ImGui::PopStyleColor(3);


        ImGui::NextColumn();
    }
    ImGui::Columns(1);
    ImGui::Separator();

    ImGui::End();
}

DMXWindow::DMXWindow(DMXBucket &dmxBucket) : dmxBucket(dmxBucket) {}
