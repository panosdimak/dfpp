#include "args.hpp"

namespace df {

auto read_arg_paths(std::span<char* const> args) -> std::expected<std::vector<std::filesystem::path>, ArgError> {
    std::vector<std::filesystem::path> paths;
    for (auto arg : args) {
        std::error_code ec;
        auto path = std::filesystem::canonical(arg, ec);
        if (ec) {
            return std::unexpected(ArgError{.arg{arg}, .ec{ec}});
        }
        paths.push_back(std::move(path));
    }

    return paths;
}

}  // namespace df
