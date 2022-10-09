#pragma once

#include "../macros.hpp"
#include "../structs.hpp"

#include "lh5264_struct.hpp"

#include "../lr35902/lr35902_struct.hpp"

namespace gb {
    void lh5264_init(lh5264_t* lh5264, lr35902_t* lr35902) {
        lh5264->pins = &lr35902->pins;

        // Allocate 8KB
        lh5264->memory = new uint8_t[0x2000];

        // Fill with test pattern (55 aa)
        for (int i = 0; i < 0x2000; i++) {
            lh5264->memory[i] = 0x55 << (i & 0x1);
        }
    }

    // Scheme breaking name:
    // This is called lh5264_update because the LH5264
    // chip doesn't require an input clock signal.
    // Data is served on-demand

    void lh5264_update(lh5264_t* lh5264) {
        // Table derived straight from Sharp's datasheet:
        // ___           __    __
        // CE1    CE2    WE    OE    Mode      D0-D7
        //  H      X     X     X     Standby   High-Z
        //  X      L     X     X     Standby   High-Z
        //  L      H     L     X     Write     Input
        //  L      H     H     L     Read      Output
        //  L      H     H     H     No Output High-Z
        
        bool we = lh5264->pins->wr;
        bool oe = lh5264->pins->rd;
        bool ce1 = lh5264->pins->cs;

        // Connected to A14
        bool ce2 = lh5264->pins->a & 0x4000;

        // A0-A12
        uint16_t addr = lh5264->pins->a & 0x1fff;

        // Non-standby mode
        if (ce2 && !ce1) {
            // Write mode
            if (!we) {
                lh5264->memory[addr] = lh5264->pins->d;
            } else {
                // Read mode
                if (!oe) {
                    lh5264->pins->d = lh5264->memory[addr];
                } // Else no output
            }
        } // Else standby
    }
}