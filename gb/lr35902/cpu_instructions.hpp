#pragma once

#include "../macros.hpp"
#include "../structs.hpp"

#include "cpu_defines.hpp"
#include "cpu_struct.hpp"
#include "cpu_bus.hpp"

#include <cassert>

// Use (void) to silence unused warnings.
#define assertm(exp, msg) assert(((void)msg, exp))

namespace gb {
    instruction_state_t nop(cpu_t* cpu) {
        return IS_LAST_CYCLE;
    }

    instruction_state_t ld_r_r(cpu_t* cpu) {
        // 01xxxyyy
        cpu->x_latch = (cpu->i_latch >> 3) & 0x7;
        cpu->y_latch = (cpu->i_latch >> 0) & 0x7;

        cpu->r[cpu->x_latch] = cpu->r[cpu->y_latch];

        return IS_LAST_CYCLE;
    }

    instruction_state_t ld_r_n(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            case 0: {
                if (!cpu->read_ongoing) {
                    cpu_init_read(cpu, cpu->pc++);
                }

                if (!cpu_handle_read(cpu, &cpu->l_latch)) {
                    cpu->ex_m_cycle++;
                }

                return IS_EXECUTING;
            } break;

            case 1: {
                cpu->x_latch = (cpu->i_latch >> 3) & 0x7;

                cpu->r[cpu->x_latch] = cpu->l_latch;

                return IS_LAST_CYCLE;
            } break;

            default: {
                _log(error, "Invalid M cycle %u while executing %s", cpu->ex_m_cycle, __FUNCTION__);

                std::exit(1);
            } break;
        }

        return IS_DONE;
    }

    instruction_state_t ld_r_hl(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            case 0: {
                if (!cpu->read_ongoing) {
                    cpu_init_read(cpu, ((uint16_t)cpu->r[4] << 8) | cpu->r[5]);
                }

                if (!cpu_handle_read(cpu, &cpu->l_latch)) {
                    cpu->ex_m_cycle++;
                }

                return IS_EXECUTING;
            } break;

            case 1: {
                cpu->x_latch = (cpu->i_latch >> 3) & 0x7;

                cpu->r[cpu->x_latch] = cpu->l_latch;

                return IS_LAST_CYCLE;
            } break;

            default: {
                _log(error, "Invalid M cycle %u while executing %s", cpu->ex_m_cycle, __FUNCTION__);

                std::exit(1);
            } break;
        }

        return IS_DONE;
    }

    instruction_state_t ld_hl_r(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            case 0: {
                if (!cpu->write_ongoing) {
                    cpu->x_latch = (cpu->i_latch >> 3) & 0x7;

                    cpu_init_write(cpu, ((uint16_t)cpu->r[4] << 8) | cpu->r[5], cpu->r[cpu->x_latch]);
                }

                if (!cpu_handle_write(cpu)) {
                    cpu->ex_m_cycle++;
                }

                return IS_EXECUTING;
            } break;

            case 1: {
                return IS_LAST_CYCLE;
            } break;

            default: {
                _log(error, "Invalid M cycle %u while executing %s", cpu->ex_m_cycle, __FUNCTION__);

                std::exit(1);
            } break;
        }

        return IS_DONE;
    }

    instruction_state_t ld_hl_n(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            case 0: {
                if (!cpu->read_ongoing) {
                    cpu_init_read(cpu, cpu->pc++);
                }

                if (!cpu_handle_read(cpu, &cpu->l_latch)) {
                    cpu->ex_m_cycle++;
                }

                return IS_EXECUTING;
            } break;

            case 1: {
                if (!cpu->write_ongoing) {
                    cpu_init_write(cpu, ((uint16_t)cpu->r[4] << 8) | cpu->r[5], cpu->l_latch);
                }

                if (!cpu_handle_write(cpu)) {
                    cpu->ex_m_cycle++;
                }

                return IS_EXECUTING;
            } break;

            case 2: {
                return IS_LAST_CYCLE;
            } break;

            default: {
                _log(error, "Invalid M cycle %u while executing %s", cpu->ex_m_cycle, __FUNCTION__);

                std::exit(1);
            } break;
        }

        return IS_DONE;
    }
}