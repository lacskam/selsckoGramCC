#include "db_proccessing.h"
#include "logs.h"

db_conection::db_conection()
    : cx(DB_CONN_STRING)
{
    try {
        if (cx.is_open()) {
            LOG_INFO_MSG("Connected to " +std::string(cx.dbname()) +" - [db_conection]");
        }
    }
    catch (const std::exception& e) {
        LOG_ERROR_MSG(
            std::string(e.what())+" - [db_conection]");
    }
}
