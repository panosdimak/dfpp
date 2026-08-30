#include "fs_info.hpp"

#include <sys/statvfs.h>

#include <cstddef>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <system_error>

namespace df {

namespace {

auto parse_mount_line(std::string_view line) -> std::optional<FileSystemInfo> {
    std::size_t start = 0;

    auto next_field = [&]() -> std::optional<std::string_view> {
        if (auto pos = line.find(' ', start); pos != std::string_view::npos) {
            const auto field = line.substr(start, pos - start);
            start = pos + 1;
            return field;
        }
        return std::nullopt;
    };

    const auto device = next_field();
    const auto mount = next_field();
    const auto fstype = next_field();

    if (!device || !mount || !fstype) {
        return std::nullopt;
    }

    return FileSystemInfo{
        .device{*device},
        .fs_type{*fstype},
        .mounted_on{*mount},
    };
}

}  // namespace

auto read_mounts(const std::filesystem::path& fpath) -> std::expected<std::vector<FileSystemInfo>, std::string> {
    std::vector<FileSystemInfo> vmounts;
    std::ifstream file(fpath);
    std::string line;

    if (!file.is_open()) {
        return std::unexpected(
            std::format("{}: {}", fpath.string(), std::error_code(errno, std::system_category()).message())
        );
    }

    while (std::getline(file, line)) {
        if (auto info = parse_mount_line(line); info) {
            vmounts.push_back(std::move(*info));
        }
    }

    return vmounts;
}

auto get_fs_stats(FileSystemInfo& info) -> void {
    struct statvfs buf;
    const auto ret = ::statvfs(info.mounted_on.c_str(), &buf);
    if (!ret) {
        info.total_bytes = buf.f_blocks * buf.f_frsize;
        info.available_bytes = buf.f_bavail * buf.f_frsize;
        Bytes free_bytes = buf.f_bfree * buf.f_frsize;
        info.used_bytes = *info.total_bytes - free_bytes;
    }
}

}  // namespace df
