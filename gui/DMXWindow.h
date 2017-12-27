#ifndef DEMO_DMXWINDOW_H
#define DEMO_DMXWINDOW_H

#include "../DMXBucket.h"

class DMXWindow {
    DMXBucket &dmxBucket;

	int selectedChannel = 0;
	int selectedChannelValueSlider = 0;
public:
    void draw();

    DMXWindow(DMXBucket &dmxBucket);
};


#endif //DEMO_DMXWINDOW_H
