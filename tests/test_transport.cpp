#include <gtest/gtest.h>
#include <polycpp/winston/transport.hpp>
#include <polycpp/winston/level_config.hpp>
#include <any>
#include <vector>

using namespace polycpp::winston;
using polycpp::JsonValue;

// Mock transport that records log calls for testing
class MockTransport : public Transport {
public:
    explicit MockTransport(TransportOptions options = {})
        : Transport(std::move(options)) {}

    void log(const LogInfo& info) override {
        logged.push_back(info);
    }

    std::vector<LogInfo> logged;
};

// A test format that adds a tag to metadata
class TagFormat : public Format {
public:
    explicit TagFormat(const std::string& tag) : tag_(tag) {}

    std::optional<LogInfo> transform(LogInfo info) override {
        info.metadata["tag"] = JsonValue(tag_);
        return info;
    }

private:
    std::string tag_;
};

// A test format that drops everything
class DropAllFormat : public Format {
public:
    std::optional<LogInfo> transform(LogInfo /*info*/) override {
        return std::nullopt;
    }
};

TEST(TransportTest, BasicWrite) {
    auto transport = std::make_shared<MockTransport>();
    LogInfo info("info", "hello world");
    transport->write(info);

    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].level, "info");
    EXPECT_EQ(transport->logged[0].message, "hello world");
}

TEST(TransportTest, SilentSuppressesOutput) {
    TransportOptions opts;
    opts.silent = true;
    auto transport = std::make_shared<MockTransport>(opts);

    LogInfo info("info", "should not appear");
    transport->write(info);

    EXPECT_TRUE(transport->logged.empty());
}

TEST(TransportTest, SilentToggle) {
    auto transport = std::make_shared<MockTransport>();

    LogInfo info1("info", "first");
    transport->write(info1);
    EXPECT_EQ(transport->logged.size(), 1u);

    transport->silent = true;
    LogInfo info2("info", "second");
    transport->write(info2);
    EXPECT_EQ(transport->logged.size(), 1u); // Still 1

    transport->silent = false;
    LogInfo info3("info", "third");
    transport->write(info3);
    EXPECT_EQ(transport->logged.size(), 2u);
}

TEST(TransportTest, LevelFilteringWithLevels) {
    auto config = LevelConfig::npm();

    TransportOptions opts;
    opts.level = "warn"; // Only error(0) and warn(1) should pass
    auto transport = std::make_shared<MockTransport>(opts);
    transport->setLevels(&config);

    // Error should pass (0 <= 1)
    transport->write(LogInfo("error", "err msg"));
    EXPECT_EQ(transport->logged.size(), 1u);

    // Warn should pass (1 <= 1)
    transport->write(LogInfo("warn", "warn msg"));
    EXPECT_EQ(transport->logged.size(), 2u);

    // Info should be filtered (2 > 1)
    transport->write(LogInfo("info", "info msg"));
    EXPECT_EQ(transport->logged.size(), 2u); // Still 2

    // Debug should be filtered (5 > 1)
    transport->write(LogInfo("debug", "debug msg"));
    EXPECT_EQ(transport->logged.size(), 2u); // Still 2
}

TEST(TransportTest, NoLevelAcceptsAll) {
    auto config = LevelConfig::npm();
    auto transport = std::make_shared<MockTransport>();
    transport->setLevels(&config);

    // No transport-level threshold set, so everything passes
    transport->write(LogInfo("error", "a"));
    transport->write(LogInfo("silly", "b"));
    EXPECT_EQ(transport->logged.size(), 2u);
}

TEST(TransportTest, NoLevelConfigAcceptsAll) {
    // Transport has a level but no level config pointer -> accepts all
    TransportOptions opts;
    opts.level = "warn";
    auto transport = std::make_shared<MockTransport>(opts);
    // setLevels not called

    transport->write(LogInfo("silly", "should still pass"));
    EXPECT_EQ(transport->logged.size(), 1u);
}

