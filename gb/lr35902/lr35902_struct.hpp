#pragma once

#include <cstdint>
#include <cstring>

#include "../structs.hpp"
#include "../macros.hpp"

namespace gb {
    // The LR35902 SoC on the Game Boy contains the CPU (Sharp SM83-like core)
    // the PSG or APU, the Video Generator or PPU, the Serial Controller or SC
    // The CPU drives the A, D, WR, RD and CS lines
    // The CPU and PPU drive the MA, MD, MWR, MRD and MCS lines
    // The APU drives the SO1 and SO2 lines
    // The SC drives the SCK, SIN and SOUT lines
    // The PPU drives the LD, CPG, CP, ST, CPL, FR and S lines

    struct lr35902_t {
        // This allows bus conflict analysis
        bool main_bus_set;
        bool vram_bus_set;

        struct pins_t {
            uint16_t a;     // Main bus Address     A0-A15
            uint8_t d;      // Main bus Data        D0-D7
            bool wr;        // Main bus Write       ~WR
            bool rd;        // Main bus Read        ~RD
            bool cs;        // Main bus Chip Select ~CS
            uint16_t ma;    // VRAM bus Address     MA0-MA12
            uint8_t md;     // VRAM bus Data        MD0-MD7
            bool mwr;       // VRAM bus Write       ~MWR
            bool mrd;       // VRAM bus Read        ~MRD
            bool mcs;       // VRAM bus Chip Select ~MCS
            bool ck[2];     // Clock                CK1-CK2
            bool phi;       // Divided clock        PHI
            uint8_t p1;     // Joypad pins          P10-P15
            float so[2];    // Audio channels       SO1-SO2
            float vin;      // Cartridge audio in   VIN
            bool sck;       // Serial clock         SCK
            bool sin;       // Serial in (RX)       SIN
            bool sout;      // Serial out (TX)      SOUT
            float res;      // Power?               ~RES
            bool ld[2];     // LCD Data             LD0-LD1
            bool cpg;       // LCD Control          CPG
            bool cp;        // LCD Clock            CP
            bool st;        // LCD Hsync            ST
            bool cpl;       // LCD Data Latch?      CPL
            bool fr;        // LCD ALTSIGL?         FR
            bool s;         // LCD Vsync            S
        } pins;

        cpu_t* cpu;
        ppu_t* ppu;
        apu_t* apu;
        sc_t* sc;
    };
}