// custom_format.cpp — write a Format subclass that redacts "password"
// metadata before any finalising format serialises the record.

#include <polycpp/winston/winston.hpp>

class RedactPasswords : public polycpp::winston::Format {
public:
    std::optional<polycpp::winston::LogInfo>
    transform(polycpp::winston::LogInfo info) override {
        if (info.has("password")) {
            info.set("password", polycpp::JsonValue(std::string("***")));
        }
        return info;
    }
};

int main() {
    using namespace polycpp;
    using namespace polycpp::winston;

    auto logger = createLogger({
        .format = combine({
            std::make_shared<RedactPasswords>(),
            formats::timestamp(),
            formats::json(),
        }),
        .transports = {std::make_shared<ConsoleTransport>()},
    });

    logger->info("login", {
        {"user",     JsonValue(std::string("alice"))},
        {"password", JsonValue(std::string("hunter2"))},
    });
    logger->close();
    return 0;
}
