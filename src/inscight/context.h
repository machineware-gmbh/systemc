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

#ifndef INSCIGHT_CONTEXT_H
#define INSCIGHT_CONTEXT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <string>

#include "inscight/entry.h"
#include "inscight/database.h"

namespace inscight {

class context
{
private:
    database* m_db;

public:
    context(const std::string& options);
    virtual ~context();

    template <typename... ARGS>
    void trace(ARGS&&... args) {
        if (m_db != nullptr)
            m_db->insert(std::forward<ARGS>(args)...);
    }
};

extern context* ctx;

} // namespace inscight

#endif
