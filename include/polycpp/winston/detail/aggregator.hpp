#pragma once

/// @file detail/aggregator.hpp
/// @brief Convenience header that includes all polycpp/winston headers.
///
/// Include this single header for full access to the entire winston library.

// Foundation types
#include <polycpp/winston/log_info.hpp>
#include <polycpp/winston/level_config.hpp>

// Colorizer
#include <polycpp/winston/colorizer.hpp>

// Format system
#include <polycpp/winston/format.hpp>
#include <polycpp/winston/formats/timestamp.hpp>
#include <polycpp/winston/formats/label.hpp>
#include <polycpp/winston/formats/ms.hpp>
#include <polycpp/winston/formats/errors.hpp>
#include <polycpp/winston/formats/metadata.hpp>
#include <polycpp/winston/formats/colorize.hpp>
#include <polycpp/winston/formats/splat.hpp>
#include <polycpp/winston/formats/align.hpp>
#include <polycpp/winston/formats/pad_levels.hpp>
#include <polycpp/winston/formats/uncolorize.hpp>
#include <polycpp/winston/formats/json.hpp>
#include <polycpp/winston/formats/simple.hpp>
#include <polycpp/winston/formats/printf.hpp>
#include <polycpp/winston/formats/logstash.hpp>
#include <polycpp/winston/formats/pretty_print.hpp>
#include <polycpp/winston/formats/cli.hpp>

// Transport base
#include <polycpp/winston/transport.hpp>

// Logger core
#include <polycpp/winston/logger.hpp>
#include <polycpp/winston/profiler.hpp>
#include <polycpp/winston/child_logger.hpp>

// Transports
#include <polycpp/winston/console_transport.hpp>
#include <polycpp/winston/stream_transport.hpp>
#include <polycpp/winston/file_transport.hpp>

// Extras
#include <polycpp/winston/container.hpp>
#include <polycpp/winston/exception_handler.hpp>

// Module entry point (includes all of the above + convenience functions)
#include <polycpp/winston/winston.hpp>
