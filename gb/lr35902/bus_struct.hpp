#pragma once

#include <cstdint>
#include <cstring>

#include "../structs.hpp"
#include "../macros.hpp"

namespace gb {
    struct bus_t {
        uint16_t a;     // Ext bus Address      A0-A15
        uint8_t d;      // Ext bus Data         D0-D7
        bool wr;        // Ext bus Write        /WR
        bool rd;        // Ext bus Read         /RD
        bool cs;        // Ext bus Chip Select  /CS
    };
}