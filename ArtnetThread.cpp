#include <boost/chrono/duration.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include "ArtnetThread.h"

using boost::asio::ip::udp;
using boost::asio::io_service;

void ArtnetThread::operator()() {
    boost::this_thread::sleep_for(boost::chrono::seconds(1)); // sleep, because why not
    artnetWindow->setDeviceCallback(
            std::bind(&ArtnetThread::endpointSelectCallback, this, std::placeholders::_1, std::placeholders::_2));
    artnetWindow->setEnabledCallback(
            std::bind(&ArtnetThread::outputEnabledCallback, this, std::placeholders::_1));
    // set the callback here, otherwise the copied data might be wrong

    restartSocket();

    io->run();
}

ArtnetThread::ArtnetThread(const std::shared_ptr<ArtnetWindow> window, const std::shared_ptr<DMXBucket> dmxBucket, const std::shared_ptr<SerialThread::Updater> serialUpdater) :
        artnetWindow(window), dmxBucket(dmxBucket), serialUpdater(serialUpdater) {
    io = std::make_shared<io_service>();
    reqAddress_mtx_ = std::make_shared<boost::mutex>();
    outputEnabled = std::make_shared<std::atomic_bool>();
    outputEnabled->store(true);
}

void ArtnetThread::restartSocket() {
    try {
        // Create and open the socket
        socket = std::make_shared<udp::socket>(*io, udp::v4());

        // Allow other nodes to connect
        boost::asio::socket_base::reuse_address option(true);
        socket->set_option(option);

        // Bind to the requested port
        socket->bind(udp::endpoint(udp::v4(), 6454));
//        socket->open(udp::endpoint(boost::asio::ip::address::from_string("2.3.4.5"), 6454));

        startReceive();
        BOOST_LOG_TRIVIAL(info) << "Art-Net socket initialised.";
    } catch (boost::system::system_error &e) {
        BOOST_LOG_TRIVIAL(error) << "Could not open Art-Net socket: " << e.what();
    }
}

void ArtnetThread::startReceive() {
    try {
        if (!socket) {
            BOOST_LOG_TRIVIAL(error) << "Could not receive Art-Net packages - The socket is not open.";
            return;
        }

        socket->async_receive_from(
                boost::asio::buffer(buffer), remoteEndpoint,
                boost::bind(&ArtnetThread::handleReceive, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
    } catch (boost::system::system_error &e) {
        BOOST_LOG_TRIVIAL(error) << "Could not receive Art-Net package: " << e.what();
    }

}

void ArtnetThread::handleReceive(const boost::system::error_code &error,
                                 std::size_t size/*bytes_transferred*/) {
    if (!error) {
        // Make sure this is an Art-Net package and not something else
        if (size >= 10 && memcmp(buffer.data(), IDENTIFIER, 8) == 0 &&
            (!requestedAddress.is_initialized() || requestedAddress.get() == remoteEndpoint.address())) {
            // bytes 9 and 8 (lowest significance first) represent the packet type
            if (buffer[9] == 0x20 && buffer[8] == 0x00) {
                // OpPoll
                BOOST_LOG_TRIVIAL(trace) << "Received OpPoll Art-Net packet (" << size << " bytes)"
                                         << " from " << remoteEndpoint.address();
                OpPoll();
            } else if (buffer[9] == 0x21 && buffer[8] == 0x00) {
                // OpPollReply
                BOOST_LOG_TRIVIAL(trace) << "Received OpPollReply Art-Net packet (" << size << " bytes)"
                                         << " from " << remoteEndpoint.address();
                OpPollReply(size);
            } else if (buffer[9] == 0x50 && buffer[8] == 0x00) {
                // OpDmx
                BOOST_LOG_TRIVIAL(trace) << "Received OpDmx  Art-Net packet (" << size << " bytes)";
                OpDmx(size);
            } else {
                BOOST_LOG_TRIVIAL(warning) << "Received unknown Art-Net packet 0x"
                                           << std::hex << std::setfill('0') << std::setw(2) << (int) buffer[9]
                                           << std::hex << std::setfill('0') << std::setw(2) << (int) buffer[8]
                                           << std::dec
                                           << " (" << (int) size << " bytes)";
            }
            artnetWindow->notifyPacketReceived();
        }
    } else {
        BOOST_LOG_TRIVIAL(error) << "Error in receiving Art-Net package: " << error.message();
    }

    startReceive();
}

inline void ArtnetThread::OpPollReply(std::size_t size) {
    auto shortName = static_cast<char*>(&(*(buffer.begin() + 26)));
    auto longName = static_cast<char*>(&(*(buffer.begin() + 44)));

    // Make sure we don't go to bad memory areas if an invalid package is received
    shortName[17] = longName[63] = '\0';

    BOOST_LOG_TRIVIAL(trace) << "OpPollReply descriptions: [" << shortName << "] " << longName;

    artnetWindow->pushControllerDescription(remoteEndpoint.address().to_string(), longName);
}

inline void ArtnetThread::OpDmx(std::size_t size) {
    auto protVerHi = static_cast<uint8_t>(buffer[10]); // Art-Net protocol version
    auto protVerLo = static_cast<uint8_t>(buffer[11]); // Art-Net protocol version
    auto sequence = static_cast<uint8_t>(buffer[12]); // Sequence number
    auto physical = static_cast<uint8_t>(buffer[13]); // Physical DMX512 input port
    auto subUniv = static_cast<uint8_t>(buffer[14]); // subuniverse (universe)
    auto net = static_cast<uint8_t>(buffer[15]); // network     (== 16 subnets == 256 universes)

    BOOST_LOG_TRIVIAL(trace) << "Received package #" << (int) sequence << " for universe " << (int) net << "."
                             << (int) subUniv;

    if (subUniv != 0 || net != 0) {
        // Discard packet. We only provide one universe.
        return;
    }

    // The length of the received data array
    auto length = (static_cast<uint8_t>(buffer[16]) << 8) + static_cast<uint8_t>(buffer[17]);

    if (size != length + 18) {
        BOOST_LOG_TRIVIAL(error) << "Length of [" << length << "] in data does not match received length [" << size - 18
                                 << "]";
        return;
    }

    if (outputEnabled->load()) {
        dmxBucket->setData(buffer.begin() + 18, buffer.begin() + 18 + ((int) length));
        serialUpdater->announceDataReady();
    }

    artnetWindow->statistics.addOne();
}

void ArtnetThread::OpPoll() {
    artnetWindow->pushController(remoteEndpoint.address().to_string());
}

void ArtnetThread::endpointSelectCallback(bool anySelected, const std::string &address) {
    boost::lock_guard<boost::mutex> guard(*reqAddress_mtx_);
    if (anySelected) {
        requestedAddress.reset();
    } else {
        requestedAddress.reset(boost::asio::ip::address::from_string(address));
    }
}

void ArtnetThread::outputEnabledCallback(bool outputEnabled) {
    // No need for a mutex, since outputEnabled is atomic
    ArtnetThread::outputEnabled->store(outputEnabled);
}
