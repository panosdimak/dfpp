#pragma once

#include <expected>
#include <filesystem>
#include <system_error>
#include <vector>

#include "types.hpp"

namespace df {

// Read and parse all mounts from /proc/mounts
auto read_mounts(const std::filesystem::path& fpath) -> std::expected<std::vector<MountEntry>, std::error_code>;

// Get total, used and available bytes of a provided filesystem
auto get_fs_stats(const std::filesystem::path& mount_point) -> std::expected<Usage, std::error_code>;

}  // namespace df
