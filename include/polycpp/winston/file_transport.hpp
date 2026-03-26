#pragma once

/// @file file_transport.hpp
/// @brief FileTransport declaration — writes log entries to a file with optional rotation.

#include <polycpp/winston/transport.hpp>
#include <mutex>
#include <optional>
#include <string>

namespace polycpp {
namespace winston {

/// @brief Options for FileTransport construction.
///
/// Extends the base TransportOptions fields with file-specific settings
/// for rotation, compression, and end-of-line control.
///
/// @see https://github.com/winstonjs/winston/blob/master/docs/transports.md#file-transport
struct FileTransportOptions {
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

    /// @brief Log file name. Required.
    ///
    /// Can be a full path or just a filename (combined with dirname).
    std::string filename;

    /// @brief Directory for the log file.
    ///
    /// Default: derived from filename via path::dirname, or "." if filename
    /// has no directory component.
    std::string dirname;

    /// @brief Maximum file size in bytes before rotation.
    ///
    /// Default: nullopt — no rotation.
    std::optional<size_t> maxsize;

    /// @brief Maximum number of rotated files to keep.
    ///
    /// Default: nullopt — no limit on rotated files.
    std::optional<int> maxFiles;

    /// @brief Tailable mode: always write to the base filename.
    ///
    /// When true, rotated files get incrementing numbers (file.1, file.2, ...)
    /// with .1 being the most recently rotated. The base filename always
    /// contains the latest logs.
    /// When false: standard rotation where base file is renamed with a numeric
    /// suffix on rotation.
    /// Default: false.
    bool tailable = false;

    /// @brief Compress rotated files with gzip.
    ///
    /// When true, after rotation the rotated file is compressed with gzip
    /// and the original uncompressed file is removed.
    /// Default: false.
    bool zippedArchive = false;

    /// @brief End-of-line character appended to each log line.
    ///
    /// Default: polycpp::os::EOL (platform-appropriate line ending).
    std::string eol;

    /// @brief Lazy mode: defer file creation until the first write.
    ///
    /// When true, the file is not opened in the constructor. It is opened
    /// on the first call to log().
    /// Default: false — file is opened immediately in constructor.
    bool lazy = false;
};

/// @brief Transport that writes log entries to a file with optional size-based rotation.
///
/// Supports rotation when the file exceeds a configurable maximum size, with
/// optional limits on the number of rotated files kept. Tailable mode ensures
/// the base filename always contains the most recent log entries. Rotated files
/// can optionally be gzip-compressed.
///
/// @par Example
/// ```cpp
/// FileTransportOptions opts;
/// opts.filename = "app.log";
/// opts.maxsize = 1024 * 1024;  // 1 MB
/// opts.maxFiles = 5;
/// auto transport = std::make_shared<FileTransport>(std::move(opts));
/// ```
///
/// @see https://github.com/winstonjs/winston/blob/master/docs/transports.md#file-transport
class FileTransport : public Transport {
public:
    /// @brief Construct with options. Opens the file immediately unless lazy mode.
    ///
    /// Computes the full path from dirname and filename, creates the directory
    /// if it does not exist, and opens the file in append mode (unless lazy).
    ///
    /// @param options File transport configuration options.
    explicit FileTransport(FileTransportOptions options);

    /// @brief Destructor closes the file descriptor.
    ~FileTransport();

    // Non-copyable, non-movable (owns a file descriptor and mutex)
    FileTransport(const FileTransport&) = delete;
    FileTransport& operator=(const FileTransport&) = delete;

    /// @brief Write a log entry to the file.
    ///
    /// Gets the output string from info.formattedMessage (or info.message
    /// as fallback), appends the configured EOL, writes to the file, and
    /// triggers rotation if the file exceeds maxsize.
    ///
    /// @param info The fully formatted log entry.
    void log(const LogInfo& info) override;

    /// @brief Close the file explicitly.
    ///
    /// Closes the file descriptor if it is open. The transport can be
    /// reopened by logging another message (lazy behavior).
    void close();

    /// @brief Get the full path of the current log file.
    /// @return The resolved full path (dirname + filename).
    std::string filename() const;

private:
    FileTransportOptions options_;
    std::string fullpath_;
    int fd_ = -1;
    size_t size_ = 0;
    int created_ = 0;
    std::mutex mutex_;

    /// @brief Open the log file (append mode). Sets fd_ and reads current size.
    void openFile_();

    /// @brief Close the current file descriptor.
    void closeFile_();

    /// @brief Rotate the current file and open a new one.
    void rotate_();

    /// @brief Standard rotation: rename current file with numeric suffix.
    void rotateStandard_();

    /// @brief Tailable rotation: shift existing numbered files up, rename current.
    void rotateTailable_();

    /// @brief Compress a file with gzip.
    /// @param sourcePath Path to the file to compress.
    void compressFile_(const std::string& sourcePath);

    /// @brief Remove excess rotated files beyond maxFiles.
    void cleanupOldFiles_();

    /// @brief Build the rotated filename for the given rotation index.
    /// @param index The rotation index (1-based).
    /// @return The full path of the rotated file.
    std::string rotatedFilename_(int index) const;
};

} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/file_transport.hpp>
