// request_scoped_child.cpp — demonstrate per-request child loggers.

#include <polycpp/winston/winston.hpp>

struct Request {
    std::string id;
    std::string route;
    std::string method;
};

int main() {
    using namespace polycpp;
    using namespace polycpp::winston;

    auto base = createLogger({
        .level       = "info",
        .format      = combine({formats::timestamp(), formats::json()}),
        .transports  = {std::make_shared<ConsoleTransport>()},
        .defaultMeta = {{"service", JsonValue(std::string("api"))}},
    });

    auto handle = [&](const Request& r) {
        auto log = base->child({
            {"request_id", JsonValue(r.id)},
            {"route",      JsonValue(r.route)},
            {"method",     JsonValue(r.method)},
        });
        log.info("request received");
        log.info("database lookup", {{"table", JsonValue(std::string("users"))}});
        log.info("response sent",   {{"status", JsonValue(200)}});
    };

    handle({"req-001", "/users/42", "GET"});
    handle({"req-002", "/orders",   "POST"});

    base->close();
    return 0;
}
