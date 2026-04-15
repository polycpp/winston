#include <gtest/gtest.h>
#include <polycpp/winston/logger.hpp>
#include <polycpp/winston/profiler.hpp>
#include <polycpp/winston/child_logger.hpp>
#include <any>
#include <string>
#include <vector>

using namespace polycpp::winston;
using polycpp::JsonValue;
using polycpp::JsonObject;

// ---------------------------------------------------------------------------
// Mock transport that records all log calls
// ---------------------------------------------------------------------------
class MockTransport : public Transport {
public:
    MockTransport() : Transport(TransportOptions{}) {}
    explicit MockTransport(TransportOptions opts) : Transport(std::move(opts)) {}

    void log(const LogInfo& info) override {
        logged.push_back(info);
    }

    std::vector<LogInfo> logged;
};

// ---------------------------------------------------------------------------
// Test format: uppercases the message
// ---------------------------------------------------------------------------
class UppercaseFormat : public Format {
public:
    std::optional<LogInfo> transform(LogInfo info) override {
        for (auto& c : info.message) {
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }
        return info;
    }
};

// ---------------------------------------------------------------------------
// Test format: drops messages at a given level
// ---------------------------------------------------------------------------
class DropLevelFormat : public Format {
public:
    explicit DropLevelFormat(const std::string& dropLevel)
        : dropLevel_(dropLevel) {}

    std::optional<LogInfo> transform(LogInfo info) override {
        if (info.originalLevel == dropLevel_ || info.level == dropLevel_) {
            return std::nullopt;
        }
        return info;
    }

private:
    std::string dropLevel_;
};

// ===========================================================================
// Basic Logging
// ===========================================================================

TEST(LoggerTest, LogAtInfoLevel) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });
    logger.info("hello");
    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].level, "info");
    EXPECT_EQ(transport->logged[0].message, "hello");
}

TEST(LoggerTest, LogAtMultipleLevels) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "silly",
        .transports = {transport}
    });
    logger.error("e");
    logger.warn("w");
    logger.info("i");
    ASSERT_EQ(transport->logged.size(), 3u);
    EXPECT_EQ(transport->logged[0].level, "error");
    EXPECT_EQ(transport->logged[1].level, "warn");
    EXPECT_EQ(transport->logged[2].level, "info");
}

TEST(LoggerTest, LogWithMetadata) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });
    logger.info("hello", {{"service", JsonValue(std::string("api"))}});
    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].metadata.at("service").asString(), "api");
}

TEST(LoggerTest, LogWithLogInfoObject) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });
    LogInfo info("info", "direct");
    info.metadata["key"] = JsonValue(std::string("val"));
    logger.log(std::move(info));
    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].message, "direct");
    EXPECT_EQ(transport->logged[0].metadata.at("key").asString(), "val");
}

// ===========================================================================
// Level Filtering
// ===========================================================================

TEST(LoggerTest, LevelFilterBlocksLowerPriority) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "warn",
        .transports = {transport}
    });
    logger.info("should not appear");
    logger.debug("should not appear");
    logger.verbose("should not appear");
    EXPECT_EQ(transport->logged.size(), 0u);
}

TEST(LoggerTest, LevelFilterAllowsHigherPriority) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "warn",
        .transports = {transport}
    });
    logger.error("e");
    logger.warn("w");
    ASSERT_EQ(transport->logged.size(), 2u);
    EXPECT_EQ(transport->logged[0].level, "error");
    EXPECT_EQ(transport->logged[1].level, "warn");
}

TEST(LoggerTest, LevelFilterExactMatch) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });
    logger.info("match");
    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].level, "info");
}

TEST(LoggerTest, ChangeLevelAtRuntime) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "error",
        .transports = {transport}
    });
    logger.info("blocked");
    EXPECT_EQ(transport->logged.size(), 0u);

    logger.setLevel("info");
    logger.info("allowed");
    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].message, "allowed");
}

TEST(LoggerTest, UnknownLevelIgnored) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });
    logger.log("nonexistent", "msg");
    EXPECT_EQ(transport->logged.size(), 0u);
}

// ===========================================================================
// Silent Mode
// ===========================================================================

TEST(LoggerTest, SilentSuppressesAll) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "error",
        .transports = {transport},
        .silent = true
    });
    logger.error("should be silent");
    EXPECT_EQ(transport->logged.size(), 0u);
}

TEST(LoggerTest, SilentToggle) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport},
        .silent = true
    });
    logger.info("blocked");
    EXPECT_EQ(transport->logged.size(), 0u);

    logger.setSilent(false);
    logger.info("allowed");
    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].message, "allowed");
}

