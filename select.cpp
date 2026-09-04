#include "select.hpp"

#include <cstddef>
#include <optional>
#include <set>
#include <string>

namespace df {

namespace {

auto is_real_filesystem(const MountEntry& info) -> bool {
    return info.device.starts_with("/dev");
}

auto find_mount_for_path(const std::vector<MountEntry>& entries, const std::filesystem::path& target)
    -> std::optional<MountEntry> {
    std::size_t max_length = 0;
    std::optional<MountEntry> max_entry = std::nullopt;
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

}  // namespace

auto filter_real(std::vector<MountEntry>& mounts) -> void {
    std::erase_if(mounts, [](const auto& fs) { return !is_real_filesystem(fs); });
}

auto dedup_by_device(std::vector<MountEntry>& mounts) -> void {
    std::set<std::string> seen;
    std::erase_if(mounts, [&seen](const auto& fs) { return !seen.insert(fs.device).second; });
}

auto resolve_arg_mounts(const std::vector<std::filesystem::path>& arg_paths, const std::vector<MountEntry>& mounts)
    -> std::expected<std::vector<MountEntry>, std::string> {
    std::vector<MountEntry> matches;
    for (const auto& arg : arg_paths) {
        if (auto res = find_mount_for_path(mounts, arg); res.has_value()) {
            matches.push_back(std::move(*res));
        } else {
            return std::unexpected(arg.string());
        }
    }

    return matches;
}

}  // namespace df
