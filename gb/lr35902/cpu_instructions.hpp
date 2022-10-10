#pragma once

#include "../macros.hpp"
#include "../structs.hpp"

#include "cpu_defines.hpp"
#include "cpu_struct.hpp"
#include "cpu_bus.hpp"

#include <cassert>

// Use (void) to silence unused warnings.
#define assertm(exp, msg) assert(((void)msg, exp))

#define A cpu->r[7]
#define B cpu->r[0]
#define C cpu->r[1]
#define D cpu->r[2]
#define E cpu->r[3]
#define F cpu->r[6]
#define H cpu->r[4]
#define L cpu->r[5]
#define AF (((uint16_t)A << 8) | F)
#define BC (((uint16_t)B << 8) | C)
#define DE (((uint16_t)D << 8) | E)
#define HL (((uint16_t)H << 8) | L)
#define NN (((uint16_t)cpu->h_latch << 8) | cpu->l_latch)

#define INVALID_M \
    default: { \
        _log(error, "Invalid M cycle %u while executing %s", cpu->ex_m_cycle, __FUNCTION__); \
        std::exit(1); \
    } break;

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

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_r_hl(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            case 0: {
                if (!cpu->read_ongoing) {
                    cpu_init_read(cpu, HL);
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

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_hl_r(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            case 0: {
                if (!cpu->write_ongoing) {
                    cpu->x_latch = (cpu->i_latch >> 3) & 0x7;

                    cpu_init_write(cpu, HL, cpu->r[cpu->x_latch]);
                }

                if (!cpu_handle_write(cpu)) {
                    cpu->ex_m_cycle++;
                }

                return IS_EXECUTING;
            } break;

            case 1: {
                return IS_LAST_CYCLE;
            } break;

            INVALID_M;
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
                    cpu_init_write(cpu, HL, cpu->l_latch);
                }

                if (!cpu_handle_write(cpu)) {
                    cpu->ex_m_cycle++;
                }

                return IS_EXECUTING;
            } break;

            case 2: {
                return IS_LAST_CYCLE;
            } break;

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_a_bc(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            case 0: {
                if (!cpu->read_ongoing) {
                    cpu_init_read(cpu, BC);
                }

                if (!cpu_handle_read(cpu, &cpu->l_latch)) {
                    cpu->ex_m_cycle++;
                }

                return IS_EXECUTING;
            } break;

            case 1: {
                A = cpu->l_latch;

                return IS_LAST_CYCLE;
            } break;

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_a_de(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            case 0: {
                if (!cpu->read_ongoing) {
                    cpu_init_read(cpu, DE);
                }

                if (!cpu_handle_read(cpu, &cpu->l_latch)) {
                    cpu->ex_m_cycle++;
                }

                return IS_EXECUTING;
            } break;

            case 1: {
                A = cpu->l_latch;

                return IS_LAST_CYCLE;
            } break;

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_bc_a(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            case 0: {
                if (!cpu->write_ongoing) {
                    cpu_init_write(cpu, BC, A);
                }

                if (!cpu_handle_write(cpu)) {
                    cpu->ex_m_cycle++;
                }

                return IS_EXECUTING;
            } break;

            case 1: {
                return IS_LAST_CYCLE;
            } break;

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_de_a(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            case 0: {
                if (!cpu->write_ongoing) {
                    cpu_init_write(cpu, DE, A);
                }

                if (!cpu_handle_write(cpu)) {
                    cpu->ex_m_cycle++;
                }

                return IS_EXECUTING;
            } break;

            case 1: {
                return IS_LAST_CYCLE;
            } break;

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_a_nn(cpu_t* cpu) {
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
                if (!cpu->read_ongoing) {
                    cpu_init_read(cpu, cpu->pc++);
                }

                if (!cpu_handle_read(cpu, &cpu->h_latch)) {
                    cpu->ex_m_cycle++;
                }

                return IS_EXECUTING;
            } break;

            case 2: {
                if (!cpu->read_ongoing) {
                    cpu_init_read(cpu, NN);
                }

                if (!cpu_handle_read(cpu, &cpu->l_latch)) {
                    cpu->ex_m_cycle++;
                }

                return IS_EXECUTING;
            } break;

            case 3: {
                A = cpu->l_latch;

                return IS_LAST_CYCLE;
            } break;

            INVALID_M;
        }

        return IS_DONE;
    }
}

#undef A
#undef B
#undef C
#undef D
#undef E
#undef F
#undef H
#undef L
#undef AF
#undef BC
#undef DE
#undef HL
#undef NN

#undef INVALID_M