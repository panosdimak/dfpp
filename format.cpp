#include <string>

#include "types.hpp"

constexpr std::array<std::string, 5> units {"B", "KiB", "MiB", "GiB", "TiB"};

std::string format_size(Bytes bytes) {
    double size = static_cast<double>(bytes);
    int counter = 0;

    while (size >= 1024 && counter < static_cast<int>(units.size() - 1)) {
        size = size / 1024;
        counter++;
    }

    int precision = (counter < 2) ? 0 : 1;

    return std::format("{:.{}f} {}", size, precision, units[counter]);
}

std::string_view usage_ansi_code(double ratio) {
    if (ratio >= 0.0 && ratio < 0.6)
        return "\033[38;5;193m";    // green
    else if (ratio <= 0.8)
        return "\033[38;5;223m";    // orange
    else
        return "\033[38;5;217m";    // red-pink
}
