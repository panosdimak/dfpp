#include <string>
#include "fs_info.hpp"

std::optional<FileSystemInfo> parse_mount_line(std::string_view line) {
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

    if (!device || !mount || ! fstype) {
        return std::nullopt;
    }

    info.device = *device;
    info.mounted_on = std::filesystem::path{*mount};
    info.fs_type = *fstype;

    return info;
}
