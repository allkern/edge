#pragma once

#include "../macros.hpp"
#include "../structs.hpp"
#include "../log.hpp"

#include "lr35902_struct.hpp"

#include "cpu_instructions.hpp"
#include "cpu_struct.hpp"
#include "cpu_table.hpp"
#include "cpu_bus.hpp"

namespace gb {
    // Initializers
    void cpu_init(cpu_t* cpu, lr35902_t* lr35902) {
        std::memset(cpu, 0, sizeof(cpu_t));

        // Initialize bus to idle state
        cpu->bus.rd = true;
        cpu->bus.wr = true;
        cpu->bus.a = 0x8000;
        cpu->bus.cs = true;

        cpu->main_bus_set = &lr35902->main_bus_set;
        cpu->vram_bus_set = &lr35902->vram_bus_set;
        cpu->state = ST_FETCH;
    }

    bool cpu_bus_is_read(cpu_t* cpu) {
        return (!cpu->bus.rd) && cpu->bus.wr;
    }

    bool cpu_bus_is_write(cpu_t* cpu) {
        return (!cpu->bus.wr) && cpu->bus.rd;
    }

    void cpu_update_clocks(cpu_t* cpu) {
        cpu->ck_half_cycle++;
        cpu->ck_half_cycle %= 8;

        if (!(cpu->ck_half_cycle % 2)) {
            cpu->total_t_cycles++;
        }

        // cpu->bus.phi = !((cpu->ck_half_cycle >> 2) & 1);
    }

    void cpu_clock(cpu_t* cpu) {
        switch (cpu->state) {
            case ST_FETCH: {
                if (!cpu->read_ongoing) {
                    cpu_init_read(cpu, cpu->pc++);
                }

                if (!cpu_handle_read(cpu, &cpu->i_latch)) {
                    cpu->state = ST_EXECUTE;
                }
            } break;

            case ST_EXECUTE: {
                instruction_state_t state = instruction_table[cpu->i_latch](cpu);

                // Emulate prefetch
                if (state == IS_LAST_CYCLE) {
                    if (!cpu->read_ongoing) {
                        cpu_init_read(cpu, cpu->pc++);
                    }

                    if (!cpu_handle_read(cpu, &cpu->temp_i_latch)) {
                        cpu->i_latch = cpu->temp_i_latch;
                        cpu->ex_m_cycle = 0;
                        cpu->state = ST_EXECUTE;
                    }
                }
            } break;

            case ST_TEST: { /* CPU is externally controlled */ } break;
        }

        cpu_update_clocks(cpu);
    }
}