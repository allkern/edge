#pragma once

#include "../macros.hpp"
#include "../structs.hpp"

#include "cpu_instructions.hpp"

namespace gb {
    static cpu_instruction_t instruction_table[] = {
    /*  X0          X1          X2          X3          X4          X5          X6          X7                */
    /*  X8          X9          Xa          Xb          Xc          Xd          Xe          Xf                */
        nop,        ld_rr_nn,   nop,        nop,        nop,        nop,        ld_r_n,     nop,        /* 0X */
        ld_nn_sp,   nop,        ld_a_bc,    nop,        nop,        nop,        ld_r_n,     nop,        
        nop,        ld_rr_nn,   nop,        nop,        nop,        nop,        ld_r_n,     nop,        /* 1X */
        jr_n,       nop,        ld_a_de,    nop,        nop,        nop,        ld_r_n,     nop,        
        jr_cc_n,    ld_rr_nn,   ld_hli_a,   nop,        nop,        nop,        ld_r_n,     nop,        /* 2X */
        jr_cc_n,    nop,        ld_a_hli,   nop,        nop,        nop,        ld_r_n,     nop,        
        jr_cc_n,    ld_rr_nn,   ld_hld_a,   nop,        nop,        nop,        ld_hl_n,    nop,        /* 3X */
        jr_cc_n,    nop,        ld_a_hld,   nop,        nop,        nop,        ld_r_n,     nop,        
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
        ret_cc,     pop_bc,     jp_cc_nn,   jp_nn,      call_cc_nn, push_bc,    nop,        rst_n,      /* cX */
        ret_cc,     ret,        jp_cc_nn,   nop,        call_cc_nn, call_nn,    nop,        rst_n,      
        ret_cc,     pop_de,     jp_cc_nn,   nop,        call_cc_nn, push_de,    nop,        rst_n,      /* dX */
        ret_cc,     reti,       jp_cc_nn,   nop,        call_cc_nn, nop,        nop,        rst_n,      
        ldh_n_a,    pop_hl,     ldh_c_a,    nop,        nop,        push_hl,    nop,        rst_n,      /* eX */
        nop,        jp_hl,      ld_nn_a,    nop,        nop,        nop,        nop,        rst_n,      
        ldh_a_n,    pop_af,     ldh_a_c,    nop,        nop,        push_af,    nop,        rst_n,      /* fX */
        nop,        ld_sp_hl,   ld_a_nn,    nop,        nop,        nop,        nop,        rst_n,      
    };
}