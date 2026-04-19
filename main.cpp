#include <print>
#include <set>
#include <filesystem>
#include <vector>
#include <system_error>

#include "fs_info.hpp"
#include "table.hpp"

int main(int argc, char* argv[]) {
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

    auto mounts = read_mounts("/proc/mounts");
    if (mounts) {
        auto& entries = mounts.value();
        std::erase_if(entries, [](const auto& fs) { return !is_real_filesystem(fs); });

        std::set<std::string> seen;
        std::erase_if(entries, [&seen](const auto& fs) {
            return !seen.insert(fs.device).second;
        });

        for (auto& fs : entries)
            get_fs_stats(fs);

        if (!varg_paths.empty()) {
            std::vector<FileSystemInfo> matches;
            for (const auto& arg : varg_paths) {
                auto res = find_mount_for_path(entries, arg);
                matches.push_back(*res);
            }

            std::set<std::string> seen;
            std::erase_if(matches, [&seen](const auto& fs) {
               return !seen.insert(fs.device).second;
            });

            std::println("{}", make_table(matches));
        } else {
            std::print("{}", make_table(mounts.value()));
        }
    }
    else {
        std::println("{}", mounts.error());
    }

    return 0;
}
