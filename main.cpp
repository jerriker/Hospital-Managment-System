/**
 * Project title: Hospital management system
 * These project is done by the following group members with collaboration using github:
 * 1. Bereket Desalegn   ID: ETS0251/16
 * 2. Bekam Yoseph       ID: ETS0240/16
 * 3. Befiker Kassahun   ID: ETS0236/16
 * 4. Barok Yeshiber     ID: ETS0224/16
 * 5. Addisalem Hailay   ID: ETS0100/16
 * 6. Bethelhem Degefu   ID: ETS0283/16
 */

#include <iostream>

#include <unordered_map> // For storing user credentials and other mappings
#include <string>
#include <cctype>  // For character checks
#include <iomanip> // For setw
#include <cstdlib> // For random number generation
#include <ctime>   // For seeding the random number generator
// #ifdef _WIN32
// #include <direct.h>  // For _getcwd on Windows
// #include <windows.h> // For LoadLibrary and GetLastError
// #define GetCurrentDir _getcwd
// #else
// #include <unistd.h> // For getcwd on Unix
// #define GetCurrentDir getcwd
// #endif
#include "database.h"

using namespace std;

// Global database object
Database *g_db = nullptr;
const string dbPath = "hospital.db";

// Function declarations
void adminPanel();
void patientPanel();
bool isStrongPassword(const string &password);
bool authenticate(int userId, const string &password);
void registerUser(int userId, const string &name, const string &password, const string &role);
void loginUser();
void registerNewUser();
void patientManagement();
void add();
void displayPatients();
void bookAppointment();
void cancelAppointment();
void displayAppointments();
void registerNewStaff();
void registerStaff(const string &name, const string &role);
void removeStaff();
void staffManagement();
void initialSlots();
int idGenerator();

// Global variables
int choice;                  // Global variable to store user choice
const int displayWidth = 60; // Set display width for the design

// variable for patient management
int num = 0, Gender, n, flag = 1;
string pname[100];
string gender[100];
int pid[100];
int age[100];
float weight[100];
float height[100];
string p_discription[100];
string name;
char x;

// variables for appointment scheduling
const int daysInWeek = 7;
const int appointmentsPerDay = 10;
string slots[daysInWeek][appointmentsPerDay];
const string daysOfWeek[daysInWeek] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
const string times[appointmentsPerDay] = {"09:00 AM", "09:30 AM", "10:00 AM", "10:30 AM", "11:00 AM", "11:30 AM", "12:00 PM", "12:30 PM", "01:00 PM", "01:30 PM"};

// structure for appointment scheduling
struct Appointment
{
    int id;
    string patientName;
};
Appointment appointments[daysInWeek][appointmentsPerDay];

// structure for staff attendance
const int MAX_STAFF = 10;
struct Staff
{
    string name;
    bool isPresent; // true = present, false = absent
};

