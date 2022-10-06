#pragma once

#include <cstdint>
#include <cstring>

#include "../structs.hpp"
#include "../macros.hpp"

#include "lr35902_struct.hpp"

#include "cpu_struct.hpp"
#include "cpu_funcs.hpp"

namespace gb {
    void lr35902_init(lr35902_t* lr35902) {
        std::memset(lr35902, 0, sizeof(lr35902_t));

        lr35902->pins.ck[0] = true;
        lr35902->pins.phi = true;
    }

    void lr35902_clock(lr35902_t* lr35902) {
        lr35902->pins.ck[1] = lr35902->pins.ck[0];
        lr35902->pins.ck[0] = !lr35902->pins.ck[0];

        cpu_clock(lr35902->cpu);
    }
}