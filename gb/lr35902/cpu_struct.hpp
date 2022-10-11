#pragma once

#include "../macros.hpp"
#include "../structs.hpp"

#include "lr35902_struct.hpp"
#include "cpu_defines.hpp"

#include <cstdint>

namespace gb {
    struct cpu_t {
        lr35902_t::pins_t* pins;
        bool* main_bus_set;
        bool* vram_bus_set;

        bool read_ongoing;
        bool write_ongoing;
        bool idle_cycle;
        bool exec_ongoing;
        bool ime;

        // Address and data bus latches
        uint16_t a_latch;
        uint8_t d_latch;

        // Main registers
        uint8_t r[8];
        uint16_t pc;
        uint16_t sp;

        // Latches for encoded operands
        uint8_t x_latch, y_latch;

        // Which execute stage cycle are we on
        uint8_t ex_m_cycle;

        // Total instruction execute cycles
        uint8_t ex_cycles_cnt;

        // Current clock half cycle
        uint8_t ck_half_cycle;

        // Current opcode being executed
        uint8_t temp_i_latch;
        uint8_t i_latch;

        // Data fetch latches
        uint8_t l_latch, h_latch;

        uint8_t state = ST_FETCH;

        uint64_t total_t_cycles;
    };
}