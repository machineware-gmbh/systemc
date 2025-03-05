/******************************************************************************
 *                                                                            *
 * Copyright (C) 2022-2025 MachineWare GmbH                                   *
 *                                                                            *
 * This file is licensed for non-commercial use only.                         *
 * You may use, modify, and distribute this file for personal or educational  *
 * purposes, but any commercial use, including but not limited to selling,    *
 * licensing, or integrating this code into proprietary software, is strictly *
 * prohibited unless otherwise agreed to in writing by MachineWare GmbH.      *
 *                                                                            *
 * THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS   *
 * OR IMPLIED. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE   *
 * FOR ANY CLAIM, DAMAGES, OR OTHER LIABILITY ARISING FROM THE USE OF         *
 * THIS SOFTWARE.                                                             *
 *                                                                            *
 ******************************************************************************/

#include "inscight/context.h"
#include "inscight/database.h"
#include "inscight/database_csv.h"
#include "inscight/database_sql.h"

namespace inscight {

static database* create_database(const std::string& options) {
    if (options.find("csv") != std::string::npos)
        return new database_csv(options);
    else
        return new database_sql(options);
}

context::context(const std::string& options):
    m_db(create_database(options)) {
    m_db->start();
}

context::~context() {
    delete m_db;
}

static context* init() {
    const char* str = getenv("INSCIGHT");
    if (str == nullptr || strcmp(str, "0") == 0)
        return nullptr;
    static context singleton(str);
    return &singleton;
}

context* ctx = init();

} // namespace inscight
