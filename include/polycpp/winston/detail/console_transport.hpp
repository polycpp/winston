#pragma once

/// @file detail/console_transport.hpp
/// @brief Inline implementations for ConsoleTransport.

#include <polycpp/winston/console_transport.hpp>
#include <polycpp/os.hpp>

namespace polycpp {
namespace winston {

inline ConsoleTransport::ConsoleTransport(const ConsoleTransportOptions& opts)
    : ConsoleTransport(opts, std::cout, std::cerr) {}

inline ConsoleTransport::ConsoleTransport(
    const ConsoleTransportOptions& opts,
    std::ostream& out, std::ostream& err)
    : Transport(TransportOptions{
          opts.format,
          opts.level,
          opts.silent,
          opts.handleExceptions,
          opts.handleRejections})
    , stdout_(out)
    , stderr_(err)
    , stderrLevels_(opts.stderrLevels.begin(), opts.stderrLevels.end())
    , consoleWarnLevels_(opts.consoleWarnLevels.begin(),
                         opts.consoleWarnLevels.end())
    , eol_(opts.eol.empty() ? polycpp::os::EOL : opts.eol) {}

inline void ConsoleTransport::log(const LogInfo& info) {
    const std::string& output = info.formattedMessage.empty()
        ? info.message : info.formattedMessage;

    const std::string& levelKey = info.originalLevel.empty()
        ? info.level : info.originalLevel;

    std::ostream& target =
        (stderrLevels_.count(levelKey) > 0 ||
         consoleWarnLevels_.count(levelKey) > 0)
        ? stderr_ : stdout_;

    target << output << eol_;
}

} // namespace winston
} // namespace polycpp
