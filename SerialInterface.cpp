#include "SerialInterface.h"

#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <memory>
#include <boost/log/trivial.hpp>
#include <iomanip>
#include "boost/filesystem.hpp"
#include "DMXBucket.h"
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>

using namespace std;
using namespace boost::filesystem;
using namespace boost::asio;
using boost::lock_guard;
using boost::mutex;

vector<std::string> SerialInterface::listInterfaces() {
    vector<string> ifaces;

    try {
        path ttys("/sys/class/tty/");
        directory_iterator end_itr; // default construction yields past-the-end

        for (directory_iterator itr(ttys); itr != end_itr; ++itr) {
            // If the device subdirectory exists, then this is a valid serial device
            if (exists(itr->path() / "device")) { // magic - device subdirectory
                std::string deviceName = "/dev/" + itr->path().leaf().string();
                if (!testWorking(deviceName)) { // Don't show the serial device if we can't connect to it
                    continue;
                }

                ifaces.push_back(itr->path().leaf().string());
            }
        }
    } catch (boost::filesystem::filesystem_error &e) {
        BOOST_LOG_TRIVIAL(error) << "Could not fetch list of serial interfaces: " << e.what();
    }

    return ifaces;
}

SerialInterface::SerialInterface() : stats(boost::chrono::milliseconds(500)) {

}

void SerialInterface::connect(std::string port, unsigned int baud_rate) {
    lock_guard<mutex> guard(mtx_);
    try {
        if (serial && serial->is_open()) {
            disconnect();
        }
        BOOST_LOG_TRIVIAL(debug) << "Opening serial interface " << port << " [" << baud_rate << ']';

        io = std::shared_ptr<io_service>(new io_service);
        serial = std::make_shared<serial_port>(*io, port);
        serial->set_option(serial_port_base::baud_rate(baud_rate));
    } catch (boost::system::system_error &e) {
        BOOST_LOG_TRIVIAL(error) << "Unable to open interface " << port << ": " << e.what();
    }
}

void SerialInterface::disconnect() {
    lock_guard<mutex> guard(mtx_);
    if (serial && serial->is_open()) {
        BOOST_LOG_TRIVIAL(debug) << "Closing serial interface";
        try {
            serial->cancel();
            serial->close();
        } catch(...) {
            BOOST_LOG_TRIVIAL(error) << "Unable to close serial interface";
        }
    }
}

void SerialInterface::test() {
    // TODO: Implement this
}

bool SerialInterface::testWorking(std::string device) {
    static io_service io;

    BOOST_LOG_TRIVIAL(trace) << "Polling interface " << device;
    try {
        serial_port port = serial_port(io, device);
    } catch (boost::system::system_error &e) {
        BOOST_LOG_TRIVIAL(trace) << "Ignoring interface " << device << ": " << e.what();
        return false;
    }

    return true;
}

SerialInterface::~SerialInterface() {
    disconnect();
}

void SerialInterface::write(const std::array<uint8_t, 512> &dmxValues) {
    lock_guard<mutex> guard(mtx_);
    BOOST_LOG_TRIVIAL(trace) << "Writing to serial port";

    std::ostringstream ss;
    ss << 'f';

    for (auto &val : dmxValues) {
        ss << val;
    }

    try {
        if (!serial || !serial->is_open()) {
            BOOST_LOG_TRIVIAL(error) << "Serial port is not open";
            return;
        }

        std::string data = ss.str();

        boost::asio::write(*serial, boost::asio::buffer(data.c_str(), data.length()));

        stats.add((int) data.length());
        if (led) led->announce();
    } catch (boost::system::system_error &e) {
        BOOST_LOG_TRIVIAL(error) << "Error writing to serial port: " << e.what();
    }


}

void SerialInterface::setLed(const shared_ptr<ActivityLED> &led) {
    SerialInterface::led = led;
}

DataStatistics &SerialInterface::getStats() {
    return stats;
}
