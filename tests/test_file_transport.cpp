#include <gtest/gtest.h>
#include <polycpp/winston/file_transport.hpp>
#include <polycpp/winston/logger.hpp>
#include <polycpp/winston/level_config.hpp>
#include <polycpp/fs.hpp>
#include <polycpp/os.hpp>
#include <polycpp/zlib.hpp>
#include <polycpp/buffer.hpp>
#include <string>
#include <unistd.h>

using namespace polycpp::winston;

// ---------------------------------------------------------------------------
// Test fixture with temp directory
// ---------------------------------------------------------------------------

class FileTransportTest : public ::testing::Test {
protected:
    std::string tmpDir_;

    void SetUp() override {
        tmpDir_ = "/tmp/winston_test_" + std::to_string(getpid()) + "_" +
                  std::to_string(::testing::UnitTest::GetInstance()
                      ->current_test_info()->line());
        polycpp::fs::mkdirSync(tmpDir_, true);
    }

    void TearDown() override {
        polycpp::fs::rmSync(tmpDir_, true, true);
    }

    std::string readFile(const std::string& path) {
        return polycpp::fs::readFileSync(path);
    }

    std::string logPath(const std::string& name) {
        return polycpp::path::join(tmpDir_, name);
    }
};

// ---------------------------------------------------------------------------
// Basic File Writing
// ---------------------------------------------------------------------------

TEST_F(FileTransportTest, BasicWrite) {
    FileTransportOptions opts;
    opts.filename = logPath("app.log");

    FileTransport transport(std::move(opts));

    LogInfo info("info", "hello world");
    transport.write(info);

    std::string content = readFile(logPath("app.log"));
    EXPECT_EQ(content, "hello world" + polycpp::os::EOL);
}

TEST_F(FileTransportTest, MultipleWrites) {
    FileTransportOptions opts;
    opts.filename = logPath("app.log");

    FileTransport transport(std::move(opts));

    transport.write(LogInfo("info", "line one"));
    transport.write(LogInfo("info", "line two"));
    transport.write(LogInfo("info", "line three"));

    std::string content = readFile(logPath("app.log"));
    std::string expected =
        "line one" + polycpp::os::EOL +
        "line two" + polycpp::os::EOL +
        "line three" + polycpp::os::EOL;
    EXPECT_EQ(content, expected);
}

TEST_F(FileTransportTest, AppendToExistingFile) {
    std::string path = logPath("existing.log");
    polycpp::fs::writeFileSync(path, "existing content\n");

    FileTransportOptions opts;
    opts.filename = path;

    FileTransport transport(std::move(opts));
    transport.write(LogInfo("info", "new line"));

    std::string content = readFile(path);
    std::string expected = "existing content\n" + std::string("new line") + polycpp::os::EOL;
    EXPECT_EQ(content, expected);
}

TEST_F(FileTransportTest, UsesFormattedMessage) {
    FileTransportOptions opts;
    opts.filename = logPath("app.log");

    FileTransport transport(std::move(opts));

    LogInfo info("info", "raw message");
    info.formattedMessage = "formatted output";
    transport.write(info);

    std::string content = readFile(logPath("app.log"));
    EXPECT_EQ(content, "formatted output" + polycpp::os::EOL);
}

TEST_F(FileTransportTest, FallbackToMessage) {
    FileTransportOptions opts;
    opts.filename = logPath("app.log");

    FileTransport transport(std::move(opts));

    LogInfo info("info", "raw message");
    // formattedMessage left empty
    transport.write(info);

    std::string content = readFile(logPath("app.log"));
    EXPECT_EQ(content, "raw message" + polycpp::os::EOL);
}

// ---------------------------------------------------------------------------
// EOL Handling
// ---------------------------------------------------------------------------

TEST_F(FileTransportTest, DefaultEol) {
    FileTransportOptions opts;
    opts.filename = logPath("app.log");

    FileTransport transport(std::move(opts));
    transport.write(LogInfo("info", "msg"));

    std::string content = readFile(logPath("app.log"));
    EXPECT_EQ(content, "msg" + polycpp::os::EOL);
}

