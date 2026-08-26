#include "format.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <format>
#include <ranges>
#include <string>

#include "types.hpp"

namespace df {

auto format_size(Bytes bytes) -> std::string {
    static constexpr std::array<std::string_view, 5> units{"B", "KiB", "MiB", "GiB", "TiB"};

    double size = static_cast<double>(bytes);
    std::size_t counter = 0;

    while (size >= 1024 && counter < (units.size() - 1)) {
        size = size / 1024;
        counter++;
    }

    const auto precision = (counter < 2) ? 0 : 1;

    return std::format("{:.{}f} {}", size, precision, units[counter]);
}

auto usage_ansi_code(double ratio) -> std::string_view {
    static constexpr std::string_view usage_low = "\033[38;5;193m";   // green
    static constexpr std::string_view usage_warn = "\033[38;5;223m";  // yellow
    static constexpr std::string_view usage_high = "\033[38;5;217m";  // red

    if (ratio >= 0.0 && ratio < 0.6) {
        return usage_low;
    } else if (ratio <= 0.8) {
        return usage_warn;
    } else {
        return usage_high;
    }
}

auto percentage_bar(double ratio, std::size_t width) -> std::array<std::string, 2> {
    assert(ratio >= 0.0 && ratio <= 1.0);

    std::array<std::string, 2> bar_segments;

    static constexpr std::string_view full_block = "█";
    static constexpr std::string_view half_block = "▌";
    static constexpr std::string_view empty_block = "░";

    const auto used = ratio * static_cast<double>(width);
    const auto full_size = static_cast<std::size_t>(used);
    const auto frac = used - static_cast<double>(full_size);
    const auto half = frac >= 0.5;
    const auto empty_size = width - full_size - (half ? 1 : 0);

    bar_segments[0] = std::views::repeat(full_block, full_size) | std::views::join | std::ranges::to<std::string>();

    if (half) {
        bar_segments[0] += half_block;
    }

    bar_segments[1] += std::views::repeat(empty_block, empty_size) | std::views::join | std::ranges::to<std::string>();

    return bar_segments;
}

}  // namespace df
