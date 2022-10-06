#pragma once

#include "../macros.hpp"
#include "../structs.hpp"

#include "lh52a64n_struct.hpp"

#include "../lr35902/lr35902_struct.hpp"

namespace gb {
    void lh52a64n_init(lh52a64n_t* lh52a64n, lr35902_t* lr35902) {
        lh52a64n->pins = &lr35902->pins;

        // Allocate 8KB
        lh52a64n->memory = new uint8_t[0x2000];

        // Fill with test pattern (55 aa)
        for (int i = 0; i < 0x2000; i++) {
            lh52a64n->memory[i] = 0x55 << (i & 0x1);
        }
    }

    // Scheme breaking name:
    // This is called lh52a64n_update because the lh52a64n
    // chip doesn't require an input clock signal.
    // Data is served on-demand

    void lh52a64n_update(lh52a64n_t* lh52a64n) {
        // Table derived straight from Sharp's datasheet:
        // ___           __    __
        // CE1    CE2    WE    OE    Mode      D0-D7
        //  H      X     X     X     Standby   High-Z
        //  X      L     X     X     Standby   High-Z
        //  L      H     L     X     Write     Input
        //  L      H     H     L     Read      Output
        //  L      H     H     H     No Output High-Z
        
        bool we = lh52a64n->pins->wr;
        bool oe = lh52a64n->pins->rd;
        bool ce1 = lh52a64n->pins->cs;

        // Connected to A14
        bool ce2 = lh52a64n->pins->a & 0x4000;

        // A0-A12
        uint16_t addr = lh52a64n->pins->a & 0x1fff;

        // Non-standby mode
        if (ce2 && !ce1) {
            // Write mode
            if (!we) {
                lh52a64n->memory[addr] = lh52a64n->pins->d;
            } else {
                // Read mode
                if (!oe) {
                    lh52a64n->pins->d = lh52a64n->memory[addr];
                } // Else no output
            }
        } // Else standby
    }
}