#include "database.h"

// Callback function for query results
static int callback(void* data, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    std::cout << std::endl;
    return 0;
}

Database::Database(const std::string& path) : dbPath(path), db(nullptr) {}

Database::~Database() {
    close();
}

bool Database::open() {
    // Print the path for debugging
    std::cout << "Attempting to open database at: " << dbPath << std::endl;
    std::cout << "SQLite version: " << sqlite3_libversion() << std::endl;
    
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    std::cout << "Database connection established successfully." << std::endl;
    
    // Create tables if they don't exist
    bool success = true;
    success &= executeQuery("CREATE TABLE IF NOT EXISTS users ("
                "user_id INTEGER PRIMARY KEY,"
                "name TEXT NOT NULL,"
                "password TEXT NOT NULL,"
                "role TEXT NOT NULL)");
                
    success &= executeQuery("CREATE TABLE IF NOT EXISTS patients ("
                "patient_id INTEGER PRIMARY KEY,"
                "name TEXT NOT NULL,"
                "gender TEXT NOT NULL,"
                "age INTEGER,"
                "weight REAL,"
                "height REAL,"
                "description TEXT)");
                
    success &= executeQuery("CREATE TABLE IF NOT EXISTS appointments ("
                "appointment_id INTEGER PRIMARY KEY,"
                "day INTEGER,"
                "slot INTEGER,"
                "patient_name TEXT,"
                "UNIQUE(day, slot))");
                
    success &= executeQuery("CREATE TABLE IF NOT EXISTS staff ("
                "staff_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "name TEXT UNIQUE NOT NULL,"
                "is_present BOOLEAN DEFAULT 0)");
    
    if (!success) {
        std::cerr << "Failed to create one or more tables." << std::endl;
        return false;
    }

    std::cout << "Database tables created/verified successfully." << std::endl;
    return true;
}

void Database::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool Database::executeQuery(const std::string& query) {
    if (!db) {
        std::cerr << "Database connection is not open" << std::endl;
        return false;
    }
    
    char* errMsg = nullptr;
    std::cout << "Executing query: " << query.substr(0, 60) << "..." << std::endl;
    
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::executeQueryWithCallback(const std::string& query, int (*callback)(void*, int, char**, char**), void* data) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), callback, data, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// User management methods
bool Database::addUser(int userId, const std::string& name, const std::string& password, const std::string& role) {
    std::string query = "INSERT INTO users (user_id, name, password, role) VALUES (" + 
                        std::to_string(userId) + ", '" + name + "', '" + password + "', '" + role + "')";
    return executeQuery(query);
}

bool Database::authenticateUser(int userId, const std::string& password) {
    // Implementation would check if user exists with given password
    // For simplicity, we'll just return true for now
    return true;
}

std::string Database::getUserRole(int userId) {
    std::string role = "";
    std::string query = "SELECT role FROM users WHERE user_id = " + std::to_string(userId);
    
    auto roleCallback = [](void* data, int argc, char** argv, char** azColName) -> int {
        std::string* role = static_cast<std::string*>(data);
        if (argc > 0 && argv[0]) {
            *role = argv[0];
        }
        return 0;
    };
    
    executeQueryWithCallback(query, roleCallback, &role);
    return role;
}

std::string Database::getUserName(int userId) {
    std::string name = "";
    std::string query = "SELECT name FROM users WHERE user_id = " + std::to_string(userId);
    
    auto nameCallback = [](void* data, int argc, char** argv, char** azColName) -> int {
        std::string* name = static_cast<std::string*>(data);
        if (argc > 0 && argv[0]) {
            *name = argv[0];
        }
        return 0;
    };
    
    executeQueryWithCallback(query, nameCallback, &name);
    return name;
}

// Implementation for initializeAppointmentSlots method
bool Database::initializeAppointmentSlots() {
    // First, clear any existing appointments
    executeQuery("DELETE FROM appointments");
    
    // Initialize all slots as available
    for (int day = 0; day < 7; day++) {
        for (int slot = 0; slot < 10; slot++) {
            std::string query = "INSERT OR IGNORE INTO appointments (day, slot, patient_name) VALUES (" + 
                               std::to_string(day) + ", " + 
                               std::to_string(slot) + ", 'Available')";
            if (!executeQuery(query)) {
                return false;
            }
        }
    }
    return true;
}

// Implementation for getAppointmentSlots method
bool Database::getAppointmentSlots() {
    return executeQueryWithCallback("SELECT day, slot, patient_name FROM appointments ORDER BY day, slot", callback, nullptr);
}

// Implementation for getBookedAppointments method
bool Database::getBookedAppointments() {
    return executeQueryWithCallback("SELECT day, slot, patient_name FROM appointments WHERE patient_name != 'Available' ORDER BY day, slot", callback, nullptr);
}

// Implementation for bookAppointment method
bool Database::bookAppointment(int day, int slot, int appointmentId, const std::string& patientName) {
    std::string query = "UPDATE appointments SET patient_name = '" + patientName + "', appointment_id = " + 
                        std::to_string(appointmentId) + " WHERE day = " + std::to_string(day) + 
                        " AND slot = " + std::to_string(slot);
    return executeQuery(query);
}

// Implementation for cancelAppointment method
bool Database::cancelAppointment(int appointmentId) {
    std::string query = "UPDATE appointments SET patient_name = 'Available', appointment_id = 0 WHERE appointment_id = " + 
                        std::to_string(appointmentId);
    return executeQuery(query);
}

bool Database::reopenDatabase() {
    // Close the database if it's open
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
    
    // Reopen the database
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        std::cerr << "Can't reopen database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    std::cout << "Database reopened successfully." << std::endl;
    return true;
}

// Implementation for checkDatabaseFile method
bool Database::checkDatabaseFile() {
    if (!db) {
        std::cerr << "Database connection is not open" << std::endl;
        return false;
    }
    
    // Check if we can execute a simple query
    return executeQuery("SELECT 1");
}


