//
// Created by Martin Boros on 5/9/23.
//

#include "register_db.h"

RegisterDb::RegisterDb() {
    char *zErrMsg = 0;
    int rc = sqlite3_open("spacetrader.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }
    // check if table "agents" exists, if not create it
    std::string sql = "CREATE TABLE IF NOT EXISTS agents("  \
         "id INTEGER PRIMARY KEY AUTOINCREMENT," \
         "name           TEXT    NOT NULL," \
         "token          TEXT    NOT NULL);";
    rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }

}

static int callback(void *data, int argc, char **argv, char **azColName) {
    int i;
    fprintf(stderr, "%s: ", (const char*)data);

    for(i = 0; i<argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

std::string RegisterDb::getToken(const char *string) {
    std::string sql = "SELECT token FROM agents WHERE name = '" + std::string(string) + "';";
    char *zErrMsg = 0;

    // get token from db
    char *token = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), callback, &token, &zErrMsg);
    if (zErrMsg) {
        std::cerr << zErrMsg << std::endl;
    } else {
        std::cout << "Token retrieved successfully" << std::endl;
    }
    return std::string(token);
}


bool RegisterDb::hasToken(const char *string) {
    std::string sql = "SELECT token FROM agents WHERE name = '" + std::string(string) + "';";
    char *zErrMsg = 0;

    // get token from db
    char *token = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), callback, &token, &zErrMsg);

    if (zErrMsg) {
        std::cerr << zErrMsg << std::endl;
    } else {
        std::cout << "Token retrieved successfully" << std::endl;
        return true;
    }
}

void RegisterDb::insertToken(const char *agentName, std::string authToken) {
    std::string sql = "INSERT INTO agents (name, token) VALUES ('" + std::string(agentName) + "', '" + authToken + "');";
    char* zErrMsg = 0;

    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);

    if (zErrMsg) {
        std::cerr << zErrMsg << std::endl;
    } else {
        std::cout << "Token inserted successfully" << std::endl;
    }
}
