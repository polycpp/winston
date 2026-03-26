#include <gtest/gtest.h>
#include <polycpp/winston/exception_handler.hpp>
#include <polycpp/winston/logger.hpp>
#include <polycpp/core/error.hpp>
#include <polycpp/core/json.hpp>
#include <stdexcept>
#include <string>

using namespace polycpp::winston;
using polycpp::JsonValue;
using polycpp::JsonObject;
using polycpp::JsonArray;

// ===========================================================================
// getAllInfo Tests
// ===========================================================================

TEST(ExceptionHandlerTest, GetAllInfoBasic) {
    std::runtime_error err("test error");
    LogInfo info = ExceptionHandler::getAllInfo(err);

    EXPECT_EQ(info.level, "error");
    EXPECT_EQ(info.originalLevel, "error");
    EXPECT_EQ(info.message, "test error");

    // Exception flag
    EXPECT_TRUE(info.has("exception"));
    EXPECT_TRUE(info.get("exception").asBool());

    // Error details
    EXPECT_TRUE(info.has("error"));
    auto errorObj = info.get("error").asObject();
    EXPECT_EQ(errorObj.at("message").asString(), "test error");
}

TEST(ExceptionHandlerTest, GetAllInfoPolycppError) {
    // Save and set stackTraceLimit to enable stack capture
    int savedLimit = polycpp::Error::stackTraceLimit.load();
    polycpp::Error::stackTraceLimit.store(10);

    polycpp::Error err("polycpp error");
    LogInfo info = ExceptionHandler::getAllInfo(err);

    // polycpp::Error::what() returns "Error: polycpp error"
    EXPECT_NE(info.message.find("polycpp error"), std::string::npos);

    auto errorObj = info.get("error").asObject();
    EXPECT_NE(errorObj.at("message").asString().find("polycpp error"), std::string::npos);
    // polycpp::Error should have stack info if stackTraceLimit > 0
    // The stack field may or may not be present depending on platform
    // Just verify the error object is well-formed

    polycpp::Error::stackTraceLimit.store(savedLimit);
}

TEST(ExceptionHandlerTest, GetAllInfoHasDate) {
    std::runtime_error err("test");
    LogInfo info = ExceptionHandler::getAllInfo(err);

    EXPECT_TRUE(info.has("date"));
    std::string date = info.get("date").asString();
    // ISO format should contain 'T' and 'Z'
    EXPECT_NE(date.find('T'), std::string::npos);
    EXPECT_NE(date.find('Z'), std::string::npos);
}

TEST(ExceptionHandlerTest, GetAllInfoHasProcessAndOs) {
    std::runtime_error err("test");
    LogInfo info = ExceptionHandler::getAllInfo(err);

    // Process info
    EXPECT_TRUE(info.has("process"));
    auto proc = info.get("process").asObject();
    EXPECT_TRUE(proc.count("pid") > 0);
    EXPECT_GT(proc.at("pid").asNumber(), 0.0);
    EXPECT_TRUE(proc.count("cwd") > 0);
    EXPECT_FALSE(proc.at("cwd").asString().empty());

    // OS info
    EXPECT_TRUE(info.has("os"));
    auto os = info.get("os").asObject();
    EXPECT_TRUE(os.count("loadavg") > 0);
    auto loadavg = os.at("loadavg").asArray();
    EXPECT_EQ(loadavg.size(), 3u);
    EXPECT_TRUE(os.count("uptime") > 0);
    EXPECT_GT(os.at("uptime").asNumber(), 0.0);
}

// ===========================================================================
// getProcessInfo Tests
// ===========================================================================

TEST(ExceptionHandlerTest, GetProcessInfo) {
    auto proc = ExceptionHandler::getProcessInfo();
    EXPECT_GT(proc.at("pid").asNumber(), 0.0);
    EXPECT_FALSE(proc.at("cwd").asString().empty());
    EXPECT_TRUE(proc.count("memoryUsage") > 0);
}

// ===========================================================================
// getOsInfo Tests
// ===========================================================================

TEST(ExceptionHandlerTest, GetOsInfo) {
    auto os = ExceptionHandler::getOsInfo();

    auto loadavg = os.at("loadavg").asArray();
    EXPECT_EQ(loadavg.size(), 3u);

    EXPECT_GT(os.at("uptime").asNumber(), 0.0);
}
