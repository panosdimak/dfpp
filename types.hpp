#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

namespace df {

using Bytes = std::uint64_t;

struct MountEntry {
    std::string device;
    std::string fs_type;
    std::filesystem::path mounted_on;
};

struct Usage {
    Bytes total_bytes{};
    Bytes used_bytes{};
    Bytes available_bytes{};
};

struct FileSystemInfo {
    MountEntry mount;
    Usage usage;
};

}  // namespace df
