#include "fs_info.hpp"

#include <sys/statvfs.h>

#include <cstddef>
#include <expected>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <system_error>

namespace df {

namespace {

auto parse_mount_line(std::string_view line) -> std::optional<MountEntry> {
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

    return MountEntry{
        .device{*device},
        .fs_type{*fstype},
        .mounted_on{*mount},
    };
}

}  // namespace

auto read_mounts(const std::filesystem::path& fpath) -> std::expected<std::vector<MountEntry>, std::error_code> {
    std::vector<MountEntry> vmounts;
    std::ifstream is{fpath};

    if (!is.is_open()) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    std::string line;
    while (std::getline(is, line)) {
        if (auto info = parse_mount_line(line); info) {
            vmounts.push_back(std::move(*info));
        }
    }

    if (is.bad()) {
        return std::unexpected(std::make_error_code(std::errc::io_error));
    }

    return vmounts;
}

auto get_fs_stats(const std::filesystem::path& mount_point) -> std::expected<Usage, std::error_code> {
    struct statvfs buf;
    const auto ret = ::statvfs(mount_point.c_str(), &buf);
    if (ret != 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    const Bytes total_bytes = buf.f_blocks * buf.f_frsize;
    const Bytes available_bytes = buf.f_bavail * buf.f_frsize;
    const Bytes free_bytes = buf.f_bfree * buf.f_frsize;
    const Bytes used_bytes = total_bytes - free_bytes;

    return Usage{.total_bytes = total_bytes, .used_bytes = used_bytes, .available_bytes = available_bytes};
}

}  // namespace df
