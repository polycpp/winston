#pragma once

/// @file stream_transport.hpp
/// @brief StreamTransport declaration -- writes log entries to any std::ostream.

#include <polycpp/winston/transport.hpp>
#include <ostream>
#include <string>

namespace polycpp {
namespace winston {

/// @brief Options for StreamTransport construction.
///
/// Extends the base TransportOptions fields with a target ostream and
/// end-of-line control.
///
/// @see https://github.com/winstonjs/winston/blob/master/lib/winston/transports/stream.js
struct StreamTransportOptions {
    /// Per-transport level threshold. Empty string means inherit from logger.
    std::string level;

    /// Per-transport format (applied after logger format, before log()).
    std::shared_ptr<Format> format;

    /// Suppress this transport entirely.
    bool silent = false;

    /// Whether this transport handles uncaught exceptions.
    bool handleExceptions = false;

    /// Whether this transport handles unhandled rejections.
    bool handleRejections = false;

    /// @brief Reference to the target ostream. Required.
    ///
    /// The transport does NOT own this stream -- the caller must keep it
    /// alive for the lifetime of the transport.
    std::ostream* stream = nullptr;

    /// @brief End-of-line string appended to each log line.
    ///
    /// Default: polycpp::os::EOL (platform-appropriate line ending).
    std::string eol;
};

/// @brief Transport that writes log entries to any std::ostream.
///
/// A thin wrapper around any output stream. This is the C++ equivalent of
/// the JS winston Stream transport, which wraps any WritableStream.
///
/// @par Example
/// ```cpp
/// std::ofstream file("app.log");
/// StreamTransportOptions opts;
/// opts.stream = &file;
/// auto transport = std::make_shared<StreamTransport>(opts);
/// ```
///
/// @see https://github.com/winstonjs/winston/blob/master/docs/transports.md#stream-transport
class StreamTransport : public Transport {
public:
    /// @brief Construct with options.
    ///
    /// @param options Stream transport configuration options.
    /// @throws polycpp::Error if options.stream is nullptr.
    explicit StreamTransport(StreamTransportOptions options);

    /// @brief Write a log entry to the output stream.
    ///
    /// Uses info.formattedMessage if non-empty, otherwise info.message.
    /// Appends the configured eol string.
    ///
    /// @param info The fully formatted log entry.
    void log(const LogInfo& info) override;

private:
    std::ostream& stream_;
    std::string eol_;
};

} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/stream_transport.hpp>
