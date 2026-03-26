#pragma once

/// @file detail/formats/pretty_print.hpp
/// @brief Inline implementation for PrettyPrintFormat.

#include <polycpp/winston/formats/pretty_print.hpp>
#include <polycpp/util/util.hpp>

namespace polycpp {
namespace winston {
namespace formats {

inline PrettyPrintFormat::PrettyPrintFormat(PrettyPrintOptions options)
    : opts_(std::move(options)) {}

inline std::optional<LogInfo> PrettyPrintFormat::transform(LogInfo info) {
    auto jv = info.toJsonValue();
    info.formattedMessage = polycpp::util::inspect(jv, false, opts_.depth, opts_.colorize);
    return info;
}

inline std::shared_ptr<Format> prettyPrint(PrettyPrintOptions options) {
    return std::make_shared<PrettyPrintFormat>(std::move(options));
}

} // namespace formats
} // namespace winston
} // namespace polycpp
