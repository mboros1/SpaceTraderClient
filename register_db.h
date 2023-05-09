//
// Created by Martin Boros on 5/9/23.
//

#ifndef SPACETRADERCLIENT_REGISTER_DB_H
#define SPACETRADERCLIENT_REGISTER_DB_H

#include <sqlite3.h>
#include <iostream>

class RegisterDb {
    sqlite3 *db;

public:
    RegisterDb();

    std::string getToken(const char *string);

    bool hasToken(const char *string);

    void insertToken(const char *agentName, std::string authToken);
};


#endif //SPACETRADERCLIENT_REGISTER_DB_H
