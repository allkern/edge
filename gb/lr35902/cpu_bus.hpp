#pragma once

#include "../macros.hpp"
#include "../structs.hpp"
#include "../log.hpp"

#include "lr35902_struct.hpp"

#include "cpu_struct.hpp"

namespace gb {
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

    void cpu_init_write(cpu_t* cpu, uint16_t addr, uint8_t data) {
        cpu->write_ongoing = true;
        cpu->a_latch = addr;
        cpu->d_latch = data;
    }

    bool cpu_handle_write(cpu_t* cpu) {
        bool rom = RANGE(cpu->a_latch, 0x0000, 0x7fff);
        bool ram = RANGE(cpu->a_latch, 0xa000, 0xfdff);

        switch (cpu->ck_half_cycle) {
            case 0: {
                cpu->pins->wr = true;
                cpu->pins->rd = false;

                // Pull A15 and CS high
                cpu->pins->a |= 0x8000;
                cpu->pins->cs = true;

            } break;

            case 1: {
                if (RANGE(cpu->a_latch, 0x0000, 0x7fff) || RANGE(cpu->a_latch, 0xa000, 0xfdff)) {
                    cpu->pins->rd = true;
                }

                // Keep A15
                cpu->pins->a &= 0x8000;

                // Latch address into A lines
                cpu->pins->a |= cpu->a_latch & 0x7fff;
            } break;

            case 2: {
                if (RANGE(cpu->a_latch, 0x0000, 0x7fff)) {
                    // A15 pulled low
                    cpu->pins->a &= 0x7fff;
                } else if (RANGE(cpu->a_latch, 0xa000, 0xfdff)) {
                    cpu->pins->cs = false;
                }
            } break;

            case 3: {
                if (RANGE(cpu->a_latch, 0x0000, 0x7fff) || RANGE(cpu->a_latch, 0xa000, 0xfdff)) {
                    // WR goes low
                    cpu->pins->wr = false;

                    // and data is latched into D lines
                    cpu->pins->d = cpu->d_latch;
                }
            } break;

            case 6: {
                // WR is pulled high
                cpu->pins->wr = true;
            } break;

            case 7: {
                cpu->write_ongoing = false;

                return false;
            }

            default: {
                // CPU doesn't change signals
            } break;
        }

        return true;
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
}