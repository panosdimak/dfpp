#pragma once

#include <expected>
#include <filesystem>
#include <string>
#include <vector>

#include "types.hpp"

namespace df {

// Drop pseudo-filesystems, keeping only mounts backed by a real device
auto filter_real(std::vector<FileSystemInfo>& mounts) -> void;

// Drop mounts whose device was already seen, keeping the first
auto dedup_by_device(std::vector<FileSystemInfo>& mounts) -> void;

// Match each path to its longest-prefix mount; fails on the first path that matches none
auto resolve_arg_mounts(const std::vector<std::filesystem::path>& arg_paths, const std::vector<FileSystemInfo>& mounts)
    -> std::expected<std::vector<FileSystemInfo>, std::string>;

}  // namespace df
