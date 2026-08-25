#include "fs_info.hpp"

#include <sys/statvfs.h>

#include <expected>
#include <filesystem>
#include <fstream>
#include <string>

auto parse_mount_line(std::string_view line) -> std::optional<FileSystemInfo> {
    FileSystemInfo info{};
    std::size_t start = 0;

    auto next_field = [&]() -> std::optional<std::string_view> {
        if (auto pos = line.find(' ', start); pos != std::string_view::npos) {
            auto field = line.substr(start, pos - start);
            start = pos + 1;
            return field;
        }
        return std::nullopt;
    };

    auto device = next_field();
    auto mount = next_field();
    auto fstype = next_field();

    if (!device || !mount || !fstype) {
        return std::nullopt;
    }

    info.device = *device;
    info.mounted_on = std::filesystem::path{*mount};
    info.fs_type = *fstype;

    return info;
}

auto read_mounts(const std::filesystem::path& fpath) -> std::expected<std::vector<FileSystemInfo>, std::string> {
    std::vector<FileSystemInfo> vmounts;
    std::ifstream file(fpath);
    std::string line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (auto info = parse_mount_line(line); info) {
                vmounts.push_back(std::move(*info));
            }
        }
        return vmounts;
    } else {
        return std::unexpected("Could not open file");
    }
}

auto get_fs_stats(FileSystemInfo& info) -> void {
    struct statvfs buf;
    int ret = statvfs(info.mounted_on.c_str(), &buf);
    if (!ret) {
        info.total_bytes = buf.f_blocks * buf.f_frsize;
        info.available_bytes = buf.f_bavail * buf.f_frsize;
        info.used_bytes = *info.total_bytes - *info.available_bytes;
    }

    return;
}

auto is_real_filesystem(const FileSystemInfo& info) -> bool {
    return info.device.starts_with("/dev");
}

auto find_mount_for_path(const std::vector<FileSystemInfo>& entries, const std::filesystem::path& target)
    -> std::optional<FileSystemInfo> {
    size_t max_length = 0;
    std::optional<FileSystemInfo> max_entry = std::nullopt;
    for (const auto& entry : entries) {
        if (!target.lexically_relative(entry.mounted_on).string().starts_with("..")) {
            if (entry.mounted_on.string().size() > max_length) {
                max_length = entry.mounted_on.string().size();
                max_entry = entry;
            }
        }
    }
    return max_entry;
}
