#pragma once

#include "../macros.hpp"
#include "../structs.hpp"

#include "cpu_instructions.hpp"

namespace gb {
    static cpu_instruction_t instruction_table[] = {
    /*  X0          X1          X2          X3          X4          X5          X6          X7                */
    /*  X8          X9          Xa          Xb          Xc          Xd          Xe          Xf                */
        nop,        ld_rr_nn,   unk,        unk,        inc_r,      dec_r,      ld_r_n,     unk,        /* 0X */
        ld_nn_sp,   unk,        ld_a_bc,    unk,        inc_r,      dec_r,      ld_r_n,     unk,        
        unk,        ld_rr_nn,   unk,        unk,        inc_r,      dec_r,      ld_r_n,     unk,        /* 1X */
        jr_n,       unk,        ld_a_de,    unk,        inc_r,      dec_r,      ld_r_n,     unk,        
        jr_cc_n,    ld_rr_nn,   ld_hli_a,   unk,        inc_r,      dec_r,      ld_r_n,     unk,        /* 2X */
        jr_cc_n,    unk,        ld_a_hli,   unk,        inc_r,      dec_r,      ld_r_n,     cpl_a,      
        jr_cc_n,    ld_rr_nn,   ld_hld_a,   unk,        inc_dhl,    dec_dhl,    ld_hl_n,    scf,        /* 3X */
        jr_cc_n,    unk,        ld_a_hld,   unk,        inc_r,      dec_r,      ld_r_n,     ccf,        
        ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_hl,    ld_r_r,     /* 4X */
        ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_hl,    ld_r_r,     
        ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_hl,    ld_r_r,     /* 5X */
        ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_hl,    ld_r_r,     
        ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_hl,    ld_r_r,     /* 6X */
        ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_hl,    ld_r_r,     
        ld_hl_r,    ld_hl_r,    ld_hl_r,    ld_hl_r,    ld_hl_r,    ld_hl_r,    unk,        ld_hl_r,    /* 7X */
        ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_r,     ld_r_hl,    ld_r_r,     
        add_a_r,    add_a_r,    add_a_r,    add_a_r,    add_a_r,    add_a_r,    add_a_hl,   add_a_r,    /* 8X */
        add_a_r,    add_a_r,    add_a_r,    add_a_r,    add_a_r,    add_a_r,    add_a_hl,   add_a_r,    
        sub_a_r,    sub_a_r,    sub_a_r,    sub_a_r,    sub_a_r,    sub_a_r,    sub_a_hl,   sub_a_r,    /* 9X */
        sub_a_r,    sub_a_r,    sub_a_r,    sub_a_r,    sub_a_r,    sub_a_r,    sub_a_hl,   sub_a_r,    
        and_a_r,    and_a_r,    and_a_r,    and_a_r,    and_a_r,    and_a_r,    and_a_hl,   and_a_r,    /* aX */
        or_a_r,     or_a_r,     or_a_r,     or_a_r,     or_a_r,     or_a_r,     or_a_hl,    or_a_r,     
        xor_a_r,    xor_a_r,    xor_a_r,    xor_a_r,    xor_a_r,    xor_a_r,    xor_a_hl,   xor_a_r,    /* bX */
        cp_a_r,     cp_a_r,     cp_a_r,     cp_a_r,     cp_a_r,     cp_a_r,     cp_a_hl,    cp_a_r,     
        ret_cc,     pop_bc,     jp_cc_nn,   jp_nn,      call_cc_nn, push_bc,    add_a_n,    rst_n,      /* cX */
        ret_cc,     ret,        jp_cc_nn,   cb,         call_cc_nn, call_nn,    add_a_n,    rst_n,      
        ret_cc,     pop_de,     jp_cc_nn,   unk,        call_cc_nn, push_de,    sub_a_n,    rst_n,      /* dX */
        ret_cc,     reti,       jp_cc_nn,   unk,        call_cc_nn, unk,        sub_a_n,    rst_n,      
        ldh_n_a,    pop_hl,     ldh_c_a,    unk,        unk,        push_hl,    and_a_n,    rst_n,      /* eX */
        unk,        jp_hl,      ld_nn_a,    unk,        unk,        unk,        or_a_n,     rst_n,      
        ldh_a_n,    pop_af,     ldh_a_c,    unk,        unk,        push_af,    xor_a_n,    rst_n,      /* fX */
        unk,        ld_sp_hl,   ld_a_nn,    unk,        unk,        unk,        cp_a_n,     rst_n       
    };
}