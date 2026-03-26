#include <gtest/gtest.h>
#include <polycpp/winston/container.hpp>
#include <polycpp/winston/logger.hpp>
#include <polycpp/winston/transport.hpp>
#include <any>
#include <string>
#include <vector>

using namespace polycpp::winston;
using polycpp::JsonValue;
using polycpp::JsonObject;

// ---------------------------------------------------------------------------
// Mock transport that records all log calls
// ---------------------------------------------------------------------------
class ContainerMockTransport : public Transport {
public:
    ContainerMockTransport() : Transport(TransportOptions{}) {}

    void log(const LogInfo& info) override {
        logged.push_back(info);
    }

    std::vector<LogInfo> logged;
};

// ===========================================================================
// Basic Operations
// ===========================================================================

TEST(ContainerTest, GetCreatesLogger) {
    Container container;
    auto logger = container.get("api");
    ASSERT_NE(logger, nullptr);
}

TEST(ContainerTest, GetReturnsSameInstance) {
    Container container;
    auto first = container.get("api");
    auto second = container.get("api");
    EXPECT_EQ(first.get(), second.get());
}

TEST(ContainerTest, HasReturnsTrue) {
    Container container;
    container.get("api");
    EXPECT_TRUE(container.has("api"));
}

TEST(ContainerTest, HasReturnsFalse) {
    Container container;
    EXPECT_FALSE(container.has("nonexistent"));
}

// ===========================================================================
// Close Operations
// ===========================================================================

TEST(ContainerTest, CloseById) {
    Container container;
    container.get("api");
    EXPECT_TRUE(container.has("api"));

    container.close("api");
    EXPECT_FALSE(container.has("api"));
}

TEST(ContainerTest, CloseAll) {
    Container container;
    container.get("api");
    container.get("web");
    EXPECT_TRUE(container.has("api"));
    EXPECT_TRUE(container.has("web"));

    container.close();
    EXPECT_FALSE(container.has("api"));
    EXPECT_FALSE(container.has("web"));
}

TEST(ContainerTest, CloseNonexistentIsNoop) {
    Container container;
    EXPECT_NO_THROW(container.close("nonexistent"));
}

// ===========================================================================
// Add Alias
// ===========================================================================

TEST(ContainerTest, AddAlias) {
    Container container;
    auto first = container.add("api");
    auto second = container.get("api");
    EXPECT_EQ(first.get(), second.get());
}

// ===========================================================================
// Options
// ===========================================================================

TEST(ContainerTest, GetWithOptions) {
    Container container;
    auto logger = container.get("api", LoggerOptions{.level = "debug"});
    EXPECT_EQ(logger->level(), "debug");
}

TEST(ContainerTest, DefaultOptions) {
    Container container(LoggerOptions{.level = "warn"});
    auto logger = container.get("api");
    EXPECT_EQ(logger->level(), "warn");
}

TEST(ContainerTest, GetWithTransportOptions) {
    auto transport = std::make_shared<ContainerMockTransport>();
    Container container;
    auto logger = container.get("api", LoggerOptions{
        .level = "info",
        .transports = {transport}
    });

    logger->info("hello");
    ASSERT_EQ(transport->logged.size(), 1u);
    EXPECT_EQ(transport->logged[0].message, "hello");
}

// ===========================================================================
// Auto-Removal on Close
// ===========================================================================

TEST(ContainerTest, AutoRemoveOnClose) {
    Container container;
    auto logger = container.get("api");
    EXPECT_TRUE(container.has("api"));

    // Closing the logger directly should trigger auto-removal
    logger->close();
    EXPECT_FALSE(container.has("api"));
}

// ===========================================================================
// Multiple Containers
// ===========================================================================

TEST(ContainerTest, IndependentContainers) {
    Container c1;
    Container c2;

    auto l1 = c1.get("shared");
    auto l2 = c2.get("shared");

    // Different containers, different logger instances
    EXPECT_NE(l1.get(), l2.get());
}
