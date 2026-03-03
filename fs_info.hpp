#pragma once

#include <vector>
#include <string_view>
#include "types.hpp"

std::optional<FileSystemInfo> parse_mount_line(std::string_view line);

std::vector<FileSystemInfo> read_mounts();
