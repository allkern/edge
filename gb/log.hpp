#pragma once

#include <iostream>
#include <fstream>
#include <cstdio>

#define _ESCAPE_BRACKET "["
#define _ESCAPE_M       "m"

#ifdef _WIN32
#define _ESCAPE_SEQ     "\x1b"
#elif __linux__
#define _ESCAPE_SEQ     "\x1b"
#endif

#define ESCAPE(seq) _ESCAPE_SEQ "[" #seq "m"

#define LBLACK   ESCAPE(30)
#define LRED     ESCAPE(31)
#define LGREEN   ESCAPE(32)
#define LYELLOW  ESCAPE(33)
#define LBLUE    ESCAPE(34)
#define LMAGENTA ESCAPE(35)
#define LCYAN    ESCAPE(36)
#define LWHITE   ESCAPE(37)
#define LBLACK   ESCAPE(30)
#define HRED     ESCAPE(31;1)
#define HGREEN   ESCAPE(32;1)
#define HYELLOW  ESCAPE(33;1)
#define HBLUE    ESCAPE(34;1)
#define HMAGENTA ESCAPE(35;1)
#define HCYAN    ESCAPE(36;1)
#define HWHITE   ESCAPE(37;1)

namespace _log {
    const char* colors_low[] = {
        ESCAPE(30), // Black
        ESCAPE(31), // Red
        ESCAPE(32), // Green
        ESCAPE(33), // Yellow
        ESCAPE(34), // Blue
        ESCAPE(35), // Magenta
        ESCAPE(36), // Cyan
        ESCAPE(37)  // White
    };
    
    const char* colors_high[] = {
        ESCAPE(30;1),
        ESCAPE(31;1),
        ESCAPE(32;1),
        ESCAPE(33;1),
        ESCAPE(34;1),
        ESCAPE(35;1),
        ESCAPE(36;1),
        ESCAPE(37;1)
    };

    enum color_t {
        BLACK,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE
    };

    unsigned int color_indexes[] = {
        WHITE, BLUE, GREEN, CYAN, MAGENTA, RED 
    };

    enum type_t {
        none, debug, ok, info, warning, error
    };

    enum type_mask_t {
        mask_none    = 0b00000001,
        mask_debug   = 0b00000010,
        mask_ok      = 0b00000100,
        mask_info    = 0b00001000,
        mask_warning = 0b00010000,
        mask_error   = 0b00100000,
        mask_all     = 0b00111111
    };

    const char* type_text[] = {
        "none",
        "debug",
        "ok",
        "info",
        "warning",
        "error"
    };

    bool disable_logs = false;

    namespace settings {
        bool disable_escape = false;
        bool bright_colors = true;
        type_mask_t mask = mask_all;
        std::string app_name;
        std::ofstream file;
    }

    bool is_allowed(int type) {
        switch (type) {
            case none   : return settings::mask & mask_none;
            case debug  : return settings::mask & mask_debug;
            case ok     : return settings::mask & mask_ok;
            case info   : return settings::mask & mask_info;
            case warning: return settings::mask & mask_warning;
            case error  : return settings::mask & mask_error;
        }

        return false;
    }

    void disable() {
        disable_logs = true;
    }

    void enable() {
        disable_logs = false;
    }

    template <class... Args> void log(int type, std::string text, Args... args) {
        if (disable_logs) return;
        if (!is_allowed(type)) return;

        static char buf[0x400];

        std::sprintf(buf, text.c_str(), args...);

        const char** cols = settings::bright_colors ? colors_high : colors_low;

        if (settings::disable_escape) {
            std::cout << settings::app_name << ": " << type_text[type] << ": " << buf << std::endl;
        } else {
            std::cout << settings::app_name << ": " << cols[color_indexes[type]] << type_text[type] << ": " << ESCAPE(0) << buf << std::endl;
        }

        if (settings::file.is_open()) {
            settings::file << settings::app_name << ": " << type_text[type] << ": " << buf << std::endl;
        }
    }

    void init(std::string app_name, std::string file_name = "", bool bright = true, bool no_escape = false) {
        _log::settings::app_name = app_name;

        if (file_name.size())
            _log::settings::file.open(file_name);

        _log::settings::bright_colors = bright;
        _log::settings::disable_escape = no_escape;
    }
}

#define _log(t, ...) _log::log(_log::t, __VA_ARGS__)