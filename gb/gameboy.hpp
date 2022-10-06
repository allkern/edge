#pragma once

#include "macros.hpp"
#include "structs.hpp"

// Hardware inside LR35902 SoC
#include "lr35902/lr35902_struct.hpp"
#include "lr35902/lr35902_funcs.hpp"
#include "lr35902/cpu_struct.hpp"
#include "lr35902/cpu_funcs.hpp"

// Hardware outside LR35902 SoC
#include "cartridge_slot/slot_struct.hpp"
#include "cartridge_slot/slot_funcs.hpp"
#include "lh5264an/lh5264an_struct.hpp"
#include "lh5264an/lh5264an_funcs.hpp"

namespace gb {
    struct gameboy_t {
        lr35902_t        lr35902;
        lh5264an_t       wram;
        cpu_t            cpu;
        cartridge_slot_t slot;
    };

    void init(gameboy_t* gb) {
        // Init hardware
        lr35902_init(&gb->lr35902);
        lh5264an_init(&gb->wram, &gb->lr35902);
        cpu_init(&gb->cpu, &gb->lr35902);
        slot_init(&gb->slot, &gb->lr35902);

        // Assign CPU to LR35902
        gb->lr35902.cpu = &gb->cpu;
    }

    void clock(gameboy_t* gb, int cycles = 1) {
        while (cycles--) {
            // This clocks all hardware inside LR35902 SoC
            lr35902_clock(&gb->lr35902);

            // We also have to clock hardware outside the SoC
            slot_clock(&gb->slot);

            lh5264an_update(&gb->wram);
        }
    }
}