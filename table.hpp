#pragma once

#include <string>
#include <vector>

#include "types.hpp"

auto make_table(const std::vector<FileSystemInfo>& info) -> std::string;
