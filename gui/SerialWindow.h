#ifndef DEMO_SERIALWINDOW_H
#define DEMO_SERIALWINDOW_H

#include <vector>
#include <string>
#include <boost/optional.hpp>
#include "../SerialInterface.h"

class SerialWindow {
    std::string ifaces;
    std::vector<std::string> devices;

    boost::optional<SerialInterface> serialInterface;

    int item; // The selected item in the list

    void refreshInterfaces();

public:
    void init();
    void draw();
};


#endif //DEMO_SERIALWINDOW_H
