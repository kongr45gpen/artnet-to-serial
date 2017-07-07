#ifndef DEMO_LOGWINDOW_H
#define DEMO_LOGWINDOW_H

#include <string>
#include <vector>
#include <array>
#include <imgui.h>

using std::vector;
using std::string;
using std::pair;
using std::array;

class LogWindow {
private:
    vector<pair<ImVec4, string> > items;
    bool scrollToBottom = false;
    int comboItem = 0;
public:
    void init();
    void draw();

    void addEntry(string text, array<float, 3> colour);
};


#endif //DEMO_LOGWINDOW_H
