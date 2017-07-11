#include <boost/thread/lock_guard.hpp>
#include "ActivityLED.h"

ActivityLED::ActivityLED(const ImVec4 &colour, const boost::chrono::duration<int, boost::milli> &duration, const char *label) : colour(
        colour), duration(duration), label(label) {}

void ActivityLED::draw() {
    announce_mtx_.lock();
    if (enabled) {
        if (timer.is_initialized()) {
            if (boost::chrono::steady_clock::now() - timer.get() > duration) {
                timer.reset();
                enabled = false;
            }
        } else {
            timer = boost::chrono::steady_clock::now();
        }
    }
    visible = enabled;
    announce_mtx_.unlock();

    ImGui::PushStyleColor(ImGuiCol_CheckMark, colour);
    ImGui::Checkbox(label, &visible); // TODO: Make this non-selectable
    ImGui::PopStyleColor();
}

void ActivityLED::announce() {
    boost::lock_guard<boost::mutex> guard(announce_mtx_); // lock the health variable before writing to it
    enabled = true;
}
