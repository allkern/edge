#pragma once

#include "../macros.hpp"
#include "../structs.hpp"

#include "../lr35902/lr35902_struct.hpp"

namespace gb {
    // WRAM is allegedly a Sharp LH5264 (or similar) chip.
    // I wasn't able to find an exact datasheet match
    // for this particular chip.
    // This is the closest I could find:
    // https://pdf1.alldatasheet.com/datasheet-pdf/view/42972/SHARP/LH5164A.html

    struct lh5264_t {
        lr35902_t::pins_t* pins;

        bool prev_we;
        bool prev_oe;

        uint8_t* memory;
    };
}