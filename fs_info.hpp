#pragma once

#include <expected>
#include <string_view>
#include <vector>

#include "types.hpp"

// Parse a single line from /proc/mounts.
// Returns nullopt if parsing fails
auto parse_mount_line(std::string_view line) -> std::optional<FileSystemInfo>;

// Read and parse all mounts from /proc/mounts
auto read_mounts(const std::filesystem::path& fpath) -> std::expected<std::vector<FileSystemInfo>, std::string>;

// Get total, used and available bytes of a provided filesystem
auto get_fs_stats(FileSystemInfo& info) -> void;

// Check whether a provided filesystem is a real device or a pseudo-filesystem
auto is_real_filesystem(const FileSystemInfo& info) -> bool;

auto find_mount_for_path(const std::vector<FileSystemInfo>& entries, const std::filesystem::path& target)
    -> std::optional<FileSystemInfo>;
