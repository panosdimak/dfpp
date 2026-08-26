#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#include "types.hpp"

namespace df {

struct UsageBar {
    std::string filled;
    std::string empty;
};

auto format_size(Bytes bytes) -> std::string;

auto usage_ansi_code(double ratio) -> std::string_view;

auto percentage_bar(double ratio, std::size_t width) -> UsageBar;

}  // namespace df
