#include "SerialInterface.h"

#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <memory>
#include <boost/log/trivial.hpp>
#include <iomanip>
#include <utility>

#include "DMXBucket.h"
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>

#ifdef WIN32
#include "atlbase.h"
#include <Setupapi.h>
#include "lib/enumser/enumser.h"
#else
#include "boost/filesystem.hpp"
using namespace boost::filesystem;
#endif

using std::vector;
using std::string;
using std::shared_ptr;
using namespace boost::asio;
using boost::lock_guard;
using boost::mutex;

vector<std::pair<string, string> > SerialInterface::listInterfaces() {
	// A list of a <value, name> pairs
    vector<std::pair<string,string >> ifaces;

#ifdef WIN32
	CEnumerateSerial::CPortsArray ports;
	CEnumerateSerial::CNamesArray names;

	if (CEnumerateSerial::UsingSetupAPI2(ports, names)) {
		ifaces.reserve(ports.size());
		for (int i = 0; i < ports.size(); i++) {
			ifaces.push_back(std::make_pair<string,string>(
				string("COM") + std::to_string(ports[i]),
				string("COM") + std::to_string(ports[i]) + " (" + names[i].c_str() + ")"
			));
		}
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "Could not fetch list of serial interfaces: " << GetLastError();
	}
#else
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

				ifaces.push_back(std::make_pair<string,string>(
					deviceName.c_str(),
					itr->path().leaf().string().c_str() // TODO: Try to get rid of the c_str()s
				));
			}
		}
	}
	catch (boost::filesystem::filesystem_error &e) {
		BOOST_LOG_TRIVIAL(error) << "Could not fetch list of serial interfaces: " << e.what();
	}
#endif

    return ifaces;
}

SerialInterface::SerialInterface() : stats(boost::chrono::milliseconds(500)) {
    connected = false;
}

void SerialInterface::connect(std::string port, unsigned int baud_rate) {
    this->baudRate = baud_rate;

    lock_guard<mutex> guard(mtx_);
    try {
        if (serial && serial->is_open()) {
            disconnect();
        }
        BOOST_LOG_TRIVIAL(debug) << "Opening serial interface " << port << " [" << baud_rate << ']';

        io = std::make_shared<io_context>();
        serial = std::make_shared<basic_serial_port<io_context::executor_type> >(*io, port);
        serial->set_option(serial_port_base::baud_rate(baud_rate));

		// Default Arduino UART options
		serial->set_option(serial_port_base::character_size(8));
		serial->set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
		serial->set_option(serial_port_base::parity(serial_port_base::parity::none));
		serial->set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));

		connected = true;
    } catch (boost::system::system_error &e) {
        BOOST_LOG_TRIVIAL(error) << "Unable to open interface " << port << ": " << e.what();
	} catch (...) {
		BOOST_LOG_TRIVIAL(error) << "Unable to open interface " << port << ": " << "Unknown error";
	}
}

void SerialInterface::disconnect() {
	connected = false;
    // No lock acquired, we assume the lock is already in place by the caller function

	try {
		if (serial && serial->is_open()) {
			BOOST_LOG_TRIVIAL(debug) << "Closing serial interface";

			serial->cancel();
			serial->close();
			serial.reset();
		}
		if (io) {
			io->stop();
			io->reset();
		}
	} catch (...) {
		BOOST_LOG_TRIVIAL(error) << "Unable to close serial interface";
	}
}

void SerialInterface::test() {
    // TODO: Improve
    lock_guard<mutex> guard(mtx_);

    std::ostringstream ss;
    ss << opSignal << 'f' << '\0' << '\0'<< '\0'<< '\0'<< '\0'<< '\0'<< '\0'<< '\0'<< 'a' << 'a' << 'a' << 'a' << 'a';
	try {
		writeWithTimeout(*serial, boost::asio::buffer(ss.str().c_str(), ss.str().length()), boost::posix_time::milliseconds(100));
	} catch (boost::system::system_error &e) {
		BOOST_LOG_TRIVIAL(error) << "Unable to write test to serial interface: " << e.what();

		// Update the `connected` variable while we're at it
		if (!serial->is_open()) {
			connected = false;
		}
	}
}

void SerialInterface::resetError() {
    // TODO: Improve
    lock_guard<mutex> guard(mtx_);

    std::ostringstream ss;
    ss << opSignal << 'e';
	try {
        writeWithTimeout(*serial, boost::asio::buffer(ss.str().c_str(), ss.str().length()), boost::posix_time::milliseconds(100));
	}
	catch (boost::system::system_error &e) {
		BOOST_LOG_TRIVIAL(error) << "Unable to clear error bit in serial interface: " << e.what();
	}
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
    ss << opSignal << 'f';

    int i = 0;

    for (auto &val : dmxValues) {
        if (i++ > 200) break;
        ss << val;
        if (val == opSignal) {
            // We're sending an operation code -- send it again so that the receiver understands it's just part of
            // the data
            ss << val;
        }
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

template <typename SyncReadStream, typename ConstBufferSequence>
static void SerialInterface::writeWithTimeout(SyncReadStream& s, const ConstBufferSequence& buffers, const boost::asio::deadline_timer::duration_type& expiry_time) {
    boost::asio::io_context& io = s.get_executor().context();
    boost::optional<boost::system::error_code> timer_result;
    boost::asio::deadline_timer timer(io);
    timer.expires_from_now(expiry_time);
    timer.async_wait([&timer_result](const boost::system::error_code& error) { timer_result.reset(error); });

    boost::optional<boost::system::error_code> read_result;
    boost::asio::async_write(s, buffers, [&read_result](const boost::system::error_code& error, size_t) { read_result.reset(error); });

    io.reset();
    while (io.run_one()) {
        if (read_result) {
            timer.cancel();
        } else if (timer_result) {
            s.cancel();
            BOOST_LOG_TRIVIAL(warning) << "Serial port timeout [" << (expiry_time.total_microseconds()) << " us]";
        }
    }

    if (*read_result) {
        throw boost::system::system_error(*read_result);
    }
}