int main()
{
    // Initialize random seed
    srand(static_cast<unsigned int>(time(nullptr)));

    // Initialize database connection
    g_db = new Database(dbPath);
    if (!g_db->open())
    {
        cerr << "Failed to open database. Exiting..." << endl;
        cout << "Press any key to exit...";
        cin.get();
        delete g_db;
        return 1;
    }
    // cout << "Database opened successfully!" << endl;

    // Initialize appointment slots
    initialSlots();

    // Title
    cout << setfill('=') << setw(displayWidth) << "=" << endl;
    cout << setfill(' ')
         << setw((displayWidth + 24) / 2) << "Hospital Management System"
         << endl;
    cout << setfill('=') << setw(displayWidth) << "=" << endl;

    // Welcome Message
    cout << "\n\n";
    cout << setfill(' ')
         << setw((displayWidth + 29) / 2) << "*******************************"
         << endl;
    cout << setw((displayWidth + 29) / 2) << "* Welcome to Miracle Hospital *"
         << endl;
    cout << setw((displayWidth + 29) / 2) << "*******************************"
         << endl;

    if (!g_db->checkStaff(1))
    {
        cout << "Please register the first admin with ID 1.\n";
        registerNewStaff();
    }
    while (true)
    {

        // Main Menu
        cout << "\n\n";
        cout << setfill('-') << setw(displayWidth) << "-" << endl;
        cout << setfill(' ')
             << setw((displayWidth + 9) / 2) << "Main Menu" << endl;
        cout << setfill('-') << setw(displayWidth) << "-" << endl;
    b:
        cout << "1,Register \n2,Login  \n3,Exit  \nChooste an option: ";

        cin >> choice;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore();
            cout << "\nEnter number only\n";
            goto b;
        }

        if (choice == 1)
        {
            registerNewUser();
        }
        else if (choice == 2)
        {
            loginUser();
        }
        else if (choice == 3)
        {
            cout << "Exiting the system.\n";
            break;
        }
        else
        {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    // Clean up database connection
    if (g_db)
    {
        g_db->close();
        delete g_db;
        g_db = nullptr;
    }

    return 0;
}

// Functions used in appointment scheduling
void initialSlots()
{
    for (int day = 0; day < daysInWeek; ++day)
    {
        for (int slot = 0; slot < appointmentsPerDay; ++slot)
        {
            slots[day][slot] = "Available";
            appointments[day][slot] = {0, ""};
        }
    }
}

// Generates a random ID for the patient between 1-9999
int idGenerator()
{
    return rand() % 10000 + 1;
}

// Functions used in user authentication
//  A map to store user credentials (ID, Password, Name, Role)
unordered_map<int, pair<string, pair<string, string>>> userCredentials; // User ID -> {Name, {Password, Role}}

// Function to check if the password is strong
bool isStrongPassword(const string &password)
{
    if (password.length() < 8)
        return false;
    bool hasLower = false, hasUpper = false, hasDigit = false, hasSpecial = false;

    for (char c : password)
    {
        if (islower(c))
            hasLower = true;
        if (isupper(c))
            hasUpper = true;
        if (isdigit(c))
            hasDigit = true;
        if (!isalnum(c))
            hasSpecial = true;
    }
    return hasLower && hasUpper && hasDigit && hasSpecial;
}

// Function to register a new user
void registerUser(int userId, const string &name, const string &password, const string &role)
{
    if (g_db && g_db->addUser(userId, name, password, role))
    {
        cout << "User " << name << " (" << role << ") registered successfully with ID: " << userId << endl;
    }
    else
    {
        cout << "Failed to register user." << endl;
    }
}

// Function to get user input for registration
void registerNewUser()
{
    string name, password, role;
    int userId;

    cout << "Enter your name: ";
    cin.ignore();
    getline(cin, name);
a:
    cout << "Enter your password: ";
    cin >> password;

    if (!isStrongPassword(password))
    {
        cout << "Password must contain at least 8 characters, including uppercase, lowercase, digit, and special character.\n";
        goto a;
    }

    cout << "Enter your role (Admin/Patient): ";
    cin >> role;

    for (char &c : role)
    {
        c = std::tolower(c);
    } // change to lowercase

    // Make sure the database is open before registering

    if (!g_db || !g_db->open())
    {
        cout << "Error: Database connection failed. Please try again later.\n";
        return;
    }

    if (role == "admin")
    {
        int admin_id;
        cout << "Enter your Admin ID given from your manager: ";
        cin >> admin_id;
        if (g_db->checkStaff(admin_id))
        {
            registerUser(admin_id, name, password, role);
        }
        else
        {
            cout << "Don't try to cheat! If you are staff ask the manager!";
        };
    }
    else
    {
        // Generate a unique 4-digit ID
        userId = rand() % 9000 + 1000;
        registerUser(userId, name, password, role);
    }
}

// Function to authenticate a user
bool authenticate(int userId, const string &password)
{
    // Make sure the database is open
    if (!g_db || !g_db->open())
    {
        cout << "Error: Database connection failed. Please try again later.\n";
        return false;
    }

    return g_db->authenticateUser(userId, password);
}

// Function to login user
void loginUser()
{
    int userId;
    string password;
    string role = "";
b:
    cout << "Enter your User ID: ";
    cin >> userId;
    if (cin.fail())
    {
        cin.clear();
        cin.ignore();
        cout << "\nEnter number only\n";
        goto b;
    }
c:
    cout << "Enter your password: ";
    cin >> password;
    if (cin.fail())
    {
        cin.clear();
        cin.ignore();
        cout << "\nEnter the correct password\n";
        goto c;
    }

    if (authenticate(userId, password))
    {
        cout << "Login successful! Welcome, " << g_db->getUserName(userId) << "!\n";

        string role = g_db->getUserRole(userId);
        if (role == "admin")
        {
        d:
            cout << "Please enter your Admin ID to proceed: ";
            int adminId;
            cin >> adminId;

            if (adminId == userId)
            {
                adminPanel();
            }
            else
            {
                cout << "Invalid Admin ID.\n";
                goto d;
            }
        }
        else
        {
            // Redirect to Patient Panel if the role is not Admin
            patientPanel();
        }
    }
    else
    {
        cout << "Incorrect User ID or password.\n";
    }
}

// Function to handle admin functionality
void adminPanel()
{
    do
    {
        cout << "\n==Welcome to the Admin Panel. (Admin functionality goes here)====";
        cout << "\n\t\t1. Patient Management   \n\t\t2. Staff Attendance   \n\t\t3. Display Booked Appointments  \n\t\t4. Back to main menu";

    a:
        cout << "\n\t\tEnter your choice: ";
        cin >> choice;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore();
            cout << "\nEnter number only\n";
            goto a;
        }
        switch (choice)
        {
        case 1:
            patientManagement();
            break;
        case 2:
            staffManagement();
            break;
        case 3:
            displayAppointments();
            break;
        case 4:
            cout << "Exiting Admin Panel.\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
            break;
        }
    } while (choice != 4);
}

