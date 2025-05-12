#include "database.h"

// Callback function for query results
static int callback(void *data, int argc, char **argv, char **azColName)
{
    for (int i = 0; i < argc; i++)
    {
        std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    std::cout << std::endl;
    return 0;
} //(is it really needed!)

Database::Database(const std::string &path) : dbPath(path), db(nullptr) {}

Database::~Database()
{
    close();
}

bool Database::open()
{
    // Print the path for debugging
    // std::cout << "Attempting to open database at: " << dbPath << std::endl;
    // std::cout << "SQLite version: " << sqlite3_libversion() << std::endl;

    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // std::cout << "Database connection established successfully." << std::endl;

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
                            "id INTEGER PRIMARY KEY, "
                            "appointment_date TEXT NOT NULL)");

    success &= executeQuery("CREATE TABLE IF NOT EXISTS staff ("
                            "staff_id INTEGER PRIMARY KEY UNIQUE,"
                            "name TEXT UNIQUE NOT NULL,"
                            "role TEXT NOT NULL,"
                            "is_present BOOLEAN DEFAULT 0)");

    if (!success)
    {
        std::cerr << "Failed to create one or more tables." << std::endl;
        return false;
    }

    // std::cout << "Database tables created/verified successfully." << std::endl;
    return true;
}

