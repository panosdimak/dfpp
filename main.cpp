#include <cstddef>
#include <print>
#include <span>
#include <vector>

#include "args.hpp"
#include "fs_info.hpp"
#include "select.hpp"
#include "table.hpp"

auto main(int argc, char* argv[]) -> int {
    auto arg_paths = df::read_arg_paths(std::span(argv, static_cast<std::size_t>(argc)).subspan(1));
    if (!arg_paths) {
        std::println(stderr, "dfpp: {}: {}", arg_paths.error().arg, arg_paths.error().ec.message());
        return 1;
    }

    auto mount_entries = df::read_mounts("/proc/mounts");
    if (!mount_entries) {
        std::println(stderr, "dfpp: /proc/mounts: {}", mount_entries.error().message());
        return 1;
    }

    df::filter_real(*mount_entries);
    df::dedup_by_device(*mount_entries);

    if (!arg_paths->empty()) {
        auto matches = df::resolve_arg_mounts(*arg_paths, *mount_entries);
        if (!matches) {
            std::println(stderr, "dfpp: cannot find mount for {}", matches.error());
            return 1;
        }

        mount_entries = std::move(*matches);
        df::dedup_by_device(*mount_entries);
    }

    std::vector<df::FileSystemInfo> filesystems;
    filesystems.reserve(mount_entries->size());
    for (auto& entry : *mount_entries) {
        auto usage = df::get_fs_stats(entry.mounted_on);
        if (!usage) {
            std::println(stderr, "dfpp: {}: {}", entry.mounted_on.string(), usage.error().message());
            return 1;
        }
        filesystems.emplace_back(std::move(entry), *usage);
    }

    std::println("{}", df::make_table(filesystems));

    return 0;
}
