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
#define X cpu->r[cpu->x_latch]
#define Y cpu->r[cpu->y_latch]
#define AF (((uint16_t)A << 8) | F)
#define BC (((uint16_t)B << 8) | C)
#define DE (((uint16_t)D << 8) | E)
#define HL (((uint16_t)H << 8) | L)
#define NN (((uint16_t)cpu->h_latch << 8) | cpu->l_latch)
#define SET_FLAGS(f) { F |= f; }
#define CLEAR_FLAGS(f) { F &= ~f; }

#define ZF 0b10000000
#define NF 0b01000000
#define HF 0b00100000
#define CF 0b00010000

#define INVALID_M \
    default: { \
        _log(error, "Invalid M cycle %u while executing %s", cpu->ex_m_cycle, __FUNCTION__); \
        std::exit(1); \
    } break;

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
        if (!cpu->read_ongoing) { \
            exec; \
        } \
        return IS_LAST_CYCLE; \
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

        X = Y;

        return IS_LAST_CYCLE;
    }

    instruction_state_t ld_r_n(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            LAST(1, cpu->x_latch = (cpu->i_latch >> 3) & 0x7; X = cpu->l_latch;);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_r_hl(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, HL, &cpu->l_latch, , );
            LAST(1, cpu->x_latch = (cpu->i_latch >> 3) & 0x7; X = cpu->l_latch;);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_hl_r(cpu_t* cpu) {
        cpu->x_latch = (cpu->i_latch >> 3) & 0x7;

        switch (cpu->ex_m_cycle) {
            WRITE(0, HL, X, , );
            LAST(1, );

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_hl_n(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            WRITE(1, HL, cpu->l_latch, , );
            LAST(2, );

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_a_bc(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, BC, &cpu->l_latch, , );
            LAST(1, A = cpu->l_latch);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_a_de(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, DE, &cpu->l_latch, , );
            LAST(1, A = cpu->l_latch);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_bc_a(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            WRITE(0, BC, A, , );
            LAST(1, );

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_de_a(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            WRITE(0, DE, A, , );
            LAST(1, );

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_a_nn(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            READ(1, cpu->pc++, &cpu->h_latch, , );
            READ(2, NN, &cpu->l_latch, , );
            LAST(3, A = cpu->l_latch);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_nn_a(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            READ(1, cpu->pc++, &cpu->h_latch, , );
            WRITE(2, NN, A, , );
            LAST(3, );

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ldh_a_c(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, 0xff00 | C, &cpu->l_latch, , );
            LAST(1, A = cpu->l_latch);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ldh_c_a(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            WRITE(0, 0xff00 | C, A, , );
            LAST(1, );

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ldh_a_n(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            READ(1, 0xff00 | cpu->l_latch, &cpu->l_latch, , );
            LAST(2, A = cpu->l_latch);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ldh_n_a(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            WRITE(1, 0xff00 | cpu->l_latch, A, , );
            LAST(2, );

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_a_hld(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, HL, &cpu->l_latch, dec_hl(cpu), );
            LAST(1, A = cpu->l_latch);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_hld_a(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            WRITE(0, HL, A, dec_hl(cpu), );
            LAST(1, );

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_a_hli(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, HL, &cpu->l_latch, inc_hl(cpu), );
            LAST(1, A = cpu->l_latch);

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t ld_hli_a(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            WRITE(0, HL, A, inc_hl(cpu), );
            LAST(1, );

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
            IDLE(0, cpu->bus.a |= cpu->sp & 0x7fff, );
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
            IDLE(0, cpu->bus.a |= cpu->sp & 0x7fff, );
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
            IDLE(0, cpu->bus.a |= cpu->sp & 0x7fff, );
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
            IDLE(0, cpu->bus.a |= cpu->sp & 0x7fff, );
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
            IDLE(2, cpu->bus.a |= cpu->sp & 0x7fff, );
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
                        cpu->bus.a |= cpu->sp & 0x7fff;
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
                        cpu->ex_m_cycle++;
                    }

                    return IS_EXECUTING;
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
            // Basically the same as push rr, but with PC
            IDLE(0, cpu->bus.a |= cpu->sp & 0x7fff, );
            WRITE(1, --cpu->sp, (cpu->pc >> 8) & 0xff, , );
            WRITE(2, --cpu->sp, (cpu->pc >> 0) & 0xff, , );
            LAST(3, cpu->pc = cpu->i_latch & 0x38);

            INVALID_M;
        }

        return IS_DONE;
    }

    // ALU

    void add8(cpu_t* cpu, uint8_t* dest, uint8_t src, bool carry) {
        cpu->alu_r_latch = *dest;
        cpu->alu_r_latch += src;

        if (carry && (F & CF)) cpu->alu_r_latch++; 

        CLEAR_FLAGS(NF);

        if (  cpu->alu_r_latch > 0xff ) SET_FLAGS(CF) else CLEAR_FLAGS(CF);
        if (!(cpu->alu_r_latch & 0xff)) SET_FLAGS(ZF) else CLEAR_FLAGS(ZF);
        if (((*dest & 0xf) + (src & 0xf)) & 0xf0) SET_FLAGS(HF) else CLEAR_FLAGS(HF);

        *dest = cpu->alu_r_latch & 0xff;
    }

    void sub8(cpu_t* cpu, uint8_t* dest, uint8_t src, bool carry) {
        cpu->alu_r_latch = *dest;
        cpu->alu_r_latch -= src;

        if (carry && (F & CF)) cpu->alu_r_latch--; 

        SET_FLAGS(NF);

        if (  cpu->alu_r_latch > 0xff ) SET_FLAGS(CF) else CLEAR_FLAGS(CF);
        if (!(cpu->alu_r_latch & 0xff)) SET_FLAGS(ZF) else CLEAR_FLAGS(ZF);
        //if (((*dest & 0xf) + (src & 0xf)) & 0xf0) SET_FLAGS(HF) else CLEAR_FLAGS(HF);

        *dest = cpu->alu_r_latch & 0xff;
    }
    
    void and8(cpu_t* cpu, uint8_t* dest, uint8_t src) {
        cpu->alu_r_latch = (*dest) & src;

        SET_FLAGS(HF);
        CLEAR_FLAGS(NF | CF);

        if (!(cpu->alu_r_latch & 0xff)) SET_FLAGS(ZF) else CLEAR_FLAGS(ZF);

        *dest = cpu->alu_r_latch;
    }
    
    void xor8(cpu_t* cpu, uint8_t* dest, uint8_t src) {
        cpu->alu_r_latch = (*dest) ^ src;

        CLEAR_FLAGS(NF | CF | HF);

        if (!(cpu->alu_r_latch & 0xff)) SET_FLAGS(ZF) else CLEAR_FLAGS(ZF);

        *dest = cpu->alu_r_latch;
    }
    
    void or8(cpu_t* cpu, uint8_t* dest, uint8_t src) {
        cpu->alu_r_latch = (*dest) | src;

        CLEAR_FLAGS(NF | CF | HF);

        if (!(cpu->alu_r_latch & 0xff)) SET_FLAGS(ZF) else CLEAR_FLAGS(ZF);

        *dest = cpu->alu_r_latch;
    }

    void cp8(cpu_t* cpu, uint8_t* dest, uint8_t src) {
        cpu->alu_r_latch = *dest;
        cpu->alu_r_latch -= src;

        SET_FLAGS(NF);

        if (  cpu->alu_r_latch > 0xff ) SET_FLAGS(CF) else CLEAR_FLAGS(CF);
        if (!(cpu->alu_r_latch & 0xff)) SET_FLAGS(ZF) else CLEAR_FLAGS(ZF);
        //if (((*dest & 0xf) + (src & 0xf)) & 0xf0) SET_FLAGS(HF) else CLEAR_FLAGS(HF);

        // *dest = cpu->alu_r_latch & 0xff;
    }

    instruction_state_t add_a_r(cpu_t* cpu) {
        cpu->y_latch = (cpu->i_latch >> 0) & 0x7;

        add8(cpu, &A, Y, cpu->i_latch & 0x8);

        return IS_LAST_CYCLE;
    }

    instruction_state_t add_a_n(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            LAST(1, add8(cpu, &A, cpu->l_latch, cpu->i_latch & 0x8));

            INVALID_M;
        }

        return IS_DONE;    
    }
    
    instruction_state_t add_a_hl(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, HL, &cpu->l_latch, , );
            LAST(1, add8(cpu, &A, cpu->l_latch, cpu->i_latch & 0x8));

            INVALID_M;
        }

        return IS_DONE;    
    }

    instruction_state_t sub_a_r(cpu_t* cpu) {
        cpu->y_latch = (cpu->i_latch >> 0) & 0x7;

        sub8(cpu, &A, Y, cpu->i_latch & 0x8);

        return IS_LAST_CYCLE;
    }

    instruction_state_t sub_a_n(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            LAST(1, sub8(cpu, &A, cpu->l_latch, cpu->i_latch & 0x8));

            INVALID_M;
        }

        return IS_DONE;    
    }
    
    instruction_state_t sub_a_hl(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, HL, &cpu->l_latch, , );
            LAST(1, sub8(cpu, &A, cpu->l_latch, cpu->i_latch & 0x8));

            INVALID_M;
        }

        return IS_DONE;    
    }

    instruction_state_t and_a_r(cpu_t* cpu) {
        cpu->y_latch = (cpu->i_latch >> 0) & 0x7;

        and8(cpu, &A, Y);

        return IS_LAST_CYCLE;
    }

    instruction_state_t and_a_n(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            LAST(1, and8(cpu, &A, cpu->l_latch));

            INVALID_M;
        }

        return IS_DONE;
    }
    
    instruction_state_t and_a_hl(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, HL, &cpu->l_latch, , );
            LAST(1, and8(cpu, &A, cpu->l_latch));

            INVALID_M;
        }

        return IS_DONE;    
    }

    instruction_state_t xor_a_r(cpu_t* cpu) {
        cpu->y_latch = (cpu->i_latch >> 0) & 0x7;

        xor8(cpu, &A, Y);

        return IS_LAST_CYCLE;
    }

    instruction_state_t xor_a_n(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            LAST(1, xor8(cpu, &A, cpu->l_latch));

            INVALID_M;
        }

        return IS_DONE;
    }
    
    instruction_state_t xor_a_hl(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, HL, &cpu->l_latch, , );
            LAST(1, xor8(cpu, &A, cpu->l_latch));

            INVALID_M;
        }

        return IS_DONE;    
    }

    instruction_state_t or_a_r(cpu_t* cpu) {
        cpu->y_latch = (cpu->i_latch >> 0) & 0x7;

        or8(cpu, &A, Y);

        return IS_LAST_CYCLE;
    }

    instruction_state_t or_a_n(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            LAST(1, or8(cpu, &A, cpu->l_latch));

            INVALID_M;
        }

        return IS_DONE;
    }
    
    instruction_state_t or_a_hl(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, HL, &cpu->l_latch, , );
            LAST(1, or8(cpu, &A, cpu->l_latch));

            INVALID_M;
        }

        return IS_DONE;    
    }

    instruction_state_t cp_a_r(cpu_t* cpu) {
        cpu->y_latch = (cpu->i_latch >> 0) & 0x7;

        cp8(cpu, &A, Y);

        return IS_LAST_CYCLE;
    }

    instruction_state_t cp_a_n(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, cpu->pc++, &cpu->l_latch, , );
            LAST(1, cp8(cpu, &A, cpu->l_latch));

            INVALID_M;
        }

        return IS_DONE;
    }
    
    instruction_state_t cp_a_hl(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, HL, &cpu->l_latch, , );
            LAST(1, cp8(cpu, &A, cpu->l_latch));

            INVALID_M;
        }

        return IS_DONE;    
    }

    instruction_state_t inc_r(cpu_t* cpu) {
        cpu->x_latch = (cpu->i_latch >> 3) & 0x7;

        cpu->r[cpu->x_latch]++;

        CLEAR_FLAGS(NF);

        if (!cpu->r[cpu->x_latch]) SET_FLAGS(ZF) else CLEAR_FLAGS(ZF);
        if (((cpu->r[cpu->x_latch] & 0xf) + 1) & 0xf0) SET_FLAGS(HF) else CLEAR_FLAGS(HF);

        return IS_LAST_CYCLE;
    }

    instruction_state_t inc_dhl(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, HL, &cpu->l_latch, , );
            WRITE(1, HL, ++cpu->l_latch, , );
            LAST(2,
                CLEAR_FLAGS(NF);

                if (!cpu->r[cpu->x_latch]) SET_FLAGS(ZF) else CLEAR_FLAGS(ZF);
                if (((cpu->r[cpu->x_latch] & 0xf) + 1) & 0xf0) SET_FLAGS(HF) else CLEAR_FLAGS(HF);
            );

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t dec_r(cpu_t* cpu) {
        cpu->x_latch = (cpu->i_latch >> 3) & 0x7;

        cpu->r[cpu->x_latch]--;

        CLEAR_FLAGS(NF);

        if (!cpu->r[cpu->x_latch]) SET_FLAGS(ZF) else CLEAR_FLAGS(ZF);
        if (((cpu->r[cpu->x_latch] & 0xf) + 1) & 0xf0) SET_FLAGS(HF) else CLEAR_FLAGS(HF);

        return IS_LAST_CYCLE;
    }

    instruction_state_t dec_dhl(cpu_t* cpu) {
        switch (cpu->ex_m_cycle) {
            READ(0, HL, &cpu->l_latch, , );
            WRITE(1, HL, --cpu->l_latch, , );
            LAST(2,
                CLEAR_FLAGS(NF);

                if (!cpu->r[cpu->x_latch]) SET_FLAGS(ZF) else CLEAR_FLAGS(ZF);
                if (((cpu->r[cpu->x_latch] & 0xf) + 1) & 0xf0) SET_FLAGS(HF) else CLEAR_FLAGS(HF);
            );

            INVALID_M;
        }

        return IS_DONE;
    }

    instruction_state_t cpl_a(cpu_t* cpu) {
        A ^= 0xff;

        return IS_LAST_CYCLE;
    }

    instruction_state_t scf(cpu_t* cpu) {
        SET_FLAGS(CF);

        return IS_LAST_CYCLE;
    }
    
    instruction_state_t ccf(cpu_t* cpu) {
        CLEAR_FLAGS(CF);

        return IS_LAST_CYCLE;
    }

    instruction_state_t cb(cpu_t* cpu) {
        _log(debug, "CB prefix unimplemented!", cpu->i_latch);

        cpu->pc++;

        return IS_LAST_CYCLE;
    }

    instruction_state_t unk(cpu_t* cpu) {
        _log(debug, "Unimplemented instruction %02x!", cpu->i_latch);

        return IS_LAST_CYCLE;
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
#undef X
#undef Y
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