// Sub menu for patient management
void patientManagement()
{
    int choice;
    do
    {
        cout << "\n================= Patient Management Menu =================";
        cout << "\n\t\t1. Add Patients   \n\t\t2. View Patients' Information     \n\t\t3. Back to Admin Portal Menu";
    b:
        cout << "\n\n\t\tEnter your choice: ";
        cin >> choice;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore();
            cout << "\nEnter number only\n";
            goto b;
        }
        switch (choice)
        {
        case 1:
            add();
            break;
        case 2:
            displayPatients();
            break;
        case 3:
            return;
        default:
            cout << "\nInvalid input!"
                 << "\nPlease select the valid option from the given menu.";
            break;
        }
    } while (choice != 5);
}

// Functions used in patient management

// Function to add new patients
void add()
{
b:
    cout << "Please enter the number of patients you want to add:\n";
    cin >> n;
    cin.get();
    if (cin.fail() || n < 0)
    {
        cin.clear();
        cin.ignore();
        cout << "Enter number only" << endl;
        goto b;
    }
    num += n;
    cout << "PLease enter their details:\n";
    for (int i = num - n; i < num; i++)
    {
        cout << "Enter the name of " << "patient " << i + 1 << " : ";

        getline(cin, pname[i]);
    i:
        cout << "Enter gender: \n1. Male\n2. Female" << endl;
        cin >> Gender;
        if (cin.fail() || Gender < 1 || Gender > 2)
        {
            cin.clear();
            cin.ignore();
            cout << "Enter proper input!" << endl;
            goto i;
        }
        if (Gender == 1)
            gender[i] = "Male";
        else
            gender[i] = "Female";
    j:
        cout << "Enter the patient ID: " << endl;
        cin >> pid[i];
        if (cin.fail())
        {
            cin.clear();
            cin.ignore();
            cout << "Enter proper input" << endl;
            goto j;
        }
    l:
        cout << "Enter the Age of the patient: ";
        cin >> age[i];
        if (cin.fail() || age[i] < 0)
        {
            cin.clear();
            cin.ignore();
            cout << "Enter proper input!" << endl;
            goto l;
        }
    m:
        cout << "Enter the Weight of the patient: ";
        cin >> weight[i];
        if (cin.fail() || weight[i] < 0)
        {
            cin.clear();
            cin.ignore();
            cout << "Enter proper input!" << endl;
            goto m;
        }
    n:
        cout << "Enter the Height of the patient: ";
        cin >> height[i];
        if (cin.fail() || height[i] < 0)
        {
            cin.clear();
            cin.ignore();
            cout << "Enter proper input!" << endl;
            goto n;
        }
        cout << "Enter the medical history of the patient: ";
        cin.ignore();
        getline(cin, p_discription[i]);
        cout << "Patient added successfully!\n";
        // Add the patient to the database
        if (!g_db->addPatient(pid[i], pname[i], gender[i], age[i], weight[i], height[i], p_discription[i]))
        {
            cout << "Failed to add patient to the database.\n";
        }
    }
}

