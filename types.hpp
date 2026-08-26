#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>

namespace df {

using Bytes = std::uint64_t;

struct FileSystemInfo {
    std::string device;
    std::string fs_type;
    std::optional<Bytes> total_bytes;
    std::optional<Bytes> available_bytes;
    std::optional<Bytes> used_bytes;
    std::filesystem::path mounted_on;
};

}  // namespace df
