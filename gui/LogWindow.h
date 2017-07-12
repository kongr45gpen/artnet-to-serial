#ifndef DEMO_LOGWINDOW_H
#define DEMO_LOGWINDOW_H

#include <list>
#include <vector>
#include <array>
#include <imgui.h>
#include <boost/circular_buffer.hpp>

using std::list;
using std::string;
using std::pair;
using std::array;

class LogWindow {
private:
    boost::circular_buffer_space_optimized<pair<ImVec4, string> > items;
    bool scrollToBottom = false;
    int comboItem = 1;
public:
    LogWindow();
    void draw();

    void addEntry(string text, array<float, 3> colour);
};


#endif //DEMO_LOGWINDOW_H