// Function to display all patients' information
void displayPatients()
{
    cout << "\n=== Patient Medical Records ===\n";
    cout << left << setw(20) << "Name" << setw(10) << "Gender" << setw(15) << "ID" << setw(10) << "Age" << setw(10) << "Weight" << setw(10) << "Height" << setw(20) << "Medical history" << "\n";
    cout << string(95, '-') << "\n";
    // Fetch all patients from the database and display them
    auto patientCallback = [](void *data, int argc, char **argv, char **azColName) -> int
    {
        if (argc > 0 && argv[0])
        {
            cout << left << setw(20) << argv[0]
                 << setw(10) << argv[1]
                 << setw(15) << argv[2]
                 << setw(10) << argv[3]
                 << setw(10) << fixed << setprecision(1) << argv[4] << setw(10) << fixed << setprecision(2) << argv[5] << setw(20) << argv[6] << "\n"; // Fix width to match header and display data from database columns argv[0] to argv[6] (name, gender, age, weight, height, description) respectively.
        }
        return 0; // Return 0 to indicate successful callback execution
    };
    g_db->getAllPatients(patientCallback); // Call the getAllPatients function with the patientCallback function as the callback parameter
}

// Function to display the staff attendance system
void registerNewStaff()
{
    string name, role;

    cout << "Enter the staff name: ";
    cin.ignore();
    getline(cin, name);
a:
    cout << "Enter the staff role: ";
    cin >> role;

    // Make sure the database is open before registering

    if (!g_db || !g_db->open())
    {

        cout << "Error: Database connection failed. Please try again later.\n";
        return;
    }

    for (char &c : role)
    {
        c = std::tolower(c);
    } // change to lowercase

    if (g_db && g_db->addStaff(name, role))
    {
        cout << "Staff " << name << " registered successfully with role: " << role << " id: " << g_db->getStaffId(name) << endl;
    }
    else
    {
        cout << "Failed to register user." << endl;
    }
}

void removeStaff()
{
    int staff_Id;
    bool found = false;

    cout << "Enter staff id to remove the database: ";
    cin >> staff_Id;

    // Handle non-integer input
    if (cin.fail())
    {
        cin.clear();             // clear error flag
        cin.ignore(10000, '\n'); // discard invalid input
        cout << "Invalid input. Please enter a numeric staff ID.\n";
    }

    if (!g_db || !g_db->open())
    {

        cout << "Error: Database connection failed. Please try again later.\n";
        return;
    }

    if (g_db->checkStaff(staff_Id))
    {
        g_db->deleteStaff(staff_Id);

        cout << "The staff with id: " << staff_Id << " deleted successfuly!" << endl;
        found = true;
    }
    else
    {
        cout << "Incorrect staff id or staff with id ( " << staff_Id << " ) doesn't found!" << endl;
    }

    if (found)
    {
        adminPanel();
    }
    else
    {
        staffManagement();
    }
}

// Sub menu for staff management
void staffManagement()
{

    struct Staff
    {
        int id;
        std::string name;
        bool isPresent;
    };
    bool status;
    do
    {
        cout << setfill('=') << setw(displayWidth) << "=" << endl;
        cout << setfill(' ')
             << setw((displayWidth + 24) / 2) << "Staff Management System"
             << endl;
        cout << setfill('=') << setw(displayWidth) << "=" << endl;

        cout << "\n\t\t1. Add staff  \n\t\t2. Remove staff   \n\t\t3. Back to Admin Portal Menu \n\t\tEnter your choice: ";
    a:
        cin >> choice;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore();
            cout << "\nEnter number only\n";
            goto a;
        }
        switch (choice)
        {
        case 1:
            registerNewStaff();
            break;
        case 2:
            removeStaff();
            break;
        case 3:
            adminPanel();
            break;
        default:
            cout << "\nInvalid choice. Please try again.\n";
            break;
        }
    } while (choice != 4);
}

// Function to display all booked appointments for the admin
void displayBookedAppointments()
{
    cout << "\n=============Booked Appointments=============\n\n";
    bool found = false;

    for (int day = 0; day < daysInWeek; ++day)
    {
        for (int slot = 0; slot < appointmentsPerDay; ++slot)
        {
            if (slots[day][slot] != "Available") // If the slot is booked
            {
                found = true;
                cout << "\t\tDay: " << daysOfWeek[day] << "\n";
                cout << "\t\tTime: " << times[slot] << "\n";
                cout << "\t\tAppointment ID: " << appointments[day][slot].id << "\n\n";
            }
        }
    }

    if (!found)
    {
        cout << "No appointments are booked yet.\n";
    }
}

