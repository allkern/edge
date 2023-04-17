#include "gb/gameboy.hpp"
#include "gb/log.hpp"

void log_cpu_state_hck(gb::gameboy_t* gb) {
    if (!gb->cpu.ck_half_cycle) {
        _log(debug, "M cycle start");
    }

    _log(debug, "CKH=%u, PC=%04x, A0-A14=%04x, RD=%u, WR=%u, A15=%u, CS=%u, D0-D7=%02x, AF=%04x, BC=%04x, DE=%04x, HL=%04x, SP=%04x, CYC=%u",
        gb->cpu.ck_half_cycle,
        gb->cpu.pc,
        gb->soc.ext_bus->a & 0x7fff,
        gb->soc.ext_bus->rd,
        gb->soc.ext_bus->wr,
        gb->soc.ext_bus->cs,
        (gb->soc.ext_bus->a >> 15) & 0x1,
        gb->soc.ext_bus->d,
        (gb->cpu.r[7] << 8) | gb->cpu.r[6],
        (gb->cpu.r[0] << 8) | gb->cpu.r[1],
        (gb->cpu.r[2] << 8) | gb->cpu.r[3],
        (gb->cpu.r[4] << 8) | gb->cpu.r[5],
        gb->cpu.sp,
        gb->cpu.total_t_cycles
    );
}

void log_cpu_state_m(gb::gameboy_t* gb) {
    if (!gb->cpu.ck_half_cycle) {
        _log(debug, "PC=%04x, A0-A14=%04x, RD=%u, WR=%u, A15=%u, CS=%u, D0-D7=%02x, AF=%04x, BC=%04x, DE=%04x, HL=%04x, SP=%04x, CYC=%u",
            gb->cpu.pc,
            gb->soc.ext_bus->a & 0x7fff,
            gb->soc.ext_bus->rd,
            gb->soc.ext_bus->wr,
            gb->soc.ext_bus->cs,
            (gb->soc.ext_bus->a >> 15) & 0x1,
            gb->soc.ext_bus->d,
            (gb->cpu.r[7] << 8) | gb->cpu.r[6],
            (gb->cpu.r[0] << 8) | gb->cpu.r[1],
            (gb->cpu.r[2] << 8) | gb->cpu.r[3],
            (gb->cpu.r[4] << 8) | gb->cpu.r[5],
            gb->cpu.sp,
            gb->cpu.total_t_cycles
        );
    }
}



int main() {
    _log::init("gb");

    gb::gameboy_t gb;
    gb::init(&gb);

    //log_cpu_state_m(&gb);

    for (int i = 0; i < 9 * M; i++) {
        gb::clock(&gb);

        log_cpu_state_m(&gb);
    }
}