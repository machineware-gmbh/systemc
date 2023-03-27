/******************************************************************************
 *                                                                            *
 * Copyright 2023 MachineWare GmbH                                            *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is unpublished proprietary work owned by MachineWare GmbH. It may be  *
 * used, modified and distributed in accordance to the license specified by   *
 * the license file in the root directory of this project.                    *
 *                                                                            *
 ******************************************************************************/

#ifndef INSCIGHT_ID_H
#define INSCIGHT_ID_H

#include <stdlib.h>
#include <stdio.h>

namespace inscight {

typedef size_t id_t;

struct id {
    id_t val;

    id(): val() {
        static id_t n = 1;
        val = n++;
    }

    constexpr id_t operator()() const { return val; }
};

} // namespace inscight

#endif
