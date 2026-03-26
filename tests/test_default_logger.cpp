#include <gtest/gtest.h>
#include <polycpp/winston/winston.hpp>
#include <any>
#include <string>
#include <vector>

using namespace polycpp::winston;
using polycpp::JsonValue;
using polycpp::JsonObject;

// ---------------------------------------------------------------------------
// Mock transport that records all log calls
// ---------------------------------------------------------------------------
class DefaultMockTransport : public Transport {
public:
    DefaultMockTransport() : Transport(TransportOptions{}) {}

    void log(const LogInfo& info) override {
        logged.push_back(info);
    }

    std::vector<LogInfo> logged;
};

// ===========================================================================
// createLogger
// ===========================================================================

TEST(DefaultLoggerTest, CreateLoggerReturnsLogger) {
    auto logger = createLogger();
    ASSERT_NE(logger, nullptr);
}

TEST(DefaultLoggerTest, CreateLoggerWithOptions) {
    auto transport = std::make_shared<DefaultMockTransport>();
    auto logger = createLogger(LoggerOptions{
        .level = "debug",
        .transports = {transport}
    });

    logger->debug("test");
    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].level, "debug");
    EXPECT_EQ(transport->logged[0].message, "test");
}

// ===========================================================================
// Default Logger Singleton
// ===========================================================================

TEST(DefaultLoggerTest, DefaultLoggerExists) {
    Logger& logger = defaultLogger();
    // Should be a valid Logger (level defaults to "info")
    EXPECT_EQ(logger.level(), "info");
}

TEST(DefaultLoggerTest, DefaultLoggerIsSingleton) {
    Logger& first = defaultLogger();
    Logger& second = defaultLogger();
    EXPECT_EQ(&first, &second);
}

// ===========================================================================
// Namespace-Level Convenience Functions
// ===========================================================================

TEST(DefaultLoggerTest, NamespaceLevelFunctions) {
    auto transport = std::make_shared<DefaultMockTransport>();
    // Configure default logger with our transport
    polycpp::winston::add(transport);

    polycpp::winston::info("hello");
    ASSERT_GE(transport->logged.size(), 1u);

    bool found = false;
    for (const auto& entry : transport->logged) {
        if (entry.message == "hello" && entry.level == "info") {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);

    // Clean up: remove transport from default logger
    polycpp::winston::remove(transport);
}

TEST(DefaultLoggerTest, NamespaceConfigure) {
    // Save original state
    auto origLevel = defaultLogger().level();

    polycpp::winston::configure(LoggerOptions{.level = "debug"});
    EXPECT_EQ(defaultLogger().level(), "debug");

    // Restore
    polycpp::winston::configure(LoggerOptions{.level = origLevel});
}

TEST(DefaultLoggerTest, NamespaceAdd) {
    auto transport = std::make_shared<DefaultMockTransport>();
    polycpp::winston::add(transport);
    EXPECT_FALSE(defaultLogger().transports().empty());

    // Clean up
    polycpp::winston::remove(transport);
}

TEST(DefaultLoggerTest, NamespaceRemove) {
    auto transport = std::make_shared<DefaultMockTransport>();
    polycpp::winston::add(transport);
    auto sizeBefore = defaultLogger().transports().size();

    polycpp::winston::remove(transport);
    EXPECT_EQ(defaultLogger().transports().size(), sizeBefore - 1);
}

TEST(DefaultLoggerTest, NamespaceClear) {
    auto t1 = std::make_shared<DefaultMockTransport>();
    auto t2 = std::make_shared<DefaultMockTransport>();
    polycpp::winston::add(t1);
    polycpp::winston::add(t2);

    polycpp::winston::clear();
    EXPECT_TRUE(defaultLogger().transports().empty());
}

TEST(DefaultLoggerTest, NamespaceProfile) {
    auto transport = std::make_shared<DefaultMockTransport>();
    polycpp::winston::add(transport);

    polycpp::winston::profile("test-profile");
    polycpp::winston::profile("test-profile");

    // Should have logged with durationMs
    bool found = false;
    for (const auto& entry : transport->logged) {
        if (entry.message == "test-profile" && entry.has("durationMs")) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);

    polycpp::winston::remove(transport);
}

TEST(DefaultLoggerTest, NamespaceStartTimer) {
    auto transport = std::make_shared<DefaultMockTransport>();
    polycpp::winston::add(transport);

    auto p = polycpp::winston::startTimer();
    p.done("timer done");

    bool found = false;
    for (const auto& entry : transport->logged) {
        if (entry.message == "timer done" && entry.has("durationMs")) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);

    polycpp::winston::remove(transport);
}

TEST(DefaultLoggerTest, NamespaceChild) {
    auto transport = std::make_shared<DefaultMockTransport>();
    polycpp::winston::add(transport);

    auto c = polycpp::winston::child({{"service", JsonValue(std::string("api"))}});
    c.info("from child");

    bool found = false;
    for (const auto& entry : transport->logged) {
        if (entry.message == "from child") {
            auto it = entry.metadata.find("service");
            if (it != entry.metadata.end() && it->second.asString() == "api") {
                found = true;
                break;
            }
        }
    }
    EXPECT_TRUE(found);

    polycpp::winston::remove(transport);
}

// ===========================================================================
// Loggers Container
// ===========================================================================

TEST(DefaultLoggerTest, LoggersContainer) {
    auto logger = polycpp::winston::loggers().get("api");
    ASSERT_NE(logger, nullptr);

    auto same = polycpp::winston::loggers().get("api");
    EXPECT_EQ(logger.get(), same.get());

    // Clean up
    polycpp::winston::loggers().close("api");
}

TEST(DefaultLoggerTest, LoggersContainerIsSingleton) {
    Container& first = polycpp::winston::loggers();
    Container& second = polycpp::winston::loggers();
    EXPECT_EQ(&first, &second);
}

// ===========================================================================
// Logging convenience with levels
// ===========================================================================

TEST(DefaultLoggerTest, NamespaceErrorWarnFunctions) {
    auto transport = std::make_shared<DefaultMockTransport>();
    polycpp::winston::add(transport);

    polycpp::winston::error("err msg");
    polycpp::winston::warn("warn msg");

    int errorCount = 0;
    int warnCount = 0;
    for (const auto& entry : transport->logged) {
        if (entry.level == "error" && entry.message == "err msg") ++errorCount;
        if (entry.level == "warn" && entry.message == "warn msg") ++warnCount;
    }
    EXPECT_GE(errorCount, 1);
    EXPECT_GE(warnCount, 1);

    polycpp::winston::remove(transport);
}

TEST(DefaultLoggerTest, NamespaceLogWithLevelAndMeta) {
    auto transport = std::make_shared<DefaultMockTransport>();
    polycpp::winston::add(transport);

    polycpp::winston::log("info", "meta test", {{"key", JsonValue(std::string("val"))}});

    bool found = false;
    for (const auto& entry : transport->logged) {
        if (entry.message == "meta test") {
            auto it = entry.metadata.find("key");
            if (it != entry.metadata.end() && it->second.asString() == "val") {
                found = true;
                break;
            }
        }
    }
    EXPECT_TRUE(found);

    polycpp::winston::remove(transport);
}

// ===========================================================================
// Version
// ===========================================================================

TEST(DefaultLoggerTest, VersionExists) {
    EXPECT_EQ(polycpp::winston::version(), "0.1.0");
}

// ===========================================================================
// addColors
// ===========================================================================

TEST(DefaultLoggerTest, AddColorsWorks) {
    // Just verify it doesn't throw
    EXPECT_NO_THROW(polycpp::winston::addColors({{"custom", "red"}}));
}
