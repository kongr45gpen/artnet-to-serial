#ifndef ARTNETTOSERIAL_SERIALINTERFACE_H
#define ARTNETTOSERIAL_SERIALINTERFACE_H


#include <vector>
#include <string>
#include <boost/asio.hpp>

class SerialInterface {
public:
    SerialInterface(std::string port, unsigned int baud_rate = 230400);
    virtual ~SerialInterface();

    /**
     * Send DMX output to the serial interface
     * @param channel The DMX channel (1-512)
     * @param value The DMX value (0-255)
     */
    void write(int channel, int value);
    void test();

    static std::vector<std::string> listInterfaces();
private:
    std::shared_ptr<boost::asio::serial_port> serial;
    std::shared_ptr<boost::asio::io_service> io;
};


#endif //ARTNETTOSERIAL_SERIALINTERFACE_H