// Function to handle patient functionality
void patientPanel()
{
    do
    {
        cout << setfill('=') << setw(displayWidth) << "=" << endl;
        cout << setfill(' ')
             << setw((displayWidth) / 2) << "Welcome to the Patient Portal! (Patient functionality goes here)"
             << endl;
        cout << setfill('=') << setw(displayWidth) << "=" << endl;
        cout << "\n\t\t1. Book an Appointment   \n\t\t2. Cancel an Appointment  \n\t\t3. Back to main menu";
        cin.ignore();

    a:
        cout << "\n\n\t\tEnter your choice: ";
        cin >> choice;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore();
            cout << "\nEnter number only\n";
            goto a;
        }
        switch (choice)
        {
        case 1:
            bookAppointment();
            break;
        case 2:
            cancelAppointment();
            break;
        case 3:
            cout << "Exiting patient portal";
            return;
        default:
            cout << "\n\t\t\tInvalid input!"
                 << "\n\t\t\tPlease select the valid option from the given menu.";
            break;
        }
    } while (choice != 3);
}

// Function that display available appointments for booking
void displayAppointments()
{
    cout << "\n====================Appointment Display=====================\n\n";

    // Get all appointments from database
    auto appointments = g_db->getAllAppointments();

    if (appointments.empty())
    {
        cout << "No booked appointments.\n";
    }
    else
    {
        cout << "Booked appointments:\n";
        for (const auto &appt : appointments)
        {
            cout << "ID: " << std::get<0>(appt) << ", Date: " << std::get<1>(appt) << "\n";
        }

        cout << "\nTo book an appointment, please select an available date and time.\n";
    }
}

// Function to book an appointment
void bookAppointment()
{
    int dayChoice, timeChoice;
    string appointmentDate;

    cout << "\n====================Appointment Booking====================\n\n";

    while (true)
    {
        cout << "Available days:\n";
        for (int i = 0; i < daysInWeek; ++i)
        {
            cout << i + 1 << ". " << daysOfWeek[i] << "\n";
        }

        cout << "Enter the number of the day you want to book: ";
        cin >> dayChoice;

        if (dayChoice < 1 || dayChoice > daysInWeek)
        {
            cout << "Invalid day choice. Please select a valid day (1-7).\n";
            continue;
        }

        cout << "\nAvailable appointment times on " << daysOfWeek[dayChoice - 1] << ":\n";
        for (int i = 0; i < appointmentsPerDay; ++i)
        {
            cout << i + 1 << ". " << times[i] << "\n";
        }

        cout << "Enter the number of the time slot you want to book: ";
        cin >> timeChoice;

        if (timeChoice < 1 || timeChoice > appointmentsPerDay)
        {
            cout << "Invalid time slot choice. Please select a valid time slot.\n";
            continue;
        }

        // Format the appointment date as "Day Time"
        appointmentDate = daysOfWeek[dayChoice - 1] + " " + times[timeChoice - 1];

        // Check if this appointment slot is available
        if (!g_db->isAppointmentAvailable(appointmentDate))
        {
            cout << "Appointment already booked. Please choose a different time.\n";
            continue;
        }

        // Generate a unique appointment ID
        int appointmentID = idGenerator();

        // Book the appointment in the database
        if (g_db->bookAppointment(appointmentID, appointmentDate))
        {
            cout << "Appointment booked successfully for " << appointmentDate << "\n";
            cout << "Your Appointment ID is: " << appointmentID << ". Please do not lose or share your ID with others for private information.\n";
        }
        else
        {
            cout << "Failed to book appointment. Please try again.\n";
        }

        break;
    }
}

// Function to cancel an appointment
void cancelAppointment()
{
    int idToCancelAppointment;

    cout << "\n===================Appointment Cancellation==================\n\n";
i:
    cout << "Enter your Appointment ID to cancel: ";
    cin >> idToCancelAppointment;

    if (cin.fail())
    {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input. Please enter a numeric appointment ID.\n";
        goto i;
    }

    // Try to cancel the appointment in the database
    if (g_db->cancelAppointment(idToCancelAppointment))
    {
        cout << "Appointment canceled successfully.\n";
        patientPanel();
    }
    else
    {
        cout << "Failed to cancel appointment. ID may not exist.\n";
        goto i;
    }
}