// ===========================================================================
// Default Metadata
// ===========================================================================

TEST(LoggerTest, DefaultMetaMerged) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport},
        .defaultMeta = {{"service", JsonValue(std::string("api"))}}
    });
    logger.info("hello");
    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].metadata.at("service").asString(), "api");
}

TEST(LoggerTest, DefaultMetaOverridden) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport},
        .defaultMeta = {{"service", JsonValue(std::string("api"))}}
    });
    logger.info("hello", {{"service", JsonValue(std::string("web"))}});
    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].metadata.at("service").asString(), "web");
}

// ===========================================================================
// Transport Management
// ===========================================================================

TEST(LoggerTest, AddTransport) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{.level = "info"});
    logger.add(transport);
    logger.info("hello");
    ASSERT_EQ(transport->logged.size(), 1u);
}

TEST(LoggerTest, RemoveTransport) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });
    logger.remove(transport);
    logger.info("hello");
    EXPECT_EQ(transport->logged.size(), 0u);
}

TEST(LoggerTest, ClearTransports) {
    auto t1 = std::make_shared<MockTransport>();
    auto t2 = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {t1, t2}
    });
    logger.clear();
    logger.info("hello");
    EXPECT_EQ(t1->logged.size(), 0u);
    EXPECT_EQ(t2->logged.size(), 0u);
}

TEST(LoggerTest, MultipleTransports) {
    auto t1 = std::make_shared<MockTransport>();
    auto t2 = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {t1, t2}
    });
    logger.info("hello");
    ASSERT_EQ(t1->logged.size(), 1u);
    ASSERT_EQ(t2->logged.size(), 1u);
    EXPECT_EQ(t1->logged[0].message, "hello");
    EXPECT_EQ(t2->logged[0].message, "hello");
}

TEST(LoggerTest, CloseEmitsEvent) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });

    bool closeFired = false;
    logger.on(event::Close, [&closeFired]() {
        closeFired = true;
    });

    logger.close();
    EXPECT_TRUE(closeFired);
    // Transports should be cleared
    EXPECT_TRUE(logger.transports().empty());
}

// ===========================================================================
// Configure
// ===========================================================================

TEST(LoggerTest, ConfigureReplacesAll) {
    auto oldTransport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {oldTransport}
    });
    logger.info("before");
    ASSERT_EQ(oldTransport->logged.size(), 1u);

    auto newTransport = std::make_shared<MockTransport>();
    logger.configure(LoggerOptions{
        .level = "warn",
        .transports = {newTransport}
    });

    // Old transport should no longer receive messages
    logger.warn("after");
    EXPECT_EQ(oldTransport->logged.size(), 1u); // still just the one from before
    ASSERT_EQ(newTransport->logged.size(), 1u);
    EXPECT_EQ(newTransport->logged[0].message, "after");

    // Info should be filtered by new level "warn"
    logger.info("filtered");
    EXPECT_EQ(newTransport->logged.size(), 1u);
}

// ===========================================================================
// Format Pipeline
// ===========================================================================

TEST(LoggerTest, FormatApplied) {
    auto transport = std::make_shared<MockTransport>();
    auto fmt = std::make_shared<UppercaseFormat>();
    Logger logger(LoggerOptions{
        .level = "info",
        .format = fmt,
        .transports = {transport}
    });
    logger.info("hello world");
    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].message, "HELLO WORLD");
}

TEST(LoggerTest, FormatFilterDropsMessage) {
    auto transport = std::make_shared<MockTransport>();
    auto fmt = std::make_shared<DropLevelFormat>("debug");
    Logger logger(LoggerOptions{
        .level = "silly",
        .format = fmt,
        .transports = {transport}
    });
    logger.debug("should be dropped");
    EXPECT_EQ(transport->logged.size(), 0u);

    // Other levels should pass through
    logger.info("should pass");
    ASSERT_EQ(transport->logged.size(), 1u);
}

TEST(LoggerTest, NoFormatPassthrough) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });
    logger.info("unchanged");
    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].message, "unchanged");
}

// ===========================================================================
// Level Checking
// ===========================================================================

TEST(LoggerTest, IsLevelEnabled) {
    Logger logger(LoggerOptions{.level = "info"});
    EXPECT_TRUE(logger.isInfoEnabled());
    EXPECT_TRUE(logger.isErrorEnabled());
    EXPECT_TRUE(logger.isWarnEnabled());
    EXPECT_FALSE(logger.isDebugEnabled());
    EXPECT_FALSE(logger.isVerboseEnabled());
    EXPECT_FALSE(logger.isSillyEnabled());
}

