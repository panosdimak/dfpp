#pragma once

#include <string>

#include "types.hpp"

std::string format_size(Bytes bytes);

std::string_view usage_ansi_code(double ratio);
