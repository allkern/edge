#pragma once

#include "macros.hpp"
#include "structs.hpp"

// Hardware inside LR35902 SoC
#include "lr35902/lr35902_struct.hpp"
#include "lr35902/lr35902_funcs.hpp"
#include "lr35902/cpu_struct.hpp"
#include "lr35902/cpu_funcs.hpp"

// Hardware outside LR35902 SoC
#include "slot/slot_struct.hpp"
#include "slot/slot_funcs.hpp"
#include "lh5264/lh5264_struct.hpp"
#include "lh5264/lh5264_funcs.hpp"

namespace gb {
    struct gameboy_t {
        lr35902_t        soc;
        lh5264_t         wram;
        cpu_t            cpu;
        cartridge_slot_t slot;
    };

    void init(gameboy_t* gb) {
        // Init hardware
        lr35902_init(&gb->soc);
        lh5264_init(&gb->wram, &gb->soc);
        cpu_init(&gb->cpu, &gb->soc);
        slot_init(&gb->slot, &gb->soc);

        // Assign CPU to LR35902
        gb->soc.cpu = &gb->cpu;
    }

    void clock(gameboy_t* gb, int cycles = 1) {
        while (cycles--) {
            // This clocks all hardware inside LR35902 SoC
            lr35902_clock(&gb->soc);

            // We also have to clock/update hardware outside the SoC
            slot_clock(&gb->slot);
            lh5264_update(&gb->wram);
        }
    }
}