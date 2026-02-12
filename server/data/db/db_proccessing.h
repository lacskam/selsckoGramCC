#ifndef DB_PROCESSING_H
#define DB_PROCESSING_H

#include <pqxx/pqxx>
#include <iostream>
#include "db_config.h"

class db_conection {
public:
    db_conection();
private:
    pqxx::connection cx;


};

#endif // DB_PROCESSING_H
