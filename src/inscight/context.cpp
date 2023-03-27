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
