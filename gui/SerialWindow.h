#ifndef DEMO_SERIALWINDOW_H
#define DEMO_SERIALWINDOW_H

#include <vector>
#include <string>
#include <boost/optional.hpp>
#include "../SerialInterface.h"
#include "ActivityLED.h"

class SerialWindow {
    std::string ifaces;
    std::vector<std::string> devices;

    std::shared_ptr<SerialInterface> serialInterface;
    std::shared_ptr<ActivityLED> sendingLED;

    int item; // The selected item in the list

    void refreshInterfaces();
public:
    SerialWindow(std::shared_ptr<SerialInterface> serialInterface);

    void draw();
};


#endif //DEMO_SERIALWINDOW_H
