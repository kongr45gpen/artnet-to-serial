#include "LoggingUtilities.h"
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#include <boost/date_time/posix_time/time_formatters_limited.hpp>

namespace LoggingUtilities {
    void coloring_terminal_formatter(
            boost::log::record_view const &rec, boost::log::basic_formatting_ostream<char> &strm) {
        auto severity = rec[boost::log::trivial::severity];
        if (severity) {
            // Set the color
            switch (severity.get()) {
                case boost::log::trivial::info:
                    strm << "\033[32m";
                    break;
                case boost::log::trivial::warning:
                    strm << "\033[33m";
                    break;
                case boost::log::trivial::error:
                case boost::log::trivial::fatal:
                    strm << "\033[31m";
                    break;
                default:
                    break;
            }
        }

        // Format the message here...
        strm << "["
             << boost::posix_time::to_simple_string(*(boost::log::extract<boost::posix_time::ptime>("TimeStamp", rec)))
             << "] ["
             << boost::log::extract<boost::log::attributes::current_thread_id::value_type>("ThreadID", rec) << "]"
             << " [" << severity << "] "
             << rec[boost::log::expressions::smessage];

        if (severity) {
            // Restore the default color
            strm << "\033[0m";
        }
    }

    void GUISinkBackend::consume(boost::log::record_view const &rec) {
        // TODO: threads(?)
        array<float, 3> colour;

        auto severity = rec[boost::log::trivial::severity];
        if (severity) {
            // Set the color
            switch (severity.get()) {
                case boost::log::trivial::info:
                    colour = {0.1f, 0.8f, 0.9f};
                    break;
                case boost::log::trivial::warning:
                    colour = {1.0f, 0.78f, 0.58f};
                    break;
                case boost::log::trivial::error:
                case boost::log::trivial::fatal:
                    colour = {1.0f, 0.2f, 0.1f};
                    break;
                case boost::log::trivial::trace:
                    colour = {0.8f, 0.8f, 0.8f};
                    break;
                default:
                    colour = {1.0f, 1.0f, 1.0f};
                    break;
            }
        }

        std::ostringstream ss;

        // Format the message here...
        ss << "["
           << boost::posix_time::to_simple_string(*(boost::log::extract<boost::posix_time::ptime>("TimeStamp", rec)))
           << "] ["
           << boost::log::extract<boost::log::attributes::current_thread_id::value_type>("ThreadID", rec) << "]"
           << " [" << severity << "] "
           << rec[boost::log::expressions::smessage];
        logWindow->addEntry(ss.str(), colour);
    }

    void GUISinkBackend::flush() {

    }
}
