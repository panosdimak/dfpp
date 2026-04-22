#pragma once

#include <string>
#include <array>

#include "types.hpp"

std::string format_size(Bytes bytes);

std::string_view usage_ansi_code(double ratio);

std::array<std::string, 2> percentage_bar(double ratio, size_t width);