void Database::close()
{
    if (db)
    {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool Database::executeQuery(const std::string &query)
{
    if (!db)
    {
        std::cerr << "Database connection is not open" << std::endl;
        return false;
    }

    char *errMsg = nullptr;

    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::executeQueryWithCallback(const std::string &query, int (*callback)(void *, int, char **, char **), void *data)
{
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), callback, data, &errMsg);

    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// User management methods
bool Database::addUser(int userId, const std::string &name, const std::string &password, const std::string &role)
{
    std::string query = "INSERT INTO users (user_id, name, password, role) VALUES (" +
                        std::to_string(userId) + ", '" + name + "', '" + password + "', '" + role + "')";
    return executeQuery(query);
}

bool Database::authenticateUser(int userId, const std::string &password)
{
    std::string query = "SELECT password FROM users WHERE user_id = " + std::to_string(userId);

    std::string passwordHash = "";

    auto authCallback = [](void *data, int argc, char **argv, char **azColName) -> int
    {
        std::string *passwordHash = static_cast<std::string *>(data);
        if (argc > 0 && argv[0])
        {
            *passwordHash = argv[0];
        }
        return 0;
    };

    executeQueryWithCallback(query, authCallback, &passwordHash);

    // Compare the passwordHash with the provided password
    // For simplicity, we'll just return true for now
    return passwordHash == password;
}

std::string Database::getUserRole(int userId)
{
    std::string role = "";
    std::string query = "SELECT role FROM users WHERE user_id = " + std::to_string(userId);

    auto roleCallback = [](void *data, int argc, char **argv, char **azColName) -> int
    {
        std::string *role = static_cast<std::string *>(data);
        if (argc > 0 && argv[0])
        {
            *role = argv[0];
        }
        return 0;
    };

    executeQueryWithCallback(query, roleCallback, &role);
    return role;
}

std::string Database::getUserName(int userId)
{
    std::string name = "";
    std::string query = "SELECT name FROM users WHERE user_id = " + std::to_string(userId);

    auto nameCallback = [](void *data, int argc, char **argv, char **azColName) -> int
    {
        std::string *name = static_cast<std::string *>(data);
        if (argc > 0 && argv[0])
        {
            *name = argv[0];
        }
        return 0;
    };

    executeQueryWithCallback(query, nameCallback, &name);
    return name;
}

// Staff management methods

bool Database::addStaff(const std::string &name, const std::string &role)
{
    std::string query = "INSERT INTO staff (name, role) VALUES ('" + name + "', '" + role + "')";
    return executeQuery(query);
}
bool Database::checkStaff(int adminId)
{
    std::string result = "";
    std::string query = "SELECT role FROM staff WHERE staff_id = " + std::to_string(adminId);

    auto roleCallback = [](void *data, int argc, char **argv, char **azColName) -> int
    {
        std::string *result = static_cast<std::string *>(data);
        if (argc > 0 && argv[0])
        {
            *result = argv[0];
        }
        return 0;
    };

    executeQueryWithCallback(query, roleCallback, &result);
    return !result.empty();
};

bool Database::deleteStaff(int staff_id)
{
    std::string query = "delete from staff where staff_id = " + staff_id;
    return executeQuery(query);
};

// Implementation for initializeAppointmentSlots method
bool Database::initializeAppointmentSlots()
{
    // First, clear any existing appointments
    executeQuery("DELETE FROM appointments");

    // Initialize all slots as available
    for (int day = 0; day < 7; day++)
    {
        for (int slot = 0; slot < 10; slot++)
        {
            std::string query = "INSERT OR IGNORE INTO appointments (day, slot, patient_name) VALUES (" +
                                std::to_string(day) + ", " +
                                std::to_string(slot) + ", 'Available')";
            if (!executeQuery(query))
            {
                return false;
            }
        }
    }
    return true;
}

// Implementation for getAppointmentSlots method
bool Database::getAppointmentSlots()
{
    return executeQueryWithCallback("SELECT day, slot, patient_name FROM appointments ORDER BY day, slot", callback, nullptr);
}

// Implementation for getBookedAppointments method
bool Database::getBookedAppointments()
{
    return executeQueryWithCallback("SELECT day, slot, patient_name FROM appointments WHERE patient_name != 'Available' ORDER BY day, slot", callback, nullptr);
}

// Implementation for bookAppointment method
bool Database::bookAppointment(int appointmentId, const std::string& appointmentDate) {
    std::string sql = "INSERT INTO appointments (id, appointment_date) VALUES (" +
                      std::to_string(appointmentId) + ", '" +
                      appointmentDate + "');";

    return executeQuery(sql);
}

// Implementation for cancelAppointment method
bool Database::cancelAppointment(int appointmentId)
{
    std::string query = "DELETE FROM appointments WHERE id = " + std::to_string(appointmentId);
    return executeQuery(query);
}

bool Database::reopenDatabase()
{
    // Close the database if it's open
    if (db)
    {
        sqlite3_close(db);
        db = nullptr;
    }

    // Reopen the database
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc)
    {
        std::cerr << "Can't reopen database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // std::cout << "Database reopened successfully." << std::endl;
    return true;
}

// Implementation for checkDatabaseFile method
bool Database::checkDatabaseFile()
{
    if (!db)
    {
        std::cerr << "Database connection is not open" << std::endl;
        return false;
    }

    // Check if we can execute a simple query
    return executeQuery("SELECT 1");
}
// Patient management methods
bool Database::addPatient(int patientId, const std::string &name, const std::string &gender, int age,
                          float weight, float height, const std::string &description)
{
    std::string query = "INSERT INTO patients (patient_id, name, gender, age, weight, height, description) VALUES (" +
                        std::to_string(patientId) + ", '" + name + "', '" + gender + "', " +
                        std::to_string(age) + ", " + std::to_string(weight) + ", " + std::to_string(height) + ", '" + description + "')";
    return executeQuery(query);
}

bool Database::getAllPatients(int (*callback)(void *, int, char **, char **))
{
    std::string query = "SELECT name, gender, patient_id, age, weight, height, description FROM patients";
    return executeQueryWithCallback(query, callback, nullptr);
}

// Implementation for getStaffId method
int Database::getStaffId(const std::string &name)
{
    int staffId = -1; // Default value if staff not found
    std::string query = "SELECT staff_id FROM staff WHERE name = '" + name + "'";

    auto idCallback = [](void *data, int argc, char **argv, char **azColName) -> int
    {
        int *staffId = static_cast<int *>(data);
        if (argc > 0 && argv[0])
        {
            *staffId = std::stoi(argv[0]);
        }
        return 0;
    };

    executeQueryWithCallback(query, idCallback, &staffId);
    return staffId;
}

// Check if an appointment date is available
bool Database::isAppointmentAvailable(const std::string &appointmentDate)
{
    bool isAvailable = true;
    std::string query = "SELECT COUNT(*) FROM appointments WHERE appointment_date = '" + appointmentDate + "'";
    
    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int
    {
        bool *available = static_cast<bool*>(data);
        if (argc > 0 && argv[0])
        {
            int count = std::stoi(argv[0]);
            *available = (count == 0); // Available if no appointments exist for this date
        }
        return 0;
    };
    
    executeQueryWithCallback(query, callback, &isAvailable);
    return isAvailable;
}

// Get all appointments from database
std::vector<std::tuple<int, std::string>> Database::getAllAppointments()
{
    std::vector<std::tuple<int, std::string>> appointments;
    std::string query = "SELECT id, appointment_date FROM appointments ORDER BY appointment_date";
    
    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int
    {
        auto *appts = static_cast<std::vector<std::tuple<int, std::string>>*>(data);
        if (argc >= 2 && argv[0] && argv[1])
        {
            int id = std::stoi(argv[0]);
            std::string date = argv[1];
            appts->push_back(std::make_tuple(id, date));
        }
        return 0;
    };
    
    executeQueryWithCallback(query, callback, &appointments);
    return appointments;
}