TEST(TransportTest, PerTransportFormatApplied) {
    TransportOptions opts;
    opts.format = std::make_shared<TagFormat>("tagged");
    auto transport = std::make_shared<MockTransport>(opts);

    LogInfo info("info", "hello");
    transport->write(info);

    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_TRUE(transport->logged[0].has("tag"));
    EXPECT_EQ(transport->logged[0].get("tag").asString(), "tagged");
}

TEST(TransportTest, PerTransportFormatCanFilter) {
    TransportOptions opts;
    opts.format = std::make_shared<DropAllFormat>();
    auto transport = std::make_shared<MockTransport>(opts);

    LogInfo info("info", "should be dropped");
    transport->write(info);

    EXPECT_TRUE(transport->logged.empty());
}

TEST(TransportTest, EmitsLoggedEvent) {
    auto transport = std::make_shared<MockTransport>();
    int loggedCount = 0;
    transport->on(event::Logged, [&loggedCount]() {
        loggedCount++;
    });

    transport->write(LogInfo("info", "hello"));
    EXPECT_EQ(loggedCount, 1);

    transport->write(LogInfo("warn", "world"));
    EXPECT_EQ(loggedCount, 2);
}

TEST(TransportTest, SilentDoesNotEmitLoggedEvent) {
    TransportOptions opts;
    opts.silent = true;
    auto transport = std::make_shared<MockTransport>(opts);
    int loggedCount = 0;
    transport->on(event::Logged, [&loggedCount]() {
        loggedCount++;
    });

    transport->write(LogInfo("info", "hello"));
    EXPECT_EQ(loggedCount, 0);
}

TEST(TransportTest, FilteredDoesNotEmitLoggedEvent) {
    auto config = LevelConfig::npm();
    TransportOptions opts;
    opts.level = "error";
    auto transport = std::make_shared<MockTransport>(opts);
    transport->setLevels(&config);

    int loggedCount = 0;
    transport->on(event::Logged, [&loggedCount]() {
        loggedCount++;
    });

    // info is less severe than error, so it's filtered
    transport->write(LogInfo("info", "hello"));
    EXPECT_EQ(loggedCount, 0);

    // error passes
    transport->write(LogInfo("error", "boom"));
    EXPECT_EQ(loggedCount, 1);
}

TEST(TransportTest, HandleExceptionsFlag) {
    TransportOptions opts;
    opts.handleExceptions = true;
    auto transport = std::make_shared<MockTransport>(opts);
    EXPECT_TRUE(transport->handleExceptions);
}

TEST(TransportTest, HandleRejectionsFlag) {
    TransportOptions opts;
    opts.handleRejections = true;
    auto transport = std::make_shared<MockTransport>(opts);
    EXPECT_TRUE(transport->handleRejections);
}

TEST(TransportTest, AcceptWithLevelAndConfig) {
    auto config = LevelConfig::npm();
    TransportOptions opts;
    opts.level = "info";
    auto transport = std::make_shared<MockTransport>(opts);
    transport->setLevels(&config);

    LogInfo errorInfo("error", "err");
    EXPECT_TRUE(transport->accept(errorInfo));

    LogInfo infoInfo("info", "msg");
    EXPECT_TRUE(transport->accept(infoInfo));

    LogInfo debugInfo("debug", "dbg");
    EXPECT_FALSE(transport->accept(debugInfo));
}

TEST(TransportTest, AcceptWithNoLevel) {
    auto transport = std::make_shared<MockTransport>();
    LogInfo info("silly", "anything");
    EXPECT_TRUE(transport->accept(info));
}

TEST(TransportTest, LevelFilteringUseOriginalLevel) {
    // Verify that level filtering uses originalLevel, not the (possibly colorized) level
    auto config = LevelConfig::npm();
    TransportOptions opts;
    opts.level = "warn";
    auto transport = std::make_shared<MockTransport>(opts);
    transport->setLevels(&config);

    LogInfo info("info", "hello");
    // Simulate colorization: level is modified but originalLevel stays "info"
    info.level = "\033[32minfo\033[39m";
    // originalLevel is still "info" (priority 2), and transport level is "warn" (priority 1)
    // 1 < 2, so it should be filtered
    transport->write(info);
    EXPECT_TRUE(transport->logged.empty());
}