TEST_F(FileTransportTest, CustomEol) {
    FileTransportOptions opts;
    opts.filename = logPath("app.log");
    opts.eol = "\r\n";

    FileTransport transport(std::move(opts));
    transport.write(LogInfo("info", "msg"));

    std::string content = readFile(logPath("app.log"));
    EXPECT_EQ(content, "msg\r\n");
}

// ---------------------------------------------------------------------------
// Directory Handling
// ---------------------------------------------------------------------------

TEST_F(FileTransportTest, DirnameOption) {
    FileTransportOptions opts;
    opts.dirname = tmpDir_;
    opts.filename = "bydir.log";

    FileTransport transport(std::move(opts));
    transport.write(LogInfo("info", "hello"));

    std::string content = readFile(logPath("bydir.log"));
    EXPECT_EQ(content, "hello" + polycpp::os::EOL);
    EXPECT_EQ(transport.filename(), logPath("bydir.log"));
}

TEST_F(FileTransportTest, FullPathFilename) {
    std::string fullPath = logPath("fullpath.log");

    FileTransportOptions opts;
    opts.filename = fullPath;

    FileTransport transport(std::move(opts));
    transport.write(LogInfo("info", "hello"));

    EXPECT_EQ(transport.filename(), fullPath);
    std::string content = readFile(fullPath);
    EXPECT_EQ(content, "hello" + polycpp::os::EOL);
}

TEST_F(FileTransportTest, CreatesDirectoryIfNeeded) {
    std::string subdir = polycpp::path::join(tmpDir_, "sub/deep");

    FileTransportOptions opts;
    opts.dirname = subdir;
    opts.filename = "deep.log";

    FileTransport transport(std::move(opts));
    transport.write(LogInfo("info", "deep"));

    std::string content = readFile(polycpp::path::join(subdir, "deep.log"));
    EXPECT_EQ(content, "deep" + polycpp::os::EOL);
}

// ---------------------------------------------------------------------------
// Rotation on maxsize
// ---------------------------------------------------------------------------

TEST_F(FileTransportTest, RotateOnMaxsize) {
    FileTransportOptions opts;
    opts.dirname = tmpDir_;
    opts.filename = "rotate.log";
    opts.maxsize = 50;

    FileTransport transport(std::move(opts));

    // Write enough data to exceed maxsize (each line ~15 bytes with EOL)
    // "message_01\n" = 11 bytes, need to exceed 50
    for (int i = 0; i < 6; ++i) {
        std::string msg = "message_" + std::string(i < 10 ? "0" : "") + std::to_string(i);
        transport.write(LogInfo("info", msg));
    }

    // After rotation, there should be a .1 file
    EXPECT_TRUE(polycpp::fs::existsSync(logPath("rotate.log")));
    EXPECT_TRUE(polycpp::fs::existsSync(logPath("rotate.log.1")));
}

TEST_F(FileTransportTest, RotateMultipleTimes) {
    FileTransportOptions opts;
    opts.dirname = tmpDir_;
    opts.filename = "multi.log";
    opts.maxsize = 30;

    FileTransport transport(std::move(opts));

    // Write many messages to trigger multiple rotations
    for (int i = 0; i < 20; ++i) {
        transport.write(LogInfo("info", "msg" + std::to_string(i)));
    }

    // Should have multiple rotated files
    EXPECT_TRUE(polycpp::fs::existsSync(logPath("multi.log")));
    EXPECT_TRUE(polycpp::fs::existsSync(logPath("multi.log.1")));
    EXPECT_TRUE(polycpp::fs::existsSync(logPath("multi.log.2")));
}

TEST_F(FileTransportTest, NoRotationWithoutMaxsize) {
    FileTransportOptions opts;
    opts.dirname = tmpDir_;
    opts.filename = "norotate.log";
    // No maxsize set

    FileTransport transport(std::move(opts));

    for (int i = 0; i < 20; ++i) {
        transport.write(LogInfo("info", "message_" + std::to_string(i)));
    }

    EXPECT_TRUE(polycpp::fs::existsSync(logPath("norotate.log")));
    EXPECT_FALSE(polycpp::fs::existsSync(logPath("norotate.log.1")));
}

