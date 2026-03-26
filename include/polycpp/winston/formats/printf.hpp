#pragma once

/// @file formats/printf.hpp
/// @brief Printf format -- user-provided template function sets formattedMessage.

#include <polycpp/winston/format.hpp>
#include <functional>
#include <string>

namespace polycpp {
namespace winston {
namespace formats {

/// @brief Sets formattedMessage using a user-provided template function.
///
/// The function receives a const reference to the LogInfo and returns
/// the formatted output string.
///
/// @see https://github.com/winstonjs/logform#printf
class PrintfFormat : public Format {
public:
    /// @brief Construct a printf format.
    /// @param fn Template function that receives LogInfo and returns the output string.
    explicit PrintfFormat(std::function<std::string(const LogInfo&)> fn);

    /// @brief Apply the template function to produce formattedMessage.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo with formattedMessage set.
    std::optional<LogInfo> transform(LogInfo info) override;

private:
    std::function<std::string(const LogInfo&)> fn_;
};

/// @brief Factory function for printf format.
/// @param fn Template function.
/// @return Shared pointer to the format.
///
/// @par Example
/// ```cpp
/// auto fmt = printf([](const LogInfo& info) {
///     return info.level + " - " + info.message;
/// });
/// ```
///
/// @see https://github.com/winstonjs/logform#printf
std::shared_ptr<Format> printf(std::function<std::string(const LogInfo&)> fn);

} // namespace formats
} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/formats/printf.hpp>
