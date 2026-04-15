#include <print>

#include "fs_info.hpp"
#include "types.hpp"

int main() {
    // Test parse_mount_line
    auto test_line = "/dev/sda1 / ext4 rw,relatime 0 0";
    if (auto info = parse_mount_line(test_line)) {
        std::println("Parsed successfully");
        std::println("  Device: {}", info->device);
        std::println("  Mount:  {}", info->mounted_on.string());
        std::println("  Type:   {}", info->fs_type);
    } else {
        std::println("Failed to parse test line");
    }

    std::println("\n--- Reading /proc/mounts ---");
    auto mounts = read_mounts("/proc/mounts");
    if (mounts) {
        std::println("Found {} mount(s)", mounts.value().size());

        for (auto& fs : mounts.value()) {
            std::println("{} on {} type {}", fs.device, fs.mounted_on.string(), fs.fs_type);

            get_fs_stats(fs);
            if (fs.total_bytes)
                std::println("{}", *fs.total_bytes);
        }
    }
    else {
        std::println("{}", mounts.error());
    }


    return 0;
}
