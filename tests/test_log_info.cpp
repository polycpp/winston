#include <gtest/gtest.h>
#include <polycpp/winston/log_info.hpp>

using namespace polycpp::winston;
using polycpp::JsonValue;
using polycpp::JsonObject;

TEST(LogInfoTest, DefaultConstruction) {
    LogInfo info;
    EXPECT_TRUE(info.level.empty());
    EXPECT_TRUE(info.originalLevel.empty());
    EXPECT_TRUE(info.message.empty());
    EXPECT_TRUE(info.formattedMessage.empty());
    EXPECT_TRUE(info.splat.empty());
    EXPECT_TRUE(info.metadata.empty());
}

TEST(LogInfoTest, TwoArgConstruction) {
    LogInfo info("info", "hello world");
    EXPECT_EQ(info.level, "info");
    EXPECT_EQ(info.originalLevel, "info");
    EXPECT_EQ(info.message, "hello world");
    EXPECT_TRUE(info.formattedMessage.empty());
    EXPECT_TRUE(info.splat.empty());
    EXPECT_TRUE(info.metadata.empty());
}

TEST(LogInfoTest, ThreeArgConstruction) {
    std::vector<JsonValue> splat;
    splat.push_back(JsonValue("world"));
    LogInfo info("info", "hello %s", std::move(splat));
    EXPECT_EQ(info.level, "info");
    EXPECT_EQ(info.originalLevel, "info");
    EXPECT_EQ(info.message, "hello %s");
    EXPECT_EQ(info.splat.size(), 1u);
    EXPECT_EQ(info.splat[0].asString(), "world");
}

TEST(LogInfoTest, FourArgConstruction) {
    std::vector<JsonValue> splat;
    JsonObject meta;
    meta["service"] = JsonValue("api");
    LogInfo info("warn", "low disk", std::move(splat), std::move(meta));
    EXPECT_EQ(info.level, "warn");
    EXPECT_EQ(info.originalLevel, "warn");
    EXPECT_EQ(info.message, "low disk");
    EXPECT_TRUE(info.splat.empty());
    EXPECT_EQ(info.metadata["service"].asString(), "api");
}

TEST(LogInfoTest, OriginalLevelMatchesLevel) {
    // originalLevel should always equal the initial level value
    LogInfo info("error", "something broke");
    EXPECT_EQ(info.originalLevel, info.level);
    // After mutating level, originalLevel stays the same
    info.level = "colorized_error";
    EXPECT_EQ(info.originalLevel, "error");
    EXPECT_NE(info.originalLevel, info.level);
}

TEST(LogInfoTest, MergeAddsNewKeys) {
    LogInfo info;
    info.metadata["service"] = JsonValue("api");

    JsonObject defaults;
    defaults["hostname"] = JsonValue("localhost");
    defaults["pid"] = JsonValue(1234.0);

    info.merge(defaults);

    EXPECT_EQ(info.metadata.size(), 3u);
    EXPECT_EQ(info.metadata["service"].asString(), "api");
    EXPECT_EQ(info.metadata["hostname"].asString(), "localhost");
    EXPECT_EQ(info.metadata["pid"].asNumber(), 1234.0);
}

TEST(LogInfoTest, MergeDoesNotOverwriteExisting) {
    LogInfo info;
    info.metadata["service"] = JsonValue("api");

    JsonObject defaults;
    defaults["service"] = JsonValue("default");
    defaults["hostname"] = JsonValue("localhost");

    info.merge(defaults);

    // Existing key "service" should keep its original value
    EXPECT_EQ(info.metadata["service"].asString(), "api");
    // New key "hostname" should be added
    EXPECT_EQ(info.metadata["hostname"].asString(), "localhost");
}

TEST(LogInfoTest, MergeEmptyDefaults) {
    LogInfo info;
    info.metadata["key"] = JsonValue("value");
    JsonObject empty;
    info.merge(empty);
    EXPECT_EQ(info.metadata.size(), 1u);
}

TEST(LogInfoTest, MergeIntoEmptyMetadata) {
    LogInfo info;
    JsonObject defaults;
    defaults["a"] = JsonValue("b");
    info.merge(defaults);
    EXPECT_EQ(info.metadata.size(), 1u);
    EXPECT_EQ(info.metadata["a"].asString(), "b");
}

