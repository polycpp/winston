# polycpp/winston

C++ port of [winston](https://www.npmjs.com/package/winston) for [polycpp](https://github.com/enricohuang/polycpp).

> A logger for just about everything.

## Overview

`polycpp/winston` is a C++20 logging library that mirrors the API design of npm's winston package. It provides a flexible, composable logging system with multiple transports, a rich format pipeline, and a familiar API for developers who know winston from the Node.js ecosystem.

## Features

- **Multiple transports** -- Console, File, Stream (extensible)
- **Composable format pipeline** -- timestamp, colorize, json, simple, printf, and more
- **Level-based filtering** -- npm, syslog, and cli presets, or define your own
- **Child loggers** -- lightweight metadata overlays
- **Profiling** -- built-in timer support
- **Exception/rejection handling** -- catch and log uncaught errors

## Prerequisites

- C++20 compiler (GCC 13+ or Clang 16+)
- CMake 3.20+
- Ninja (recommended)

## Build

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
```

## Usage

```cpp
#include <polycpp/winston/winston.hpp>

int main() {
    auto logger = polycpp::winston::createLogger({
        .level = "info",
        .transports = { std::make_shared<polycpp::winston::transports::Console>() }
    });

    logger->info("Hello from winston!");
    logger->warn("Something might be wrong");
    logger->error("Something went wrong", {{"code", 500}});

    return 0;
}
```

## License

MIT -- see [LICENSE](LICENSE).
