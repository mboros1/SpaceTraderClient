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

std::string RegisterDb::getToken(const char *string) {
    std::string sql = "SELECT token FROM agents WHERE name = '" + std::string(string) + "';";
    char *zErrMsg = 0;

    // Create a vector to store the token(s)
    std::vector<std::string> tokens;

    // Define the lambda function
    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
        auto& tokens = *static_cast<std::vector<std::string>*>(data);
        for(int i = 0; i<argc; i++) {
            if (std::string(azColName[i]) == "token") {
                tokens.push_back(argv[i] ? argv[i] : "NULL");
            }
        }
        return 0;
    };

    // Get tokens from db
    int rc = sqlite3_exec(db, sql.c_str(), callback, &tokens, &zErrMsg);
    if (zErrMsg) {
        std::cerr << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
    } else {
        std::cout << "Token(s) retrieved successfully" << std::endl;
    }

    assert(tokens.size() == 1);

    return tokens[0];
}

bool RegisterDb::hasToken(const char *string) {
    std::string sql = "SELECT token FROM agents WHERE name = '" + std::string(string) + "';";
    char *zErrMsg = 0;

    // Define the lambda function
    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
        auto& hasToken = *static_cast<bool*>(data);
        for(int i = 0; i<argc; i++) {
            if (std::string(azColName[i]) == "token") {
                hasToken = true;
                return 0;
            }
        }
        hasToken = false;
        return 0;
    };

    // get token from db
    bool hasToken = false;
    int rc = sqlite3_exec(db, sql.c_str(), callback, &hasToken, &zErrMsg);

    if (zErrMsg) {
        std::cerr << zErrMsg << std::endl;
    } else if (hasToken) {
        std::cout << "Token retrieved successfully" << std::endl;
        return true;
    }
    return false;
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
