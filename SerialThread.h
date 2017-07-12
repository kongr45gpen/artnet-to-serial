#ifndef DEMO_SERIALTHREAD_H
#define DEMO_SERIALTHREAD_H

#include "SerialInterface.h"
#include "DMXBucket.h"
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

class SerialThread {
public:
    class Updater {
        // Class that can be used to update serial data whenever an artnet packet is fired
        boost::condition_variable cond;
        bool dataReady = false;
        boost::mutex ready_mtx_;

        friend SerialThread;
    public:
        void announceDataReady();
    };
private:

    std::shared_ptr<SerialInterface> serialInterface;
    std::shared_ptr<DMXBucket> dmxBucket;

    std::shared_ptr<Updater> updater;

public:
    SerialThread(const std::shared_ptr<SerialInterface> &serialInterface, const std::shared_ptr<DMXBucket> &dmxBucket,
                 const std::shared_ptr<Updater> &updater);

    void operator() ();


};


#endif //DEMO_SERIALTHREAD_H
