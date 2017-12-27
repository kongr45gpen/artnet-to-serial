#ifndef DEMO_DMXWINDOW_H
#define DEMO_DMXWINDOW_H

#include "../DMXBucket.h"
#include "../SerialThread.h"

class DMXWindow {
    DMXBucket &dmxBucket;

	int selectedChannel = 0;
	int selectedChannelValueSlider = 0;

	std::shared_ptr<SerialThread::Updater> serialUpdater;
public:
    void draw();

    DMXWindow(DMXBucket &dmxBucket);
	void setSerialUpdater(const std::shared_ptr<SerialThread::Updater> &serialUpdater);
};


#endif //DEMO_DMXWINDOW_H
