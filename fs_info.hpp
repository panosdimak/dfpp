#pragma once

#include <expected>
#include <filesystem>
#include <vector>

#include "types.hpp"

namespace df {

// Read and parse all mounts from /proc/mounts
auto read_mounts(const std::filesystem::path& fpath) -> std::expected<std::vector<FileSystemInfo>, std::string>;

// Get total, used and available bytes of a provided filesystem
auto get_fs_stats(FileSystemInfo& info) -> void;

}  // namespace df
