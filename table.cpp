#include <vector>
#include <numeric>
#include <functional>
#include <algorithm>
#include <ranges>

#include "types.hpp"
#include "format.hpp"

struct FormattedInfo {
    std::string mounted_on;
    std::string total_size;
    std::string used_size;
    std::string avail_size;
    std::string type;
    std::string device;
    double usage_ratio;
};

std::string make_border(std::string_view left, std::string_view fill, std::string_view sep, std::string_view right,
                        const std::array<size_t, 6>& widths) {
    std::string border;

    auto repeated = [&](const size_t width) {
        return std::views::repeat(fill, width + 2)
            | std::views::join
            | std::ranges::to<std::string>();
    };

    border += left;
    for (size_t i = 0; i < widths.size(); i++) {
        border += repeated(widths[i]);
        if (i < widths.size() - 1)
            border += sep;
    }
    border += right;
    border += "\n";

    return border;
}

std::string make_table(const std::vector<FileSystemInfo>& info) {
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

       fmt_info.push_back(std::move(fmt_row));
   }

   std::array<size_t, 6> max_fmt_sizes{};

   auto max_len =[](const auto& range, auto proj) {
       return std::ranges::max(
           range | std::views::transform([&](const auto& x) {
               return std::invoke(proj, x).size(); })
       );
   };

   max_fmt_sizes[0] = std::max(max_len(fmt_info, &FormattedInfo::mounted_on), std::string_view("MOUNTED_ON").size());
   max_fmt_sizes[1] = std::max(max_len(fmt_info, &FormattedInfo::total_size), std::string_view("TOTAL").size());
   max_fmt_sizes[2] = std::max(max_len(fmt_info, &FormattedInfo::used_size), std::string_view("USED").size());
   max_fmt_sizes[3] = std::max(max_len(fmt_info, &FormattedInfo::avail_size), std::string_view("AVAIL").size());
   max_fmt_sizes[4] = std::max(max_len(fmt_info, &FormattedInfo::type), std::string_view("TYPE").size());
   max_fmt_sizes[5] = std::max(max_len(fmt_info, &FormattedInfo::device), std::string_view("DEVICE").size());

   std::string table;

   constexpr std::string_view RESET             = "\033[0m";
   constexpr std::string_view BOLD_PALE_BLUE    = "\033[1;38;5;153m";
   constexpr std::string_view DIM_WHITE         = "\033[2;38;5;231m";

   table += make_border("╭", "─", "─", "╮", max_fmt_sizes);

   table += std::format("│ {} local {:<{}} │\n", info.size(),
                        info.size() == 1 ? "device" : "devices",
                        std::accumulate(max_fmt_sizes.begin(), max_fmt_sizes.end(), 0) + 7);

   table += make_border("├", "─", "┬", "┤", max_fmt_sizes);

   table += std::format("│ {:<{}} │ {:>{}} │ {:>{}} │ {:>{}} │ {:<{}} │ {:<{}} │\n",
       "MOUNTED ON", max_fmt_sizes[0], "TOTAL", max_fmt_sizes[1], "USED", max_fmt_sizes[2],
       "AVAIL", max_fmt_sizes[3], "TYPE", max_fmt_sizes[4], "DEVICE", max_fmt_sizes[5]);

   table += make_border("├", "─", "┼", "┤", max_fmt_sizes);

   auto colorize = [&](std::string_view code, std::string_view text) {
       return std::string(code) + std::string(text) + std::string(RESET);
   };

   auto pad_left = [](std::string_view s, size_t w) {
       return std::format("{:>{}}", s, w);
   };

   auto pad_right = [](std::string_view s, size_t w) {
       return std::format("{:<{}}", s, w);
   };

   for (const auto& row : fmt_info) {
       auto mounted  = colorize(BOLD_PALE_BLUE, pad_right(row.mounted_on, max_fmt_sizes[0]));
       auto total    = colorize(DIM_WHITE,  pad_left(row.total_size,  max_fmt_sizes[1]));
       auto used     = colorize(DIM_WHITE,  pad_left(row.used_size,   max_fmt_sizes[2]));
       auto avail    = colorize(usage_ansi_code(row.usage_ratio), pad_left(row.avail_size, max_fmt_sizes[3]));
       auto type     = colorize(DIM_WHITE,  pad_right(row.type,       max_fmt_sizes[4]));
       auto device   = colorize(DIM_WHITE,  pad_right(row.device,     max_fmt_sizes[5]));

       table += std::format("│ {} │ {} │ {} │ {} │ {} │ {} │\n",
           mounted, total, used, avail, type, device);
   }

   table += make_border("╰", "─", "┴", "╯", max_fmt_sizes);

   return table;
}
