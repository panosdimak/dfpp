#pragma once

#include <array>
#include <string>

#include "types.hpp"

auto format_size(Bytes bytes) -> std::string;

auto usage_ansi_code(double ratio) -> std::string_view;

auto percentage_bar(double ratio, size_t width) -> std::array<std::string, 2>;
