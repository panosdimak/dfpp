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

// Get total and available bytes of a provided filesystem
void get_fs_stats(FileSystemInfo& info);

// Check whether a provided filesystem is a real device or a pseudo-filesystem
bool is_real_filesystem(const FileSystemInfo& info);