TEST(LoggerTest, IsLevelEnabledCustom) {
    Logger logger(LoggerOptions{.level = "info"});
    EXPECT_FALSE(logger.isLevelEnabled("verbose"));
    EXPECT_TRUE(logger.isLevelEnabled("error"));
    EXPECT_TRUE(logger.isLevelEnabled("info"));
}

TEST(LoggerTest, IsHttpEnabled) {
    Logger logger(LoggerOptions{.level = "http"});
    EXPECT_TRUE(logger.isHttpEnabled());
    EXPECT_TRUE(logger.isInfoEnabled());
    EXPECT_FALSE(logger.isVerboseEnabled());
}

// ===========================================================================
// Profiling -- Toggle
// ===========================================================================

TEST(LoggerTest, ProfileToggle) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });

    logger.profile("test");
    // No log yet
    EXPECT_EQ(transport->logged.size(), 0u);

    logger.profile("test");
    // Should have logged with durationMs
    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].level, "info");
    EXPECT_EQ(transport->logged[0].message, "test");
    EXPECT_TRUE(transport->logged[0].has("durationMs"));
    double dur = transport->logged[0].get("durationMs").asNumber();
    EXPECT_GE(dur, 0.0);
}

TEST(LoggerTest, ProfileWithMeta) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });

    logger.profile("test");
    logger.profile("test", {{"extra", JsonValue(std::string("data"))}});

    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_TRUE(transport->logged[0].has("durationMs"));
    EXPECT_EQ(transport->logged[0].get("extra").asString(), "data");
}

// ===========================================================================
// Profiling -- startTimer
// ===========================================================================

TEST(LoggerTest, StartTimerDone) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });

    auto profiler = logger.startTimer();
    profiler.done("completed");

    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].level, "info");
    EXPECT_EQ(transport->logged[0].message, "completed");
    EXPECT_TRUE(transport->logged[0].has("durationMs"));
    double dur = transport->logged[0].get("durationMs").asNumber();
    EXPECT_GE(dur, 0.0);
}

TEST(LoggerTest, StartTimerCustomLevel) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "silly",
        .transports = {transport}
    });

    auto profiler = logger.startTimer();
    LogInfo info;
    info.level = "debug";
    info.message = "custom timer";
    profiler.done(std::move(info));

    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].level, "debug");
    EXPECT_EQ(transport->logged[0].message, "custom timer");
    EXPECT_TRUE(transport->logged[0].has("durationMs"));
}

// ===========================================================================
// Child Logger
// ===========================================================================

TEST(LoggerTest, ChildMergesMetadata) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });

    auto childLogger = logger.child({{"requestId", JsonValue(std::string("abc"))}});
    childLogger.info("hello");

    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].message, "hello");
    EXPECT_EQ(transport->logged[0].metadata.at("requestId").asString(), "abc");
}

TEST(LoggerTest, ChildMetaPrecedence) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport},
        .defaultMeta = {
            {"service", JsonValue(std::string("api"))},
            {"env", JsonValue(std::string("prod"))}
        }
    });

    auto childLogger = logger.child({
        {"service", JsonValue(std::string("web"))},
        {"requestId", JsonValue(std::string("abc"))}
    });

    // Per-call metadata overrides child meta, child meta overrides parent meta
    childLogger.info("hello", {{"requestId", JsonValue(std::string("override"))}});

    ASSERT_EQ(transport->logged.size(), 1u);
    const auto& meta = transport->logged[0].metadata;
    // Per-call wins over child
    EXPECT_EQ(meta.at("requestId").asString(), "override");
    // Child wins over parent
    EXPECT_EQ(meta.at("service").asString(), "web");
    // Parent meta still present when not overridden
    EXPECT_EQ(meta.at("env").asString(), "prod");
}

TEST(LoggerTest, ChildInheritsLevel) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "warn",
        .transports = {transport}
    });

    auto childLogger = logger.child({{"requestId", JsonValue(std::string("abc"))}});
    childLogger.info("should be filtered");
    EXPECT_EQ(transport->logged.size(), 0u);

    childLogger.error("should pass");
    ASSERT_EQ(transport->logged.size(), 1u);
}

TEST(LoggerTest, ChildUsesParentTransports) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });

    auto childLogger = logger.child({});
    childLogger.info("from child");
    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].message, "from child");
}

