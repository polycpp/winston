// two_transports.cpp — fan out to console (coloured) and a JSON file.

#include <polycpp/winston/winston.hpp>

int main() {
    using namespace polycpp;
    using namespace polycpp::winston;

    auto consoleFormat = combine({
        formats::colorize(),
        formats::simple(),
    });
    auto fileFormat = combine({
        formats::timestamp(),
        formats::json(),
    });

    auto console = std::make_shared<ConsoleTransport>(ConsoleTransportOptions{
        .level  = "info",
        .format = consoleFormat,
    });

    FileTransportOptions fopts;
    fopts.filename = "app.log";
    fopts.level    = "debug";
    auto file = std::make_shared<FileTransport>(std::move(fopts));
    file->format = fileFormat;

    auto logger = createLogger({
        .level       = "debug",
        .transports  = {console, file},
        .defaultMeta = {{"service", JsonValue(std::string("demo"))}},
    });

    logger->info("server started",  {{"port", JsonValue(3000)}});
    logger->warn("slow request",    {{"route", JsonValue(std::string("/users"))},
                                     {"ms",    JsonValue(412)}});
    logger->debug("cache miss",     {{"key",  JsonValue(std::string("k42"))}});
    logger->close();
    return 0;
}
