#pragma once

#include "../macros.hpp"
#include "../structs.hpp"

#include "cpu_instructions.hpp"

namespace gb {
    static cpu_instruction_t instruction_table[] = {
    /*  X0          X1          X2          X3          X4          X5          X6          X7                */
    /*  X8          X9          Xa          Xb          Xc          Xd          Xe          Xf                */
        nop,        nop,        nop,        nop,        nop,        nop,        ld_r_n,        nop,     /* 0X */
        nop,        nop,        ld_a_bc,    nop,        nop,        nop,        ld_r_n,        nop,     
        nop,        nop,        nop,        nop,        nop,        nop,        ld_r_n,        nop,     /* 1X */
        nop,        nop,        ld_a_de,    nop,        nop,        nop,        ld_r_n,        nop,     
        nop,        nop,        ld_hli_a,   nop,        nop,        nop,        ld_r_n,        nop,     /* 2X */
        nop,        nop,        ld_a_hli,   nop,        nop,        nop,        ld_r_n,        nop,     
        nop,        nop,        ld_hld_a,   nop,        nop,        nop,        ld_hl_n,       nop,     /* 3X */
        nop,        nop,        ld_a_hld,   nop,        nop,        nop,        ld_r_n,        nop,     
        ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_hl,    ld_r_r,     /* 4X */
        ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_hl,    ld_r_r,     
        ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_hl,    ld_r_r,     /* 5X */
        ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_hl,    ld_r_r,     
        ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_hl,    ld_r_r,     /* 6X */
        ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_hl,    ld_r_r,     
        ld_hl_r,    ld_hl_r,    ld_hl_r,    ld_hl_r,    ld_hl_r,    ld_hl_r,    nop,        ld_hl_r,    /* 7X */
        ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_hl,    ld_r_r,     
        nop,        nop,        nop,        nop,        nop,        nop,        nop,        nop,        /* 8X */
        nop,        nop,        nop,        nop,        nop,        nop,        nop,        nop,        
        nop,        nop,        nop,        nop,        nop,        nop,        nop,        nop,        /* 9X */
        nop,        nop,        nop,        nop,        nop,        nop,        nop,        nop,        
        nop,        nop,        nop,        nop,        nop,        nop,        nop,        nop,        /* aX */
        nop,        nop,        nop,        nop,        nop,        nop,        nop,        nop,        
        nop,        nop,        nop,        nop,        nop,        nop,        nop,        nop,        /* bX */
        nop,        nop,        nop,        nop,        nop,        nop,        nop,        nop,        
        nop,        nop,        nop,        nop,        nop,        nop,        nop,        nop,        /* cX */
        nop,        nop,        nop,        nop,        nop,        nop,        nop,        nop,        
        nop,        nop,        nop,        nop,        nop,        nop,        nop,        nop,        /* dX */
        nop,        nop,        nop,        nop,        nop,        nop,        nop,        nop,        
        ldh_n_a,    nop,        ldh_c_a,    nop,        nop,        nop,        nop,        nop,        /* eX */
        nop,        nop,        ld_nn_a,    nop,        nop,        nop,        nop,        nop,        
        ldh_a_n,    nop,        ldh_a_c,    nop,        nop,        nop,        nop,        nop,        /* fX */
        nop,        nop,        ld_a_nn,    nop,        nop,        nop,        nop,        nop,        
    };
}