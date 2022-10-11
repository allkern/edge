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

#define ZF 0b10000000
#define NF 0b01000000
#define HF 0b00100000
#define CF 0b00010000

#define INVALID_M \
    default: { \
        _log(error, "Invalid M cycle %u while executing %s", cpu->ex_m_cycle, __FUNCTION__); \
        std::exit(1); \
    } break;

namespace gb {
    inline void dec_hl(cpu_t* cpu) {
        uint16_t hl = HL - 1;

        H = (hl >> 8) & 0xff;
        L = (hl >> 0) & 0xff;
    }

    inline void inc_hl(cpu_t* cpu) {
        uint16_t hl = HL + 1;

        H = (hl >> 8) & 0xff;
        L = (hl >> 0) & 0xff;
    }

    inline void set_bc(cpu_t* cpu, uint16_t value) {
        B = (value >> 8) & 0xff;
        C = (value >> 0) & 0xff;
    }

    inline void set_de(cpu_t* cpu, uint16_t value) {
        D = (value >> 8) & 0xff;
        E = (value >> 0) & 0xff;
    }

    inline void set_hl(cpu_t* cpu, uint16_t value) {
        H = (value >> 8) & 0xff;
        L = (value >> 0) & 0xff;
    }

    inline void set_af(cpu_t* cpu, uint16_t value) {
        A = (value >> 8) & 0xff;
        F = (value >> 0) & 0xf0;
    }

    inline void set_sp(cpu_t* cpu, uint16_t value) {
        cpu->sp = value;
    }

    inline uint16_t get_hl(cpu_t* cpu) {
        return HL;
    }

    inline uint16_t get_bc(cpu_t* cpu) {
        return BC;
    }

    inline uint16_t get_de(cpu_t* cpu) {
        return DE;
    }

    inline uint16_t get_sp(cpu_t* cpu) {
        return cpu->sp;
    }

    inline uint16_t get_af(cpu_t* cpu) {
        return AF;
    }

    typedef void (*set16_fn_t)(cpu_t*, uint16_t);
    typedef uint16_t (*get16_fn_t)(cpu_t*);

    set16_fn_t set16[] = {
        set_bc, set_de, set_hl, set_sp
    };

    get16_fn_t get16[] = {
        get_bc, get_de, get_hl, get_sp
    };
    
    get16_fn_t get16_push[] = {
        get_bc, get_de, get_hl, get_af
    };

    inline bool check_condition(cpu_t* cpu, uint8_t cc) {
        switch (cc) {
            case 0: return !(cpu->r[6] & ZF);
            case 1: return  (cpu->r[6] & ZF);
            case 2: return !(cpu->r[6] & CF);
            case 3: return  (cpu->r[6] & CF);
        }

        return false;
    }

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

    instruction_state_t ld_nn_a(cpu_t* cpu) {
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
                if (!cpu->write_ongoing) {
                    cpu_init_write(cpu, NN, A);
                }

                if (!cpu_handle_write(cpu)) {
                    cpu->ex_m_cycle++;
                }

                return IS_EXECUTING;
            } break;

