#include <imgui.h>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <iostream>
#include "LogWindow.h"

LogWindow::LogWindow() : items(400) {
    items.push_back(std::make_pair(ImVec4(1.0,0.2,0.1,1.0), "Hello"));
}

void LogWindow::draw() {
    ImGui::SetNextWindowSize(ImVec2(520,600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Log")) {
        ImGui::End();
        return;
    };

    // TODO: display items starting from the bottom


    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
    static ImGuiTextFilter filter;
    filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);

    ImGui::SameLine(0, 100);
    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
    if (ImGui::Combo("min level", &comboItem, "trace\0debug\0info\0warning\0error\0fatal\0\0")) {
        boost::log::trivial::severity_level level;
        switch (comboItem) {
            case 0: level = boost::log::trivial::trace; break;
            case 1: level = boost::log::trivial::debug; break;
            case 2: default: level = boost::log::trivial::info; break;
            case 3: level = boost::log::trivial::warning; break;
            case 4: level = boost::log::trivial::error; break;
            case 5: level = boost::log::trivial::fatal; break;
        }
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= level);
    }
    ImGui::PopStyleVar();

    ImGui::Separator();

    ImGui::BeginChild("ScrollingRegion", ImVec2(0,-ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
//    if (ImGui::BeginPopupContextWindow())
//    {
//        if (ImGui::Selectable("Clear")) {}
//        ImGui::EndPopup();
//    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1)); // Tighten spacing
    // TODO: Use a mutex or something else for items
    for (auto &it : items) {
        const char* item = it.second.c_str();
        if (!filter.PassFilter(item))
            continue;
        ImVec4 col = it.first;
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        ImGui::TextUnformatted(item);
        ImGui::PopStyleColor();
    }
    if (scrollToBottom)
        ImGui::SetScrollHere();
    scrollToBottom = false;
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    ImGui::End();
}

void LogWindow::addEntry(std::string text, std::array<float, 3> colour) {
    items.push_back(std::make_pair(ImVec4(colour[0], colour[1], colour[2], 1.0f), text));
    scrollToBottom = true;
}
