#include <filesystem>
#include <print>
#include <set>
#include <system_error>
#include <vector>

#include "fs_info.hpp"
#include "table.hpp"

auto main(int argc, char* argv[]) -> int {
    std::vector<std::filesystem::path> varg_paths;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            std::error_code ec;
            auto path = std::filesystem::canonical(argv[i], ec);
            if (ec) {
                std::println(stderr, "dfpp: {}: {}", argv[i], ec.message());
                return 1;
            }
            varg_paths.push_back(path);
        }
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

        if (!varg_paths.empty()) {
            std::vector<df::FileSystemInfo> matches;
            for (const auto& arg : varg_paths) {
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