TEST(LoggerTest, NestedChild) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });

    auto child1 = logger.child({{"a", JsonValue(std::string("1"))}});
    auto child2 = child1.child({{"b", JsonValue(std::string("2"))}});
    child2.info("nested");

    ASSERT_EQ(transport->logged.size(), 1u);
    const auto& meta = transport->logged[0].metadata;
    EXPECT_EQ(meta.at("a").asString(), "1");
    EXPECT_EQ(meta.at("b").asString(), "2");
}

// ===========================================================================
// Custom Levels
// ===========================================================================

TEST(LoggerTest, SyslogLevels) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "debug",
        .levels = LevelConfig::syslog(),
        .transports = {transport}
    });

    logger.emerg("e");
    logger.alert("a");
    logger.crit("c");
    logger.warning("w");
    logger.notice("n");
    ASSERT_EQ(transport->logged.size(), 5u);
    EXPECT_EQ(transport->logged[0].level, "emerg");
    EXPECT_EQ(transport->logged[1].level, "alert");
    EXPECT_EQ(transport->logged[2].level, "crit");
    EXPECT_EQ(transport->logged[3].level, "warning");
    EXPECT_EQ(transport->logged[4].level, "notice");
}

TEST(LoggerTest, CustomLevelConfig) {
    LevelConfig custom;
    custom.levels = {{"fatal", 0}, {"info", 1}, {"trace", 2}};
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .levels = custom,
        .transports = {transport}
    });

    logger.log("fatal", "f");
    logger.log("info", "i");
    logger.log("trace", "t"); // should be blocked (info=1 < trace=2)
    ASSERT_EQ(transport->logged.size(), 2u);
    EXPECT_EQ(transport->logged[0].level, "fatal");
    EXPECT_EQ(transport->logged[1].level, "info");
}

// ===========================================================================
// Events
// ===========================================================================

TEST(LoggerTest, ErrorEventFromTransport) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });

    bool errorFired = false;
    logger.on(event::Error_, [&errorFired](const polycpp::Error&) {
        errorFired = true;
    });

    // Transport emits an error
    transport->emit(event::Error_, polycpp::Error("transport failure"));
    EXPECT_TRUE(errorFired);
}

// ===========================================================================
// Accessors
// ===========================================================================

TEST(LoggerTest, Accessors) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "warn",
        .transports = {transport},
        .silent = true,
        .defaultMeta = {{"key", JsonValue(std::string("val"))}}
    });

    EXPECT_EQ(logger.level(), "warn");
    EXPECT_TRUE(logger.silent());
    EXPECT_EQ(logger.transports().size(), 1u);
    EXPECT_EQ(logger.defaultMeta().at("key").asString(), "val");
    EXPECT_EQ(logger.format(), nullptr);

    logger.setLevel("info");
    EXPECT_EQ(logger.level(), "info");

    logger.setSilent(false);
    EXPECT_FALSE(logger.silent());

    logger.setDefaultMeta({{"newKey", JsonValue(std::string("newVal"))}});
    EXPECT_EQ(logger.defaultMeta().at("newKey").asString(), "newVal");
}

// ===========================================================================
// ChildLogger isLevelEnabled
// ===========================================================================

TEST(LoggerTest, ChildIsLevelEnabled) {
    Logger logger(LoggerOptions{.level = "warn"});
    auto childLogger = logger.child({});
    EXPECT_TRUE(childLogger.isLevelEnabled("error"));
    EXPECT_TRUE(childLogger.isLevelEnabled("warn"));
    EXPECT_FALSE(childLogger.isLevelEnabled("info"));
}

// ===========================================================================
// Profiler default message
// ===========================================================================

TEST(LoggerTest, StartTimerDoneDefaultMessage) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });

    auto profiler = logger.startTimer();
    profiler.done(); // empty message

    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].message, "");
    EXPECT_TRUE(transport->logged[0].has("durationMs"));
}

// ===========================================================================
// Logger with log(level, msg, meta)
// ===========================================================================

TEST(LoggerTest, LogWithLevelAndMeta) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });
    logger.log("info", "with meta", {{"foo", JsonValue(std::string("bar"))}});
    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].message, "with meta");
    EXPECT_EQ(transport->logged[0].metadata.at("foo").asString(), "bar");
}

// ===========================================================================
// OriginalLevel is preserved
// ===========================================================================

TEST(LoggerTest, OriginalLevelPreserved) {
    auto transport = std::make_shared<MockTransport>();
    Logger logger(LoggerOptions{
        .level = "info",
        .transports = {transport}
    });
    logger.info("test");
    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].originalLevel, "info");
}
