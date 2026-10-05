/// @file TicoConfig.h
/// @brief Minimal hardcoded configuration for tico overlay (fceumm)
#pragma once

#include <cstring>
#include <string>

namespace TicoConfig {
    constexpr const char* TEST_ROM = "sdmc:/tico/roms/nes/rom.nes";

    constexpr const char* FONT_PATH = "romfs:/fonts/font.ttf";

    // Current console slug (nes, from argv[1])
    inline std::string CURRENT_SLUG = "nes";

    /// @brief Set the console being booted (nes)
    inline void SetSlug(const std::string& slug) {
        if (!slug.empty())
            CURRENT_SLUG = slug;
    }

    /// Content directories, with a trailing slash. Tico's per-module Paths tab
    /// stores custom roots as tico_{system,saves,states}_path in fceumm.jsonc;
    /// empty or missing keys fall back to sdmc:/tico/<kind>/. Like tico's own
    /// {saves}/{states}/{system}, the console slug is appended to the root.
    std::string SystemPath();
    std::string SavesPath();
    std::string StatesPath();

    /// Create a directory and any missing parents.
    void MakeDirs(const std::string& path);

    /// @brief RetroAchievements console ID for a loaded image: Famicom Disk
    /// System disks (fwNES "FDS\x1A" header, or a raw disk starting with its
    /// "*NINTENDO-HVC*" block) are their own console, the rest is the NES.
    inline int GetRcConsoleId(const unsigned char* data, size_t size) {
        const bool fds = data && size >= 15 &&
            (!memcmp(data, "FDS\x1A", 4) || (data[0] == 0x01 && !memcmp(data + 1, "*NINTENDO-HVC*", 14)));
        return fds ? 81 /* RC_CONSOLE_FAMICOM_DISK_SYSTEM */ : 7 /* RC_CONSOLE_NINTENDO */;
    }

    constexpr int WINDOW_WIDTH = 1280;
    constexpr int WINDOW_HEIGHT = 720;
    constexpr float FONT_SIZE = 32.0f;

    /// @brief Use callback/ring-buffer path (supports resampling)
    constexpr bool USE_SDLQUEUEAUDIO = false;
}

/// @brief UI action identifiers for the helpers bar
enum UIActions {
    ACTION_CONFIRM,
    ACTION_BACK,
    ACTION_DETAILS,
    ACTION_MENU,
    ACTION_EDIT,
    ACTION_DELETE
};