TEST(LogInfoTest, ToJsonValueBasic) {
    LogInfo info;
    info.level = "info";
    info.message = "hello world";
    info.metadata["service"] = JsonValue("api");

    auto jv = info.toJsonValue();
    auto obj = jv.asObject();

    EXPECT_EQ(obj["level"].asString(), "info");
    EXPECT_EQ(obj["message"].asString(), "hello world");
    EXPECT_EQ(obj["service"].asString(), "api");
}

TEST(LogInfoTest, ToJsonValueMetadataAtTopLevel) {
    // Verify that metadata keys appear at the top level of the JSON object,
    // matching JS behavior where info = { level, message, ...metadata }
    LogInfo info;
    info.level = "warn";
    info.message = "disk full";
    info.metadata["code"] = JsonValue(507.0);
    info.metadata["path"] = JsonValue("/var/log");

    auto obj = info.toJsonValue().asObject();

    EXPECT_EQ(obj.size(), 4u); // level, message, code, path
    EXPECT_EQ(obj["code"].asNumber(), 507.0);
    EXPECT_EQ(obj["path"].asString(), "/var/log");
}

TEST(LogInfoTest, ToJsonValueEmptyMetadata) {
    LogInfo info("error", "oops");
    auto obj = info.toJsonValue().asObject();
    EXPECT_EQ(obj.size(), 2u); // level, message only
    EXPECT_EQ(obj["level"].asString(), "error");
    EXPECT_EQ(obj["message"].asString(), "oops");
}

TEST(LogInfoTest, HasKeyExists) {
    LogInfo info;
    info.metadata["service"] = JsonValue("api");
    EXPECT_TRUE(info.has("service"));
}

TEST(LogInfoTest, HasKeyDoesNotExist) {
    LogInfo info;
    EXPECT_FALSE(info.has("nonexistent"));
}

TEST(LogInfoTest, GetKeyExists) {
    LogInfo info;
    info.metadata["service"] = JsonValue("api");
    EXPECT_EQ(info.get("service").asString(), "api");
}

TEST(LogInfoTest, GetKeyDoesNotExistThrows) {
    LogInfo info;
    EXPECT_THROW(info.get("nonexistent"), std::out_of_range);
}

TEST(LogInfoTest, GetWithDefaultKeyExists) {
    LogInfo info;
    info.metadata["service"] = JsonValue("api");
    auto val = info.get("service", JsonValue("fallback"));
    EXPECT_EQ(val.asString(), "api");
}

TEST(LogInfoTest, GetWithDefaultKeyMissing) {
    LogInfo info;
    auto val = info.get("missing", JsonValue("fallback"));
    EXPECT_EQ(val.asString(), "fallback");
}

TEST(LogInfoTest, SetNewKey) {
    LogInfo info;
    info.set("service", JsonValue("api"));
    EXPECT_TRUE(info.has("service"));
    EXPECT_EQ(info.get("service").asString(), "api");
}

TEST(LogInfoTest, SetOverwritesExistingKey) {
    LogInfo info;
    info.set("service", JsonValue("old"));
    info.set("service", JsonValue("new"));
    EXPECT_EQ(info.get("service").asString(), "new");
}

TEST(LogInfoTest, RemoveExistingKey) {
    LogInfo info;
    info.metadata["service"] = JsonValue("api");
    EXPECT_TRUE(info.remove("service"));
    EXPECT_FALSE(info.has("service"));
}

TEST(LogInfoTest, RemoveNonexistentKey) {
    LogInfo info;
    EXPECT_FALSE(info.remove("nonexistent"));
}

TEST(LogInfoTest, SplatField) {
    LogInfo info("info", "hello %s %d");
    info.splat.push_back(JsonValue("world"));
    info.splat.push_back(JsonValue(42.0));
    EXPECT_EQ(info.splat.size(), 2u);
}

TEST(LogInfoTest, FormattedMessageField) {
    LogInfo info("info", "hello");
    EXPECT_TRUE(info.formattedMessage.empty());
    info.formattedMessage = "info: hello";
    EXPECT_EQ(info.formattedMessage, "info: hello");
}
