#pragma once

#include "../macros.hpp"
#include "../structs.hpp"

#include "../lr35902/lr35902_struct.hpp"

namespace gb {
    struct cartridge_slot_t {
        lr35902_t::pins_t* pins;
    };
}