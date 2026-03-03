#pragma once

#include <string>
#include <cstdint>
#include <optional>
#include <filesystem>

using Bytes = std::uint64_t;

struct FileSystemInfo {
  std::string device;
  std::string fs_type;
  std::optional<Bytes> total_bytes;
  std::optional<Bytes> available_bytes;
  std::filesystem::path mounted_on;
};
