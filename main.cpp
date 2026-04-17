#include <print>
#include <set>

#include "fs_info.hpp"
#include "table.hpp"

int main() {
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

        std::print("{}", make_table(mounts.value()));
    }
    else {
        std::println("{}", mounts.error());
    }

    return 0;
}
