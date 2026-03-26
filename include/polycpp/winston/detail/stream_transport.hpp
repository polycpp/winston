#pragma once

/// @file detail/stream_transport.hpp
/// @brief Inline implementations for StreamTransport.

#include <polycpp/winston/stream_transport.hpp>
#include <polycpp/os.hpp>
#include <polycpp/core/error.hpp>

namespace polycpp {
namespace winston {

inline StreamTransport::StreamTransport(StreamTransportOptions options)
    : Transport(TransportOptions{
          std::move(options.format),
          std::move(options.level),
          options.silent,
          options.handleExceptions,
          options.handleRejections})
    , stream_([&options]() -> std::ostream& {
          if (!options.stream) {
              throw polycpp::Error("StreamTransport requires a non-null stream");
          }
          return *options.stream;
      }())
    , eol_(options.eol.empty() ? polycpp::os::EOL : std::move(options.eol)) {}

inline void StreamTransport::log(const LogInfo& info) {
    const std::string& output = info.formattedMessage.empty()
        ? info.message : info.formattedMessage;
    stream_ << output << eol_;
}

} // namespace winston
} // namespace polycpp
