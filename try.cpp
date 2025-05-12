#include <iostream>
#include <sqlite3.h>

int main() {
    sqlite3 *db;
    char *errMsg = nullptr;

    int rc = sqlite3_open("hospital.db", &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << '\n';
        return 1;
    }

    const char *sql = "DROP TABLE IF EXISTS staff;";
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << '\n';
        sqlite3_free(errMsg);
    } else {
        std::cout << "Table dropped successfully\n";
    }

    sqlite3_close(db);
    return 0;
}
