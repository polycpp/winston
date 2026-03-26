#include <gtest/gtest.h>
#include <polycpp/winston/stream_transport.hpp>
#include <polycpp/winston/logger.hpp>
#include <polycpp/os.hpp>
#include <polycpp/core/error.hpp>
#include <any>
#include <sstream>
#include <string>
#include <vector>

using namespace polycpp::winston;
using polycpp::JsonValue;
using polycpp::JsonObject;

// ===========================================================================
// Basic Output
// ===========================================================================

TEST(StreamTransportTest, WritesToStream) {
    std::ostringstream oss;
    StreamTransportOptions opts;
    opts.stream = &oss;
    auto transport = std::make_shared<StreamTransport>(opts);

    LogInfo info("info", "hello world");
    transport->write(info);

    std::string output = oss.str();
    EXPECT_NE(output.find("hello world"), std::string::npos);
}

TEST(StreamTransportTest, UsesFormattedMessage) {
    std::ostringstream oss;
    StreamTransportOptions opts;
    opts.stream = &oss;
    auto transport = std::make_shared<StreamTransport>(opts);

    LogInfo info("info", "raw message");
    info.formattedMessage = "formatted output";
    transport->write(info);

    std::string output = oss.str();
    EXPECT_NE(output.find("formatted output"), std::string::npos);
    EXPECT_EQ(output.find("raw message"), std::string::npos);
}

TEST(StreamTransportTest, FallbackToMessage) {
    std::ostringstream oss;
    StreamTransportOptions opts;
    opts.stream = &oss;
    auto transport = std::make_shared<StreamTransport>(opts);

    LogInfo info("info", "fallback message");
    // formattedMessage is empty
    transport->write(info);

    std::string output = oss.str();
    EXPECT_NE(output.find("fallback message"), std::string::npos);
}

// ===========================================================================
// EOL Handling
// ===========================================================================

TEST(StreamTransportTest, CustomEol) {
    std::ostringstream oss;
    StreamTransportOptions opts;
    opts.stream = &oss;
    opts.eol = "\n\n";
    auto transport = std::make_shared<StreamTransport>(opts);

    LogInfo info("info", "test");
    transport->write(info);

    std::string output = oss.str();
    EXPECT_EQ(output, "test\n\n");
}

TEST(StreamTransportTest, DefaultEol) {
    std::ostringstream oss;
    StreamTransportOptions opts;
    opts.stream = &oss;
    auto transport = std::make_shared<StreamTransport>(opts);

    LogInfo info("info", "test");
    transport->write(info);

    std::string expected = "test" + polycpp::os::EOL;
    EXPECT_EQ(oss.str(), expected);
}

// ===========================================================================
// Error Handling
// ===========================================================================

TEST(StreamTransportTest, NullStreamThrows) {
    StreamTransportOptions opts;
    opts.stream = nullptr;
    EXPECT_THROW(
        { StreamTransport transport(opts); },
        polycpp::Error
    );
}

// ===========================================================================
// Events
// ===========================================================================

TEST(StreamTransportTest, EmitsLoggedEvent) {
    std::ostringstream oss;
    StreamTransportOptions opts;
    opts.stream = &oss;
    auto transport = std::make_shared<StreamTransport>(opts);

    bool loggedFired = false;
    transport->on("logged", [&loggedFired](const std::vector<std::any>&) {
        loggedFired = true;
    });

    LogInfo info("info", "test");
    transport->write(info);

    EXPECT_TRUE(loggedFired);
}

// ===========================================================================
// Transport-Level Filtering
// ===========================================================================

TEST(StreamTransportTest, TransportLevelFiltering) {
    std::ostringstream oss;
    StreamTransportOptions opts;
    opts.stream = &oss;
    opts.level = "error";
    auto transport = std::make_shared<StreamTransport>(opts);

    // Set up level config (normally done by Logger::add)
    LevelConfig levels = LevelConfig::npm();
    transport->setLevels(&levels);

    LogInfo info("info", "should not appear");
    transport->write(info);

    EXPECT_TRUE(oss.str().empty());
}

// ===========================================================================
// Integration with Logger
// ===========================================================================

TEST(StreamTransportTest, WorksWithLogger) {
    std::ostringstream oss;
    StreamTransportOptions opts;
    opts.stream = &oss;
    auto transport = std::make_shared<StreamTransport>(opts);

    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });
    logger.info("from logger");

    std::string output = oss.str();
    EXPECT_NE(output.find("from logger"), std::string::npos);
}