// ---------------------------------------------------------------------------
// maxFiles Cleanup
// ---------------------------------------------------------------------------

TEST_F(FileTransportTest, MaxFilesCleanup) {
    FileTransportOptions opts;
    opts.dirname = tmpDir_;
    opts.filename = "cleanup.log";
    opts.maxsize = 20;
    opts.maxFiles = 2;

    FileTransport transport(std::move(opts));

    // Write enough to trigger many rotations
    for (int i = 0; i < 30; ++i) {
        transport.write(LogInfo("info", "msg" + std::to_string(i)));
    }

    // Should have at most maxFiles rotated files
    EXPECT_TRUE(polycpp::fs::existsSync(logPath("cleanup.log")));

    // Count existing rotated files
    int rotatedCount = 0;
    for (int i = 1; i <= 20; ++i) {
        if (polycpp::fs::existsSync(logPath("cleanup.log." + std::to_string(i)))) {
            rotatedCount++;
        }
    }
    EXPECT_LE(rotatedCount, 2);
}

TEST_F(FileTransportTest, MaxFilesNoLimit) {
    FileTransportOptions opts;
    opts.dirname = tmpDir_;
    opts.filename = "nolimit.log";
    opts.maxsize = 20;
    // No maxFiles set

    FileTransport transport(std::move(opts));

    for (int i = 0; i < 20; ++i) {
        transport.write(LogInfo("info", "msg" + std::to_string(i)));
    }

    // All rotated files should remain
    int rotatedCount = 0;
    for (int i = 1; i <= 20; ++i) {
        if (polycpp::fs::existsSync(logPath("nolimit.log." + std::to_string(i)))) {
            rotatedCount++;
        }
    }
    EXPECT_GT(rotatedCount, 2);
}

// ---------------------------------------------------------------------------
// Tailable Mode
// ---------------------------------------------------------------------------

TEST_F(FileTransportTest, TailableRotation) {
    FileTransportOptions opts;
    opts.dirname = tmpDir_;
    opts.filename = "tailable.log";
    opts.maxsize = 50;
    opts.tailable = true;

    FileTransport transport(std::move(opts));

    // Write enough to trigger at least one rotation
    for (int i = 0; i < 10; ++i) {
        transport.write(LogInfo("info", "line_" + std::to_string(i)));
    }

    // Base file should exist and have the latest logs
    EXPECT_TRUE(polycpp::fs::existsSync(logPath("tailable.log")));
    // Rotated file .1 should exist with older logs
    EXPECT_TRUE(polycpp::fs::existsSync(logPath("tailable.log.1")));

    // The base file should contain some recent messages
    std::string baseContent = readFile(logPath("tailable.log"));
    EXPECT_FALSE(baseContent.empty());
}

TEST_F(FileTransportTest, TailableShiftsFiles) {
    FileTransportOptions opts;
    opts.dirname = tmpDir_;
    opts.filename = "tailshift.log";
    opts.maxsize = 30;
    opts.tailable = true;

    FileTransport transport(std::move(opts));

    // Write enough to trigger multiple rotations
    for (int i = 0; i < 20; ++i) {
        transport.write(LogInfo("info", "msg" + std::to_string(i)));
    }

    // Should have multiple rotated files
    EXPECT_TRUE(polycpp::fs::existsSync(logPath("tailshift.log")));
    EXPECT_TRUE(polycpp::fs::existsSync(logPath("tailshift.log.1")));
    EXPECT_TRUE(polycpp::fs::existsSync(logPath("tailshift.log.2")));

    // .1 should be more recent than .2
    // (We verify by checking that .1 file content differs from .2)
    std::string content1 = readFile(logPath("tailshift.log.1"));
    std::string content2 = readFile(logPath("tailshift.log.2"));
    EXPECT_NE(content1, content2);
}

// ---------------------------------------------------------------------------
// Lazy Mode
// ---------------------------------------------------------------------------

TEST_F(FileTransportTest, LazyDoesNotCreateFileImmediately) {
    FileTransportOptions opts;
    opts.dirname = tmpDir_;
    opts.filename = "lazy.log";
    opts.lazy = true;

    FileTransport transport(std::move(opts));

    // File should NOT exist yet
    EXPECT_FALSE(polycpp::fs::existsSync(logPath("lazy.log")));
}

