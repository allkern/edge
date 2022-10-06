#pragma once

#include "../macros.hpp"
#include "../structs.hpp"
#include "../log.hpp"

#include "lr35902_struct.hpp"

#include "cpu_struct.hpp"
#include "cpu_instructions.hpp"
#include "cpu_table.hpp"

namespace gb {
    // Initializers
    void cpu_init(cpu_t* cpu, lr35902_t* lr35902) {
        std::memset(cpu, 0, sizeof(cpu_t));

        cpu->pins = &lr35902->pins;

        // Initialize bus to idle state
        cpu->pins->a = 0x8000;
        cpu->pins->cs = true;

        cpu->main_bus_set = &lr35902->main_bus_set;
        cpu->vram_bus_set = &lr35902->vram_bus_set;
        cpu->state = ST_FETCH;
    }

    // Utilities

    // For reads to all regions:
    // Address is put onto A0-A14 on ck=1
    // RD is pulled low on ck=0
    // WR is pulled high on ck=0
    // ---
    // For reads to 0000-7fff:
    // A15 is pulled high on ck=0 then pulled low on ck=2
    // CS is pulled high on ck=0
    // ---
    // For reads to a000-fdff
    // A15 is pulled high on ck=0
    // CS is pulled high on ck=0 then pulled low on ck=2
    // ---
    // For reads to fe00-ffff
    // Both A15 and CS are pulled high on ck=0
    void cpu_init_read(cpu_t* cpu, uint16_t addr) {
        cpu->read_ongoing = true;
        cpu->a_latch = addr;
    }

    void cpu_update_clocks(cpu_t* cpu) {
        cpu->ck_half_cycle++;
        cpu->ck_half_cycle %= 8;

        cpu->pins->phi = !((cpu->ck_half_cycle >> 2) & 1);
    }

    bool cpu_handle_read(cpu_t* cpu, uint8_t* dest) {
        switch (cpu->ck_half_cycle) {
            case 0: {
                cpu->pins->wr = true;
                cpu->pins->rd = false;

                // Pull A15 and CS high
                cpu->pins->a |= 0x8000;
                cpu->pins->cs = true;
            } break;

            case 1: {
                // Keep A15
                cpu->pins->a &= 0x8000;

                // Latch address into A lines
                cpu->pins->a |= cpu->a_latch & 0x7fff;
            } break;

            case 2: {
                if (RANGE(cpu->a_latch, 0x0000, 0x7fff)) {
                    cpu->pins->a &= 0x7fff;
                } else if (RANGE(cpu->a_latch, 0xa000, 0xfdff)) {
                    cpu->pins->cs = false;
                }
            } break;

            // Latch data pins into destination
            case 6: {
                *dest = cpu->pins->d;
            } break;

            case 7: {
                cpu->read_ongoing = false;

                return false;
            } break;

            default: {
                // CPU doesn't change signals, waiting for data
            } break;
        }

        return true;
    }

    void cpu_clock(cpu_t* cpu) {
        switch (cpu->state) {
            case ST_FETCH: {
                if (!cpu->read_ongoing) {
                    cpu_init_read(cpu, cpu->pc++);
                }

                if (!cpu_handle_read(cpu, &cpu->instruction_latch)) {
                    cpu->state = ST_EXECUTE;
                }
            } break;

            case ST_EXECUTE: {
                instruction_state_t state = instruction_table[cpu->instruction_latch](cpu);

                // Emulate prefetch
                if (state == IS_LAST_CYCLE) {
                    if (!cpu->read_ongoing) {
                        cpu_init_read(cpu, cpu->pc++);
                    }

                    if (!cpu_handle_read(cpu, &cpu->instruction_latch)) {
                        cpu->state = ST_EXECUTE;
                    }
                }
            } break;
        }

        cpu_update_clocks(cpu);
    }
}