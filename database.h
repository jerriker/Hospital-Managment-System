#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite/sqlite3.h"
#include <iostream>
#include <string>
#include <vector>

class Database {
private:
    sqlite3* db;
    std::string dbPath;

public:
    Database(const std::string& path);
    ~Database();
    
    bool open();
    void close();
    bool executeQuery(const std::string& query);
    bool executeQueryWithCallback(const std::string& query, int (*callback)(void*, int, char**, char**), void* data);
    
    // User management
    bool addUser(int userId, const std::string& name, const std::string& password, const std::string& role);
    bool authenticateUser(int userId, const std::string& password);
    std::string getUserRole(int userId);
    std::string getUserName(int userId);
    
    // Patient management
    bool addPatient(int patientId, const std::string& name, const std::string& gender, int age, 
                   float weight, float height, const std::string& description);
    bool updatePatient(int patientId, const std::string& name, const std::string& gender, int age, 
                      float weight, float height, const std::string& description);
    bool getPatientById(int patientId, std::string& name, std::string& gender, int& age, 
                       float& weight, float& height, std::string& description);
    bool getAllPatients();
    
    // Appointment management
    bool initializeAppointmentSlots();
    bool bookAppointment(int day, int slot, int appointmentId, const std::string& patientName);
    bool cancelAppointment(int appointmentId);
    bool getAppointmentSlots();
    bool getBookedAppointments();
    
    // Staff management
    bool addStaff(const std::string& name);
    bool markAttendance(const std::string& name, bool isPresent);
    bool getStaffAttendance();
};

#endif // DATABASE_H