TEST_F(FileTransportTest, LazyCreatesOnFirstWrite) {
    FileTransportOptions opts;
    opts.dirname = tmpDir_;
    opts.filename = "lazy.log";
    opts.lazy = true;

    FileTransport transport(std::move(opts));
    EXPECT_FALSE(polycpp::fs::existsSync(logPath("lazy.log")));

    transport.write(LogInfo("info", "first"));
    EXPECT_TRUE(polycpp::fs::existsSync(logPath("lazy.log")));

    std::string content = readFile(logPath("lazy.log"));
    EXPECT_EQ(content, "first" + polycpp::os::EOL);
}

// ---------------------------------------------------------------------------
// Gzip Archive
// ---------------------------------------------------------------------------

TEST_F(FileTransportTest, ZippedArchive) {
    FileTransportOptions opts;
    opts.dirname = tmpDir_;
    opts.filename = "zipped.log";
    opts.maxsize = 50;
    opts.zippedArchive = true;

    FileTransport transport(std::move(opts));

    // Write enough to trigger rotation
    for (int i = 0; i < 10; ++i) {
        transport.write(LogInfo("info", "line_" + std::to_string(i)));
    }

    // The .1 file should NOT exist (compressed away)
    EXPECT_FALSE(polycpp::fs::existsSync(logPath("zipped.log.1")));
    // The .1.gz file SHOULD exist
    EXPECT_TRUE(polycpp::fs::existsSync(logPath("zipped.log.1.gz")));

    // Decompress and verify content is non-empty
    auto compressed = polycpp::fs::readFileSyncBuffer(logPath("zipped.log.1.gz"));
    auto decompressed = polycpp::zlib::gunzipSync(compressed);
    std::string content = decompressed.toString();
    EXPECT_FALSE(content.empty());
}

// ---------------------------------------------------------------------------
// Close
// ---------------------------------------------------------------------------

TEST_F(FileTransportTest, CloseFile) {
    FileTransportOptions opts;
    opts.dirname = tmpDir_;
    opts.filename = "close.log";

    FileTransport transport(std::move(opts));
    transport.write(LogInfo("info", "before close"));

    transport.close();

    // After close, writing again should reopen (lazy behavior in log())
    transport.write(LogInfo("info", "after close"));

    std::string content = readFile(logPath("close.log"));
    std::string expected =
        "before close" + polycpp::os::EOL +
        "after close" + polycpp::os::EOL;
    EXPECT_EQ(content, expected);
}

// ---------------------------------------------------------------------------
// Transport-Level Filtering
// ---------------------------------------------------------------------------

TEST_F(FileTransportTest, TransportLevelFiltering) {
    FileTransportOptions opts;
    opts.dirname = tmpDir_;
    opts.filename = "filter.log";
    opts.level = "error";

    FileTransport transport(std::move(opts));

    // Set up level config for filtering
    auto levels = LevelConfig::npm();
    transport.setLevels(&levels);

    // Log at info (should be filtered out since error < info in severity)
    transport.write(LogInfo("info", "should not appear"));

    // Log at error (should pass)
    transport.write(LogInfo("error", "should appear"));

    std::string content = readFile(logPath("filter.log"));
    EXPECT_EQ(content, "should appear" + polycpp::os::EOL);
}

// ---------------------------------------------------------------------------
// Integration with Logger
// ---------------------------------------------------------------------------

TEST_F(FileTransportTest, IntegrationWithLogger) {
    auto fileOpts = std::make_shared<FileTransport>(FileTransportOptions{
        .filename = logPath("logger.log")
    });

    LoggerOptions loggerOpts;
    loggerOpts.level = "info";
    loggerOpts.transports.push_back(fileOpts);

    Logger logger(std::move(loggerOpts));
    logger.log("info", "from logger");

    std::string content = readFile(logPath("logger.log"));
    EXPECT_FALSE(content.empty());
    EXPECT_NE(content.find("from logger"), std::string::npos);
}
