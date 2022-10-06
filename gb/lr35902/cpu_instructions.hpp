#pragma once

#include "../macros.hpp"
#include "../structs.hpp"

#include "cpu_struct.hpp"
#include "cpu_defines.hpp"

namespace gb {
    instruction_state_t ld_r_r(cpu_t* cpu) {
        // 01xxxyyy
        cpu->x_latch = (cpu->instruction_latch >> 3) & 0x7;
        cpu->y_latch = (cpu->instruction_latch >> 0) & 0x7;

        cpu->r[cpu->x_latch] = cpu->r[cpu->y_latch];

        return IS_LAST_CYCLE;
    }
}