#include <boost/thread/thread.hpp>
#include "SerialThread.h"

SerialThread::SerialThread(const std::shared_ptr<SerialInterface> &serialInterface,
                           const std::shared_ptr<DMXBucket> &dmxBucket, const std::shared_ptr<Updater> &updater)
        : serialInterface(serialInterface), dmxBucket(dmxBucket), updater(updater) {}

void SerialThread::operator()() {
    while (true) {
        boost::unique_lock<boost::mutex> lock(updater->ready_mtx_);
        /*while (!updater->dataReady) {
            updater->cond.wait(lock);
        }
        updater->dataReady = false;
        lock.unlock();*/
        boost::this_thread::sleep_for(boost::chrono::milliseconds(1000/30));

        serialInterface->write(dmxBucket->getData());
    }
}

void SerialThread::Updater::announceDataReady() {
    {
        boost::lock_guard<boost::mutex> lock(ready_mtx_);
        dataReady = true;
    }
    cond.notify_one();
}
