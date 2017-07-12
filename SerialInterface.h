#ifndef ARTNETTOSERIAL_SERIALINTERFACE_H
#define ARTNETTOSERIAL_SERIALINTERFACE_H


#include <vector>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include "gui/ActivityLED.h"
#include "DataStatistics.h"
#include "DMXBucket.h"

class SerialInterface {
    std::shared_ptr<boost::asio::io_service> io;
    std::shared_ptr<boost::asio::serial_port> serial;

    std::shared_ptr<ActivityLED> led;

    DataStatistics stats;

    /**
     * Poll an interface too see if it's connected
     * @param device
     */
    static bool testWorking(std::string device);

    boost::mutex mtx_;
public:
    SerialInterface();
    virtual ~SerialInterface();

    void setLed(const std::shared_ptr<ActivityLED> &led);

    void connect(std::string port, unsigned int baud_rate = 230400);
    void disconnect();

    /**
     * Send DMX output to the serial interface
     * @todo Oversight parameters
     */
    void write(const std::array<uint8_t, 512> &dmxValues);

    void test();
    static std::vector<std::string> listInterfaces();

    DataStatistics &getStats();
};


#endif //ARTNETTOSERIAL_SERIALINTERFACE_H
