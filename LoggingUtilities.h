#ifndef DEMO_LOGGINGUTILITIES_H
#define DEMO_LOGGINGUTILITIES_H

#include <boost/log/core/record_view.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/sinks/frontend_requirements.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>
//#include <memory>
#include "gui/LogWindow.h"

namespace LoggingUtilities {
    void
    coloring_terminal_formatter(boost::log::record_view const &rec, boost::log::basic_formatting_ostream<char> &strm);

    class GUISinkBackend :
            public boost::log::sinks::basic_sink_backend<boost::log::sinks::synchronized_feeding> {
        std::shared_ptr<LogWindow> logWindow;
    public:
        GUISinkBackend(const std::shared_ptr<LogWindow> window) : logWindow(window) {};

        // The function consumes the log records that come from the frontend
        void consume(boost::log::record_view const &rec);

        // The function flushes the file
        void flush();
    };
}

#endif //DEMO_LOGGINGUTILITIES_H
