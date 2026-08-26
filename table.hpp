#pragma once

#include <string>
#include <vector>

#include "types.hpp"

namespace df {

auto make_table(const std::vector<FileSystemInfo>& info) -> std::string;

}  // namespace df
