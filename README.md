# Hospital-Managment-System
# Group Members

| Member Name           |    ID Number   |
| --------------------- | -------------- |
| Bereket Desalegn      | ID: ETS0251/16 |
| Bekam Yoseph          | ID: ETS0240/16 |
| Befiker Kassahun      | ID: ETS0236/16 |
| Barok Yeshiber        | ID: ETS0224/16 |
| Bethelhem Degefu      | ID: ETS0283/16 |
| Addisalem Hailay      | ID: ETS0100/16 |



# Project Overview
The Hospital Management System is a C++ software project designed to digitize and streamline hospital operations. It aims to replace traditional, paper-based methods with an automated solution that enhances efficiency and accuracy in managing patient information, scheduling appointments, and maintaining medical records. The system prioritizes user-friendliness, data security, and effective access control to support hospital staff in delivering better healthcare services.

# Features
✅ Patient Management
Patient registration and information entry

Deleting Patient records

Viewing Patient records

✅ Staff Management
Staff registration and information entry

Remving of Existing Staff members

✅ Appointment Scheduling
Booking of patient appointments with doctors

Cancellation of appointments

Display of available appointment slots

✅ User Authentication & Role-Based Access Control
Login system for users with different roles (Admin, Patient)

Permissions and access based on assigned roles

Ensures data confidentiality and operational control

# Getting Started
# Prerequisites
- C++ compiler with C++17 support
- SQLite3 library
- MinGW (for Windows users)

# How to compile and run
  1. Clone the repository: https://github.com/jerriker/Hospital-Managment-System/blob/main/main.cpp
  2. Navigate to the project directory:

  3. Compile the project:
  4. Run the Path

# Project Structure

# Database schema
# Tables
# 1. Users
Stores login credentials and roles for system users (e.g., admin, doctor, receptionist).

| Column   | Type    | Constraints                          |
|----------|---------|--------------------------------------|
| user_id  | INTEGER | Primary Key, Auto-Increment          |
| name     | TEXT    | Not Null, Unique                     |
| password | TEXT    | Not Null                             |
| role     | TEXT    | Not Null (e.g., admin, doctor)       |

# 2. Patients
Holds basic patient information and medical notes.

| Column      | Type    | Constraints                      |
|-------------|---------|----------------------------------|
| patient_id  | INTEGER | Primary Key, Auto-Increment      |
| name        | TEXT    | Not Null                         |
| gender      | TEXT    | Not Null                         |
| age         | INTEGER |                                  |
| weight      | REAL    |                                  |
| height      | REAL    |                                  |
| description | TEXT    |                                  |


# 3. Appointments
Tracks available and booked appointment slots.

| Column       | Type        | Constraints                                  |
| ------------ | ----------- | -------------------------------------------- |
| day          | INTEGER     | Not Null                                     |
| slot         | INTEGER     | Not Null                                     |
| patient_name | TEXT        | Not Null (e.g., 'Available' or patient name) |
| PRIMARY KEY  | (day, slot) | Composite Primary Key                        |

# 4. Staff
Manages hospital staff details and attendance tracking.

| Column     | Type    | Constraints                     |
|------------|---------|---------------------------------|
| staff_id   | INTEGER | Primary Key, Auto-Increment     |
| name       | TEXT    | Not Null, Unique                |
| is_present | BOOLEAN | Default: 0 (false, not present) |
