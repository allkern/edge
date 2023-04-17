#pragma once

#include <cstdint>
#include <cstring>

#include "../structs.hpp"
#include "../macros.hpp"

#include "cpu_struct.hpp"

namespace gb {
    struct bootrom_t {
        bus_t* pins;

        bool boot_off;

        uint8_t* rom;
    };
}