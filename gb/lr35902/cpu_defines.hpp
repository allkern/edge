#pragma once

#include "../macros.hpp"
#include "../structs.hpp"

namespace gb {
    enum cpu_state_t {
        ST_FETCH,
        ST_EXECUTE,
        ST_EXECUTE_FETCH,
        ST_HALT,
        ST_STOP
    };

    enum instruction_state_t {
        IS_DONE,
        IS_EXECUTING,
        IS_LAST_CYCLE
    };

    typedef instruction_state_t (*cpu_instruction_t)(cpu_t*);
}