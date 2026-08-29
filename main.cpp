#include <cstddef>
#include <print>
#include <set>
#include <span>
#include <vector>

#include "args.hpp"
#include "fs_info.hpp"
#include "table.hpp"

namespace {

auto dedup_by_device(std::vector<df::FileSystemInfo>& mounts) -> void {
    std::set<std::string> seen;
    std::erase_if(mounts, [&seen](const auto& fs) { return !seen.insert(fs.device).second; });
}

auto resolve_arg_mounts(
    const std::vector<std::filesystem::path>& arg_paths, const std::vector<df::FileSystemInfo>& mounts
) -> std::expected<std::vector<df::FileSystemInfo>, std::string> {
    std::vector<df::FileSystemInfo> matches;
    for (const auto& arg : arg_paths) {
        if (auto res = df::find_mount_for_path(mounts, arg); res.has_value()) {
            matches.push_back(std::move(*res));
        } else {
            return std::unexpected(arg.string());
        }
    }

    return matches;
}

}  // namespace

auto main(int argc, char* argv[]) -> int {
    auto arg_paths = df::read_arg_paths(std::span(argv, static_cast<std::size_t>(argc)).subspan(1));
    if (!arg_paths) {
        std::println(stderr, "dfpp: {}: {}", arg_paths.error().arg, arg_paths.error().ec.message());
        return 1;
    }

    auto mounts = df::read_mounts("/proc/mounts");
    if (!mounts) {
        std::println(stderr, "dfpp: {}", mounts.error());
        return 1;
    }

    std::erase_if(*mounts, [](const auto& fs) { return !df::is_real_filesystem(fs); });

    dedup_by_device(*mounts);

    for (auto& fs : *mounts) {
        df::get_fs_stats(fs);
    }

    if (!arg_paths->empty()) {
        auto matches = resolve_arg_mounts(*arg_paths, *mounts);
        if (!matches) {
            std::println(stderr, "dfpp: cannot find mount for {}", matches.error());
            return 1;
        }

        mounts = std::move(*matches);
        dedup_by_device(*mounts);
    }

    std::println("{}", df::make_table(*mounts));

    return 0;
}
