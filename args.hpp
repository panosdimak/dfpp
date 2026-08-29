#pragma once

#include <expected>
#include <filesystem>
#include <span>
#include <string>
#include <system_error>
#include <vector>

namespace df {

struct ArgError {
    std::string arg;
    std::error_code ec;
};

// Canonicalize each command-line argument; fails on the first bad path
auto read_arg_paths(std::span<char* const> args) -> std::expected<std::vector<std::filesystem::path>, ArgError>;

}  // namespace df
