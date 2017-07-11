#include "SerialInterface.h"

#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <memory>
#include <boost/log/trivial.hpp>
#include <iomanip>
#include "boost/filesystem.hpp"

using namespace std;
using namespace boost::filesystem;
using namespace boost::asio;

vector<std::string> SerialInterface::listInterfaces() {
    vector<string> ifaces;

    try {
        path ttys("/sys/class/tty/");
        directory_iterator end_itr; // default construction yields past-the-end

        for (directory_iterator itr(ttys); itr != end_itr; ++itr) {
            if (exists(itr->path() / "device")) { // magic - device subdirectory
                std::string deviceName = "/dev/" + itr->path().leaf().string();
                if (!testWorking(deviceName)) {
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

SerialInterface::SerialInterface(std::string port, unsigned int baud_rate) {
    BOOST_LOG_TRIVIAL(debug) << "Opening serial interface " << port << " [" << baud_rate << ']';

    try {
        io = std::shared_ptr<io_service>(new io_service);
        serial = std::make_shared<serial_port>(*io, port);
        serial->set_option(serial_port_base::baud_rate(baud_rate));
    } catch (boost::system::system_error &e) {
        BOOST_LOG_TRIVIAL(error) << "Unable to open interface " << port << ": " << e.what();
    }
}

void SerialInterface::test() {
    write(45, 50);
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
    if (serial && serial->is_open()) {
        BOOST_LOG_TRIVIAL(debug) << "Closing serial interface";
        try {
            serial->cancel();
            serial->close();
        } catch(...) {
            BOOST_LOG_TRIVIAL(error) << "Unable to close serial interface";
        }
    }
    io.reset(); // TODO: See if this leaks memory
}

void SerialInterface::write(int channel, int value) {
    BOOST_LOG_TRIVIAL(trace) << "Writing to serial port: [" << setfill('0') << setw(3) << channel << "] " << setw(3) << value;

    std::ostringstream ss;
    ss << channel << 'c' << value << 'w';

    try {
        if (!serial || !serial->is_open()) {
            BOOST_LOG_TRIVIAL(error) << "Serial port is not open";
            return;
        }

        boost::asio::write(*serial, boost::asio::buffer(ss.str().c_str(),ss.str().length()));
    } catch (boost::system::system_error &e) {
        BOOST_LOG_TRIVIAL(error) << "Error writing to serial port: " << e.what();
    }
}
