#include <cstddef>
#include <print>
#include <set>
#include <span>
#include <vector>

#include "args.hpp"
#include "fs_info.hpp"
#include "table.hpp"

auto main(int argc, char* argv[]) -> int {
    auto arg_paths = df::read_arg_paths(std::span(argv, static_cast<std::size_t>(argc)).subspan(1));
    if (!arg_paths) {
        std::println(stderr, "dfpp: {}: {}", arg_paths.error().arg, arg_paths.error().ec.message());
        return 1;
    }

    auto mounts = df::read_mounts("/proc/mounts");
    if (mounts) {
        auto& entries = mounts.value();
        std::erase_if(entries, [](const auto& fs) { return !df::is_real_filesystem(fs); });

        std::set<std::string> seen;
        std::erase_if(entries, [&seen](const auto& fs) { return !seen.insert(fs.device).second; });

        for (auto& fs : entries) {
            df::get_fs_stats(fs);
        }

        if (!arg_paths->empty()) {
            std::vector<df::FileSystemInfo> matches;
            for (const auto& arg : *arg_paths) {
                if (auto res = df::find_mount_for_path(entries, arg); res.has_value()) {
                    matches.push_back(std::move(*res));
                } else {
                    std::println(stderr, "dfpp: cannot find mount for {}", arg.string());
                    return 1;
                }
            }

            std::set<std::string> args_seen;
            std::erase_if(matches, [&args_seen](const auto& fs) { return !args_seen.insert(fs.device).second; });

            std::println("{}", df::make_table(matches));
        } else {
            std::println("{}", df::make_table(mounts.value()));
        }
    } else {
        std::println(stderr, "{}", mounts.error());
        return 1;
    }

    return 0;
}
