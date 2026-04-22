#include <array>
#include <string>
#include <ranges>

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

std::array<std::string, 2> percentage_bar(double ratio, size_t width) {
	std::array<std::string, 2> bar_segments;

	constexpr std::string_view full_block = "█";
	constexpr std::string_view half_block = "▌";
	constexpr std::string_view empty_block = "░";

	double used = ratio * width;
	size_t full_size = static_cast<size_t>(used);
	bool half = (used - full_size) >= 0.5;
	size_t empty_size = width - full_size - (half ? 1 : 0);

	bar_segments[0] = std::views::repeat(full_block, full_size)
		| std::views::join
		| std::ranges::to<std::string>();

	if (half) bar_segments[0] += half_block;

	bar_segments[1] += std::views::repeat(empty_block, empty_size)
		| std::views::join
		| std::ranges::to<std::string>();

	return bar_segments;
}
