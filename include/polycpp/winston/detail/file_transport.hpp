#pragma once

/// @file detail/file_transport.hpp
/// @brief Inline implementations for FileTransport.

#include <polycpp/winston/file_transport.hpp>
#include <polycpp/fs.hpp>
#include <polycpp/path.hpp>
#include <polycpp/os.hpp>
#include <polycpp/zlib.hpp>
#include <polycpp/buffer.hpp>
#include <string>

namespace polycpp {
namespace winston {

inline FileTransport::FileTransport(FileTransportOptions options)
    : Transport(TransportOptions{
          options.format,
          options.level,
          options.silent,
          options.handleExceptions,
          options.handleRejections})
    , options_(std::move(options)) {
    // Resolve fullpath from dirname and filename
    if (!options_.dirname.empty()) {
        fullpath_ = polycpp::path::join(options_.dirname, options_.filename);
    } else if (options_.filename.find('/') != std::string::npos) {
        // filename contains a directory component
        fullpath_ = options_.filename;
        options_.dirname = polycpp::path::dirname(fullpath_);
        options_.filename = polycpp::path::basename(fullpath_);
    } else {
        options_.dirname = ".";
        fullpath_ = polycpp::path::join(".", options_.filename);
    }

    // Set eol default
    if (options_.eol.empty()) {
        options_.eol = polycpp::os::EOL;
    }

    // Open immediately unless lazy mode
    if (!options_.lazy) {
        openFile_();
    }
}

inline FileTransport::~FileTransport() {
    std::lock_guard<std::mutex> lock(mutex_);
    closeFile_();
}

inline void FileTransport::log(const LogInfo& info) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Lazy mode: open on first write
    if (fd_ < 0) {
        openFile_();
    }

    // Get output string
    const std::string& output = info.formattedMessage.empty()
        ? info.message : info.formattedMessage;

    // Construct the line with EOL
    std::string line = output + options_.eol;

    // Write to file
    polycpp::fs::writeSync(fd_, line);

    // Track size
    size_ += line.size();

    // Check rotation
    if (options_.maxsize.has_value() && size_ >= *options_.maxsize) {
        rotate_();
    }
}

inline void FileTransport::close() {
    std::lock_guard<std::mutex> lock(mutex_);
    closeFile_();
}

inline std::string FileTransport::filename() const {
    return fullpath_;
}

inline void FileTransport::openFile_() {
    // Ensure directory exists
    if (!polycpp::fs::existsSync(options_.dirname)) {
        polycpp::fs::mkdirSync(options_.dirname, true);
    }

    // Open file in append mode
    fd_ = polycpp::fs::openSync(fullpath_, "a");

    // Get current file size
    auto stats = polycpp::fs::statSync(fullpath_);
    size_ = stats.size;
}

inline void FileTransport::closeFile_() {
    if (fd_ >= 0) {
        polycpp::fs::closeSync(fd_);
        fd_ = -1;
    }
}

inline void FileTransport::rotate_() {
    closeFile_();
    created_++;

    if (options_.tailable) {
        rotateTailable_();
    } else {
        rotateStandard_();
    }

    if (options_.maxFiles.has_value()) {
        cleanupOldFiles_();
    }

    openFile_();
}

inline void FileTransport::rotateStandard_() {
    // Rename current base file to a numbered variant
    std::string rotatedPath = rotatedFilename_(created_);
    polycpp::fs::renameSync(fullpath_, rotatedPath);

    // Optionally compress the rotated file
    if (options_.zippedArchive) {
        compressFile_(rotatedPath);
    }
}

inline void FileTransport::rotateTailable_() {
    // Shift existing rotated files up by 1, capped by maxFiles to avoid
    // iterating over indices that were already cleaned up
    int maxIdx = options_.maxFiles.has_value()
        ? std::min(created_, options_.maxFiles.value())
        : created_;
    for (int i = maxIdx; i >= 2; --i) {
        std::string src = rotatedFilename_(i - 1);
        std::string dst = rotatedFilename_(i);
        if (options_.zippedArchive) {
            std::string srcGz = src + ".gz";
            if (polycpp::fs::existsSync(srcGz)) {
                polycpp::fs::renameSync(srcGz, dst + ".gz");
            }
        } else {
            if (polycpp::fs::existsSync(src)) {
                polycpp::fs::renameSync(src, dst);
            }
        }
    }

    // Rename current base file to .1
    polycpp::fs::renameSync(fullpath_, rotatedFilename_(1));

    // Optionally compress the rotated file
    if (options_.zippedArchive) {
        compressFile_(rotatedFilename_(1));
    }
}

inline void FileTransport::compressFile_(const std::string& sourcePath) {
    // Read the file content as a Buffer
    auto content = polycpp::fs::readFileSyncBuffer(sourcePath);

    // Compress with gzip
    auto compressed = polycpp::zlib::gzipSync(content);

    // Write compressed file
    polycpp::fs::writeFileSync(sourcePath + ".gz", compressed);

    // Remove original
    polycpp::fs::unlinkSync(sourcePath);
}

inline void FileTransport::cleanupOldFiles_() {
    // After each rotation only one file is ever in excess. Delete at most
    // a small range above maxFiles to avoid an unbounded O(created_) scan.
    int maxFiles = *options_.maxFiles;
    int start = maxFiles + 1;
    int end = std::min(created_, start + 100);
    for (int i = start; i <= end; ++i) {
        std::string path = rotatedFilename_(i);
        if (options_.zippedArchive) {
            std::string gzPath = path + ".gz";
            if (polycpp::fs::existsSync(gzPath)) {
                polycpp::fs::unlinkSync(gzPath);
            }
        }
        if (polycpp::fs::existsSync(path)) {
            polycpp::fs::unlinkSync(path);
        }
    }
}

inline std::string FileTransport::rotatedFilename_(int index) const {
    return polycpp::path::join(
        options_.dirname,
        options_.filename + "." + std::to_string(index));
}

} // namespace winston
} // namespace polycpp
