/*
 * Copyright (C) 2022-2025 MachineWare GmbH  
 *
 * This file is licensed for non-commercial use only.
 * You may use, modify, and distribute this file for personal or educational purposes,
 * but any commercial use, including but not limited to selling, licensing, or
 * integrating this code into proprietary software, is strictly prohibited unless otherwise agreed to in writing by MachineWare GmbH.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES,
 * OR OTHER LIABILITY ARISING FROM THE USE OF THIS SOFTWARE.
 */

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
