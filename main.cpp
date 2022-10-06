#include "gb/gameboy.hpp"
#include "gb/log.hpp"

void log_cpu_state(gb::gameboy_t* gb) {
    if (!gb->cpu.ck_half_cycle) {
        _log(debug, "M cycle start");
    }

    _log(debug, "CKH=%u, CLK=%u, PHI=%u, A0-A14=%04x, A15=%u, CS=%u, D0-D7=%02x, A=%02x, B=%02x, C=%02x, D=%02x, E=%02x, F=%02x, H=%02x, L=%02x",
        gb->cpu.ck_half_cycle,
        !(gb->cpu.ck_half_cycle & 1),
        !((gb->cpu.ck_half_cycle >> 2) & 1),
        gb->lr35902.pins.a & 0x7fff,
        (gb->lr35902.pins.a >> 15) & 0x1,
        gb->lr35902.pins.cs,
        gb->lr35902.pins.d,
        gb->cpu.r[7],
        gb->cpu.r[0],
        gb->cpu.r[1],
        gb->cpu.r[2],
        gb->cpu.r[3],
        gb->cpu.r[6],
        gb->cpu.r[4],
        gb->cpu.r[5]
    );
}

int main() {
    _log::init("gb");

    gb::gameboy_t gb;
    gb::init(&gb);

    // Test
    gb.cpu.r[0] = 0xaa;

    log_cpu_state(&gb);

    for (int i = 0; i < 32; i++) {
        gb::clock(&gb);

        log_cpu_state(&gb);
    }
}