#pragma once

#include "../macros.hpp"
#include "../structs.hpp"

#include "../lr35902/lr35902_struct.hpp"

namespace gb {
    struct cartridge_slot_t {
        bus_t* pins; // External bus
    };
}