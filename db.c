#include <stdio.h>
#include <sqlite3.h>
#include "db.h"

void write_to_database(const char *db_name, const char *data) {
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open(db_name, &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    char *sql = sqlite3_mprintf("INSERT INTO Transactions(Data) VALUES('%q');", data);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    sqlite3_free(sql);
    sqlite3_close(db);
}
