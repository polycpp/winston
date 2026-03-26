#pragma once

/// @file formats/metadata.hpp
/// @brief Metadata format -- nests non-standard metadata under a key.

#include <polycpp/winston/format.hpp>
#include <string>
#include <vector>

namespace polycpp {
namespace winston {
namespace formats {

/// @brief Options for the metadata format.
struct MetadataOptions {
    /// Key under which to nest the metadata. Default: "metadata".
    std::string key = "metadata";

    /// Fields to exclude from metadata nesting (kept at top level).
    /// If empty, all metadata keys are moved into the nested object.
    std::vector<std::string> fillExcept;

    /// Fields to include in metadata nesting (only these are moved).
    /// If non-empty, overrides fillExcept.
    std::vector<std::string> fillWith;
};

/// @brief Moves non-standard metadata fields into a nested sub-object.
///
/// By default, moves all metadata keys into `metadata["metadata"]`.
/// Use `fillExcept` to keep certain keys at the top level, or `fillWith`
/// to move only specific keys.
///
/// @see https://github.com/winstonjs/logform#metadata
class MetadataFormat : public Format {
public:
    /// @brief Construct a metadata format.
    /// @param options Metadata options.
    explicit MetadataFormat(MetadataOptions options = {});

    /// @brief Reorganize metadata fields.
    /// @param info The log entry to transform.
    /// @return The transformed LogInfo with reorganized metadata.
    std::optional<LogInfo> transform(LogInfo info) override;

private:
    MetadataOptions opts_;
};

/// @brief Factory function for metadata format.
/// @param options Metadata options.
/// @return Shared pointer to the format.
///
/// @par Example
/// ```cpp
/// auto fmt = metadata();
/// auto fmt = metadata({.fillExcept = {"service"}});
/// auto fmt = metadata({.fillWith = {"requestId"}});
/// ```
///
/// @see https://github.com/winstonjs/logform#metadata
std::shared_ptr<Format> metadata(MetadataOptions options = {});

} // namespace formats
} // namespace winston
} // namespace polycpp

#include <polycpp/winston/detail/formats/metadata.hpp>
