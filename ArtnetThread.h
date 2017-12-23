#ifndef DEMO_ARTNETTHREAD_H
#define DEMO_ARTNETTHREAD_H


#include <iostream>
#include <boost/asio/io_service.hpp> // TODO: Fixed location (boost/asio/impl/io_service.hpp)
#include <boost/array.hpp>
#include <boost/asio/ip/udp.hpp>
#include "ArtnetWindow.h"
#include "DMXBucket.h"
#include "SerialThread.h"


class ArtnetThread {
    std::shared_ptr<ArtnetWindow> artnetWindow;
    std::shared_ptr<DMXBucket> dmxBucket;
    std::shared_ptr<SerialThread::Updater> serialUpdater;

    std::shared_ptr<boost::asio::io_service> io;
    std::shared_ptr<boost::asio::ip::udp::socket> socket;
    boost::asio::ip::udp::endpoint remoteEndpoint;
    std::array<char, 600> buffer;

    boost::optional<boost::asio::ip::address> requestedAddress;
    std::shared_ptr<boost::mutex> reqAddress_mtx_;

    static constexpr const char *IDENTIFIER = "Art-Net";

    inline void OpDmx(std::size_t received_size);
    inline void OpPoll();

public:
    void operator()();

    ArtnetThread(const std::shared_ptr<ArtnetWindow> window, const std::shared_ptr<DMXBucket> dmxBucket, const std::shared_ptr<SerialThread::Updater> serialUpdater);

    void startReceive();
    void handleReceive(const boost::system::error_code &error,
                       std::size_t size/*bytes_transferred*/);

    void restartSocket();

    void endpointSelectCallback(bool anySelected, const std::string &address);

};


#endif //DEMO_ARTNETTHREAD_H
