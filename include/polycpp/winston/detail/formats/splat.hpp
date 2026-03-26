#pragma once

/// @file detail/formats/splat.hpp
/// @brief Inline implementation for SplatFormat.

#include <polycpp/winston/formats/splat.hpp>
#include <polycpp/core/json.hpp>
#include <polycpp/core/number.hpp>
#include <polycpp/util/util.hpp>
#include <cmath>
#include <string>

namespace polycpp {
namespace winston {
namespace formats {

namespace detail {

/// @brief Runtime printf-style formatter for splat interpolation.
///
/// Scans the format string for %s, %d, %i, %f, %j, %o, %O, %% tokens
/// and substitutes them with values from the args vector.
///
/// @param fmt The format string.
/// @param args The arguments to substitute.
/// @param nextArg [out] Index of the next unconsumed argument.
/// @return The interpolated string.
inline std::string runtimeFormat(const std::string& fmt,
                                 const std::vector<JsonValue>& args,
                                 size_t& nextArg) {
    std::string result;
    result.reserve(fmt.size() * 2);
    nextArg = 0;

    for (size_t i = 0; i < fmt.size(); ++i) {
        if (fmt[i] != '%' || i + 1 >= fmt.size()) {
            result += fmt[i];
            continue;
        }

        char spec = fmt[i + 1];
        if (spec == '%') {
            result += '%';
            ++i;
            continue;
        }

        if (nextArg >= args.size()) {
            // No more args, leave the specifier as-is
            result += fmt[i];
            continue;
        }

        const auto& arg = args[nextArg];

        switch (spec) {
            case 's':
                if (arg.isString()) {
                    result += arg.asString();
                } else {
                    result += polycpp::JSON::stringify(arg);
                }
                ++nextArg;
                ++i;
                break;
            case 'd':
            case 'i':
                if (arg.isNumber()) {
                    result += polycpp::Number::toString(
                        static_cast<double>(static_cast<int64_t>(arg.asNumber())));
                } else {
                    result += "NaN";
                }
                ++nextArg;
                ++i;
                break;
            case 'f':
                if (arg.isNumber()) {
                    result += polycpp::Number::toString(arg.asNumber());
                } else {
                    result += "NaN";
                }
                ++nextArg;
                ++i;
                break;
            case 'j':
                result += polycpp::JSON::stringify(arg);
                ++nextArg;
                ++i;
                break;
            case 'o':
            case 'O':
                result += polycpp::util::inspect(arg);
                ++nextArg;
                ++i;
                break;
            default:
                // Unknown specifier, leave as-is
                result += fmt[i];
                break;
        }
    }

    return result;
}

} // namespace detail

inline SplatFormat::SplatFormat() {}

inline std::optional<LogInfo> SplatFormat::transform(LogInfo info) {
    if (info.splat.empty() && info.message.find('%') == std::string::npos) {
        return info;
    }

    size_t nextArg = 0;
    info.message = detail::runtimeFormat(info.message, info.splat, nextArg);

    // Merge remaining object-typed splat args into metadata
    for (size_t i = nextArg; i < info.splat.size(); ++i) {
        if (info.splat[i].isObject()) {
            const auto& obj = info.splat[i].asObject();
            for (const auto& [key, value] : obj) {
                info.metadata[key] = value;
            }
        }
    }

    info.splat.clear();
    return info;
}

inline std::shared_ptr<Format> splat() {
    return std::make_shared<SplatFormat>();
}

} // namespace formats
} // namespace winston
} // namespace polycpp
