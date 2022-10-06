#pragma once

#include "../macros.hpp"
#include "../structs.hpp"

#include "slot_struct.hpp"

#include "../lr35902/lr35902_struct.hpp"

namespace gb {
    void slot_init(cartridge_slot_t* slot, lr35902_t* lr35902) {
        slot->pins = &lr35902->pins;
    }

    uint8_t rom[0xff] = {
        0x78, 0x48
    };

    void slot_clock(cartridge_slot_t* slot) {
        // To-do

        // Simulate ROM
        bool access = slot->pins->cs && !(slot->pins->a & 0x8000);

        if (access && RANGE(slot->pins->a, 0x0000, 0xff)) {
            slot->pins->d = rom[slot->pins->a];
        }
    }
}