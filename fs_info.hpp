#pragma once

#include <vector>
#include <string_view>
#include <expected>
#include "types.hpp"

// Parse a single line from /proc/mounts.
// Returns nullopt if parsing fails
std::optional<FileSystemInfo> parse_mount_line(std::string_view line);

// Read and parse all mounts from /proc/mounts
std::expected<std::vector<FileSystemInfo>, std::string> read_mounts(const std::filesystem::path& fpath);
