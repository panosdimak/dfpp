#include "table.hpp"

#include <algorithm>
#include <array>
#include <format>
#include <functional>
#include <numeric>
#include <ranges>
#include <string_view>
#include <vector>

#include "format.hpp"
#include "types.hpp"

namespace df {

namespace {

struct FormattedInfo {
    std::string mounted_on;
    std::string total_size;
    std::string used_size;
    std::string avail_size;
    std::string pct;
    std::string type;
    std::string device;
    double usage_ratio;
};

auto make_border(
    std::string_view left, std::string_view fill, std::string_view sep, std::string_view right,
    const std::array<size_t, 7>& widths
) -> std::string {
    std::string border;

    auto repeated = [&](const size_t width) {
        return std::views::repeat(fill, width + 2) | std::views::join | std::ranges::to<std::string>();
    };

    border += left;
    for (size_t i = 0; i < widths.size(); i++) {
        border += repeated(widths[i]);
        if (i < widths.size() - 1) {
            border += sep;
        }
    }
    border += right;
    border += "\n";

    return border;
}

}  // namespace

auto make_table(const std::vector<FileSystemInfo>& info) -> std::string {
    std::vector<FormattedInfo> fmt_info;

    for (const auto& row : info) {
        FormattedInfo fmt_row;
        fmt_row.mounted_on = row.mounted_on.string();
        fmt_row.total_size = format_size(*row.total_bytes);
        fmt_row.used_size = format_size(*row.used_bytes);
        fmt_row.avail_size = format_size(*row.available_bytes);
        fmt_row.type = row.fs_type;
        fmt_row.device = row.device;

        fmt_row.usage_ratio = static_cast<double>(*row.used_bytes) / static_cast<double>(*row.total_bytes);
        fmt_row.pct = std::format("{:.1f}%", fmt_row.usage_ratio * 100);

        fmt_info.push_back(std::move(fmt_row));
    }

    constexpr std::array<std::string_view, 7> headers{"MOUNTED ON", "TOTAL", "USED", "AVAIL", "USE%", "TYPE", "DEVICE"};

    std::array<size_t, 7> max_fmt_sizes{};

    auto max_len = [](const auto& range, auto proj, std::string_view header) {
        return std::ranges::fold_left(range, header.size(), [proj](size_t acc, const auto& item) {
            return std::max(acc, std::invoke(proj, item).size());
        });
    };

    constexpr size_t bar_width = 12;

    max_fmt_sizes[0] = max_len(fmt_info, &FormattedInfo::mounted_on, headers[0]);
    max_fmt_sizes[1] = max_len(fmt_info, &FormattedInfo::total_size, headers[1]);
    max_fmt_sizes[2] = max_len(fmt_info, &FormattedInfo::used_size, headers[2]);
    max_fmt_sizes[3] = max_len(fmt_info, &FormattedInfo::avail_size, headers[3]);
    const size_t pct_field = max_len(fmt_info, &FormattedInfo::pct, headers[4]) + 1;  // text + separating space
    max_fmt_sizes[4] = pct_field + bar_width;
    max_fmt_sizes[5] = max_len(fmt_info, &FormattedInfo::type, headers[5]);
    max_fmt_sizes[6] = max_len(fmt_info, &FormattedInfo::device, headers[6]);

    std::string table;

    constexpr std::string_view RESET = "\033[0m";
    constexpr std::string_view BOLD_PALE_BLUE = "\033[1;38;5;153m";
    constexpr std::string_view DIM_WHITE = "\033[2;38;5;231m";
    constexpr std::string_view DIM_GRAY = "\033[38;5;240m";

    table += make_border("╭", "─", "─", "╮", max_fmt_sizes);

    auto inner_width = std::accumulate(max_fmt_sizes.begin(), max_fmt_sizes.end(), 0uz) + 3 * max_fmt_sizes.size() -
                       1;  // 2 spaces per column + N-1 separators

    auto label = std::format("{} local {}", info.size(), info.size() == 1 ? "device" : "devices");

    table += std::format("│ {:<{}} │\n", label, inner_width - 2);  // -2 for spaces

    table += make_border("├", "─", "┬", "┤", max_fmt_sizes);

    table += std::format(
        "│ {:<{}} │ {:>{}} │ {:>{}} │ {:>{}} │ {:^{}} │ {:<{}} │ {:<{}} │\n", headers[0], max_fmt_sizes[0], headers[1],
        max_fmt_sizes[1], headers[2], max_fmt_sizes[2], headers[3], max_fmt_sizes[3], headers[4], max_fmt_sizes[4],
        headers[5], max_fmt_sizes[5], headers[6], max_fmt_sizes[6]
    );

    table += make_border("├", "─", "┼", "┤", max_fmt_sizes);

    auto colorize = [&](std::string_view code, std::string_view text) {
        return std::string(code) + std::string(text) + std::string(RESET);
    };

    auto pad_left = [](std::string_view s, size_t w) { return std::format("{:>{}}", s, w); };

    auto pad_right = [](std::string_view s, size_t w) { return std::format("{:<{}}", s, w); };

    for (const auto& row : fmt_info) {
        auto mounted = colorize(BOLD_PALE_BLUE, pad_right(row.mounted_on, max_fmt_sizes[0]));
        auto total = colorize(DIM_WHITE, pad_left(row.total_size, max_fmt_sizes[1]));
        auto used = colorize(DIM_WHITE, pad_left(row.used_size, max_fmt_sizes[2]));
        auto avail = colorize(usage_ansi_code(row.usage_ratio), pad_left(row.avail_size, max_fmt_sizes[3]));
        auto type = colorize(DIM_WHITE, pad_right(row.type, max_fmt_sizes[5]));
        auto device = colorize(DIM_WHITE, pad_right(row.device, max_fmt_sizes[6]));

        auto bar_segments = percentage_bar(row.usage_ratio, bar_width);
        auto full = colorize(usage_ansi_code(row.usage_ratio), bar_segments[0]);
        auto empty = colorize(DIM_GRAY, bar_segments[1]);
        auto pct_text = std::format("{:>{}}", row.pct, pct_field);
        std::string pct_bar = full + empty + pct_text;

        table +=
            std::format("│ {} │ {} │ {} │ {} │ {} │ {} │ {} │\n", mounted, total, used, avail, pct_bar, type, device);
    }

    table += make_border("╰", "─", "┴", "╯", max_fmt_sizes);

    return table;
}

}  // namespace df
