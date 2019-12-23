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
    std::atomic_bool connected; // Used just for the indicator, might not be trustworthy

    std::shared_ptr<ActivityLED> led;

    DataStatistics stats;

    unsigned int baudRate = 0;

    /**
     * Poll an interface to see if it's connected
     * @param device
     */
    static bool testWorking(std::string device);

    // The signal that defines an operation
    const static uint8_t opSignal = 125;

    boost::mutex mtx_;
public:
    SerialInterface();
    virtual ~SerialInterface();

    void setLed(const std::shared_ptr<ActivityLED> &led);
    unsigned int getBaudRate() const {
        return baudRate;
    }

    void connect(std::string port, unsigned int baudRate = 500000);
    void disconnect();
	bool isConnected() const {
		return connected;
	};

    /**
     * Send DMX output to the serial interface
     * @todo Oversight parameters
     */
    void write(const std::array<uint8_t, 512> &dmxValues);

    void test();
    void resetError();
    static std::vector<std::pair<std::string, std::string> > listInterfaces();

    DataStatistics &getStats();
};


#endif //ARTNETTOSERIAL_SERIALINTERFACE_H
