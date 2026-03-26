#pragma once

/// @file formats/pretty_print.hpp
/// @brief PrettyPrint format -- produces human-readable inspect output.

#include <polycpp/winston/format.hpp>

namespace polycpp {
namespace winston {
namespace formats {

/// @brief Options for the prettyPrint format.
struct PrettyPrintOptions {
    /// Inspection depth. Default: 2.
    int depth = 2;

    /// Use ANSI colors in output. Default: false.
    bool colorize = false;
};

/// @brief Produces human-readable output using polycpp::util::inspect.
///
/// Sets `formattedMessage` to the inspect output of the log entry
/// converted to a JsonValue.
///
/// @see https://github.com/winstonjs/logform#prettyprint
class PrettyPrintFormat : public Format {
public:
    /// @brief Construct a prettyPrint format.
    /// @param options PrettyPrint options.
    explicit PrettyPrintFormat(PrettyPrintOptions options = {});

    /// @brief Format the log entry using util::inspect.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo with formattedMessage set.
    std::optional<LogInfo> transform(LogInfo info) override;

private:
    PrettyPrintOptions opts_;
};

/// @brief Factory function for prettyPrint format.
/// @param options PrettyPrint options.
/// @return Shared pointer to the format.
///
/// @par Example
/// ```cpp
/// auto fmt = prettyPrint();
/// auto fmt = prettyPrint({.depth = 5, .colorize = true});
/// ```
///
/// @see https://github.com/winstonjs/logform#prettyprint
std::shared_ptr<Format> prettyPrint(PrettyPrintOptions options = {});

} // namespace formats
} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/formats/pretty_print.hpp>
