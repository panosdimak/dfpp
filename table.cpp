#include <vector>
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

   table += make_border("╭", "─", "┬", "╮", max_fmt_sizes);

   table += std::format("│ {:<{}} │ {:>{}} │ {:>{}} │ {:>{}} │ {:<{}} │ {:<{}} │\n",
       "MOUNTED ON", max_fmt_sizes[0], "TOTAL", max_fmt_sizes[1], "USED", max_fmt_sizes[2],
       "AVAIL", max_fmt_sizes[3], "TYPE", max_fmt_sizes[4], "DEVICE", max_fmt_sizes[5]);

   table += make_border("├", "─", "┼", "┤", max_fmt_sizes);

   for (const auto& row : fmt_info) {
       table += std::format("│ {:<{}} │ {:>{}} │ {:>{}} │ {:>{}} │ {:<{}} │ {:<{}} │\n",
           row.mounted_on, max_fmt_sizes[0], row.total_size, max_fmt_sizes[1], row.used_size, max_fmt_sizes[2],
           row.avail_size, max_fmt_sizes[3], row.type, max_fmt_sizes[4], row.device, max_fmt_sizes[5]);
   }

   table += make_border("╰", "─", "┴", "╯", max_fmt_sizes);

   return table;
}
