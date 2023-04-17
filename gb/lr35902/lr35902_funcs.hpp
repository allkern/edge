#pragma once

#include <cstdint>
#include <cstring>

#include "../structs.hpp"
#include "../macros.hpp"

#include "lr35902_struct.hpp"

#include "cpu_struct.hpp"
#include "cpu_funcs.hpp"
#include "bootrom_struct.hpp"

#define RANGE(v, l, h) ((v >= l) && (v <= h))

namespace gb {
    void lr35902_init(lr35902_t* lr35902) {
        std::memset(lr35902, 0, sizeof(lr35902_t));

        lr35902->pins.ck[0] = true;
        lr35902->pins.phi = true;
        lr35902->ext_bus = &lr35902_idle_ext_bus;
    }

    inline bool lr35902_is_internal_cycle(lr35902_t* lr35902) {
        // If BootROM is mapped
        if (!lr35902->boot->boot_off) {

            // If A0-A15 is within bootROM then internal cycle
            return RANGE(lr35902->cpu->bus.a, 0x0000, 0x00ff);
        }

        return (lr35902->cpu->bus.a & 0x8000) &&   // A15 is high
               (lr35902->cpu->bus.cs);             // CS is high
    }

    void lr35902_clock(lr35902_t* lr35902) {
        // Copy D0-D7 between external and CPU
        // depending on whether the CPU is reading
        // or writing. (Only on external cycles)

        // if (!lr35902_is_internal_cycle(lr35902)) {
        //     if (cpu_bus_is_read(lr35902->cpu)) {
        //         lr35902->cpu->bus.d = lr35902->ext_bus->d;
        //     } else if (cpu_bus_is_write(lr35902->cpu)) {
        //         lr35902->ext_bus->d = lr35902->cpu->bus.d;
        //     }
        // }

        // This might work before and after??
        // Set external bus depending on whether the last
        // CPU cycle was an internal or external cycle
        if (lr35902_is_internal_cycle(lr35902)) {
            lr35902->ext_bus = &lr35902_idle_ext_bus;
        } else {
            lr35902->ext_bus = &lr35902->cpu->bus;
        }

        cpu_clock(lr35902->cpu);

        // Set external bus depending on whether the last
        // CPU cycle was an internal or external cycle
        if (lr35902_is_internal_cycle(lr35902)) {
            lr35902->ext_bus = &lr35902_idle_ext_bus;
        } else {
            lr35902->ext_bus = &lr35902->cpu->bus;
        }
    }
}