            case 3: {
                return IS_LAST_CYCLE;
            } break;

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ldh_a_c(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            case 0: {
                if (!cpu->read_ongoing) {
                    cpu_init_read(cpu, 0xff00 | C);
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

    instruction_state_t ldh_c_a(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            case 0: {
                if (!cpu->write_ongoing) {
                    cpu_init_write(cpu, 0xff00 | C, A);
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

    instruction_state_t ldh_a_n(cpu_t* cpu) {
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
                    cpu_init_read(cpu, 0xff00 | cpu->l_latch);
                }

                if (!cpu_handle_read(cpu, &cpu->l_latch)) {
                    cpu->ex_m_cycle++;
                }

                return IS_EXECUTING;
            } break;

            case 2: {
                A = cpu->l_latch;

                return IS_LAST_CYCLE;
            } break;

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ldh_n_a(cpu_t* cpu) {
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
                    cpu_init_write(cpu, 0xff00 | cpu->l_latch, A);
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

    instruction_state_t ld_a_hld(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            case 0: {
                if (!cpu->read_ongoing) {
                    cpu_init_read(cpu, HL);

                    dec_hl(cpu);
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

    instruction_state_t ld_hld_a(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            case 0: {
                if (!cpu->write_ongoing) {
                    cpu_init_write(cpu, HL, A);

                    dec_hl(cpu);
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

    instruction_state_t ld_a_hli(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            case 0: {
                if (!cpu->read_ongoing) {
                    cpu_init_read(cpu, HL);

                    inc_hl(cpu);
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

#define WRITE(cycle, addr, data, init, fini) \
    case cycle: { \
        if (!cpu->write_ongoing) { \
            cpu_init_write(cpu, addr, data); \
            init; \
        } \
        if (!cpu_handle_write(cpu)) { \
            cpu->ex_m_cycle++; \
            fini;  \
        } \
        return IS_EXECUTING; \
    } break; \

#define READ(cycle, addr, dest, init, fini) \
    case cycle: { \
        if (!cpu->read_ongoing) { \
            cpu_init_read(cpu, addr); \
            init; \
        } \
        if (!cpu_handle_read(cpu, dest)) { \
            cpu->ex_m_cycle++; \
            fini;  \
        } \
        return IS_EXECUTING; \
    } break; \

#define IDLE(cycle, init, fini) \
    case cycle: { \
        if (!cpu->idle_cycle) { \
            cpu_init_idle(cpu); \
            init; \
        } \
        if (!cpu_handle_idle(cpu)) { \
            cpu->ex_m_cycle++; \
            fini;  \
        } \
        return IS_EXECUTING; \
    } break; \

#define LAST(cycle, exec) \
    case cycle: { \
        exec; \
        return IS_LAST_CYCLE; \
    } break;

    instruction_state_t ld_hli_a(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            case 0: {
                if (!cpu->write_ongoing) {
                    cpu_init_write(cpu, HL, A);

                    inc_hl(cpu);
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

    instruction_state_t ld_rr_nn(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            READ(1, cpu->pc++, &cpu->h_latch, , );

            case 2: {
                set16[(cpu->i_latch >> 4) & 0x3](cpu, NN);

                return IS_LAST_CYCLE;
            } break;

            INVALID_M;
        }

        return IS_DONE;
    }
    
    instruction_state_t ld_nn_sp(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            READ(1, cpu->pc++, &cpu->h_latch, , );
            WRITE(2, NN, cpu->sp & 0xff, , );
            WRITE(3, NN + 1, (cpu->sp >> 8) & 0xff, , );
            LAST(4, );

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_sp_hl(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            IDLE(0, , set_sp(cpu, HL));
            LAST(1, );

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t push_bc(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            // push rr's first cycle is bus idle
            // but SP is latched onto the address bus
            IDLE(0, cpu->pins->a |= cpu->sp & 0x7fff, );
            WRITE(1, --cpu->sp, B, , );
            WRITE(2, --cpu->sp, C, , );
            LAST(3, );

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t push_de(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            // push rr's first cycle is bus idle
            // but SP is latched onto the address bus
            IDLE(0, cpu->pins->a |= cpu->sp & 0x7fff, );
            WRITE(1, --cpu->sp, D, , );
            WRITE(2, --cpu->sp, E, , );
            LAST(3, );

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t push_hl(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            // push rr's first cycle is bus idle
            // but SP is latched onto the address bus
            IDLE(0, cpu->pins->a |= cpu->sp & 0x7fff, );
            WRITE(1, --cpu->sp, H, , );
            WRITE(2, --cpu->sp, L, , );
            LAST(3, );

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t push_af(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            // push rr's first cycle is bus idle
            // but SP is latched onto the address bus
            IDLE(0, cpu->pins->a |= cpu->sp & 0x7fff, );
            WRITE(1, --cpu->sp, A, , );
            WRITE(2, --cpu->sp, F, , );
            LAST(3, );

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t pop_bc(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->sp++, &cpu->l_latch, , );
            READ(1, cpu->sp++, &cpu->h_latch, , );
            LAST(2, set_bc(cpu, NN));

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t pop_de(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->sp++, &cpu->l_latch, , );
            READ(1, cpu->sp++, &cpu->h_latch, , );
            LAST(2, set_de(cpu, NN));

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t pop_hl(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->sp++, &cpu->l_latch, , );
            READ(1, cpu->sp++, &cpu->h_latch, , );
            LAST(2, set_hl(cpu, NN));

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t pop_af(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->sp++, &cpu->l_latch, , );
            READ(1, cpu->sp++, &cpu->h_latch, , );
            LAST(2, set_af(cpu, NN));

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t jp_nn(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            READ(1, cpu->pc++, &cpu->h_latch, , );
            IDLE(2, , );
            LAST(3, cpu->pc = NN);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t jp_hl(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            LAST(0, cpu->pc = HL);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t jp_cc_nn(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            READ(1, cpu->pc++, &cpu->h_latch, , );
            
            case 2: {
                if (check_condition(cpu, (cpu->i_latch >> 3) & 0x3)) {
                    if (!cpu->idle_cycle) {
                        cpu_init_idle(cpu);
                    }

                    if (!cpu_handle_idle(cpu)) {
                        cpu->ex_m_cycle++;
                    }

                    return IS_EXECUTING;
                } else {
                    return IS_LAST_CYCLE;
                }
            } break;

            LAST(3, cpu->pc = NN);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t jr_n(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            IDLE(1, , );
            LAST(2, cpu->pc += (int8_t)cpu->l_latch);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t jr_cc_n(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            
            case 1: {
                if (check_condition(cpu, (cpu->i_latch >> 3) & 0x3)) {
                    if (!cpu->idle_cycle) {
                        cpu_init_idle(cpu);
                    }

                    if (!cpu_handle_idle(cpu)) {
                        cpu->ex_m_cycle++;
                    }

                    return IS_EXECUTING;
                } else {
                    return IS_LAST_CYCLE;
                }
            } break;

            LAST(2, cpu->pc += (int8_t)cpu->l_latch);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t call_nn(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            READ(1, cpu->pc++, &cpu->h_latch, , );

            // Basically the same as push rr, but with PC
            IDLE(2, cpu->pins->a |= cpu->sp & 0x7fff, );
            WRITE(3, --cpu->sp, (cpu->pc >> 8) & 0xff, , );
            WRITE(4, --cpu->sp, (cpu->pc >> 0) & 0xff, , );
            LAST(5, cpu->pc = NN);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t call_cc_nn(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            READ(1, cpu->pc++, &cpu->h_latch, , );

            case 2: {
                if (check_condition(cpu, (cpu->i_latch >> 3) & 0x3)) {
                    if (!cpu->idle_cycle) {
                        cpu_init_idle(cpu);

                        // Put SP on the bus while bus-idling
                        cpu->pins->a |= cpu->sp & 0x7fff;
                    }

                    if (!cpu_handle_idle(cpu)) {
                        cpu->ex_m_cycle++;
                    }

                    return IS_EXECUTING;
                } else {
                    return IS_LAST_CYCLE;
                }
            } break;

            WRITE(3, --cpu->sp, (cpu->pc >> 8) & 0xff, , );
            WRITE(4, --cpu->sp, (cpu->pc >> 0) & 0xff, , );
            LAST(5, cpu->pc = NN);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ret(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->sp++, &cpu->l_latch, , );
            READ(1, cpu->sp++, &cpu->h_latch, , );
            IDLE(2, , );
            LAST(3, cpu->pc = NN);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ret_cc(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            IDLE(0, , );

            case 1: {
                if (check_condition(cpu, (cpu->i_latch >> 3) & 0x3)) {
                    if (!cpu->read_ongoing) {
                        cpu_init_read(cpu, cpu->sp++);
                    }

                    if (!cpu_handle_read(cpu, &cpu->l_latch)) {
                        cpu->ex_m_cycle++; \
                    }
                    return IS_EXECUTING; \
                } else {
                    return IS_LAST_CYCLE;
                }
            } break;

            READ(2, cpu->sp++, &cpu->h_latch, , );
            IDLE(3, , );
            LAST(4, cpu->pc = NN);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t reti(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->sp++, &cpu->l_latch, , );
            READ(1, cpu->sp++, &cpu->h_latch, , );
            IDLE(2, , cpu->ime = true);
            LAST(3, cpu->pc = NN);

            INVALID_M;
        }

        return IS_DONE;
    }
    
    instruction_state_t rst_n(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            READ(1, cpu->pc++, &cpu->h_latch, , );

            // Basically the same as push rr, but with PC
            IDLE(2, cpu->pins->a |= cpu->sp & 0x7fff, );
            WRITE(3, --cpu->sp, (cpu->pc >> 8) & 0xff, , );
            WRITE(4, --cpu->sp, (cpu->pc >> 0) & 0xff, , );
            LAST(5, cpu->pc = cpu->i_latch & 0x38);

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

#undef READ
#undef WRITE
#undef IDLE

#undef ZF
#undef NF
#undef HF
#undef CF

#undef INVALID_M