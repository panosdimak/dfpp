#include <cstddef>
#include <print>
#include <span>

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

    auto mounts = df::read_mounts("/proc/mounts");
    if (!mounts) {
        std::println(stderr, "dfpp: {}", mounts.error());
        return 1;
    }

    df::filter_real(*mounts);

    df::dedup_by_device(*mounts);

    if (!arg_paths->empty()) {
        auto matches = df::resolve_arg_mounts(*arg_paths, *mounts);
        if (!matches) {
            std::println(stderr, "dfpp: cannot find mount for {}", matches.error());
            return 1;
        }

        mounts = std::move(*matches);
        df::dedup_by_device(*mounts);
    }

    for (auto& fs : *mounts) {
        df::get_fs_stats(fs);
    }

    std::println("{}", df::make_table(*mounts));

    return 0;
}
