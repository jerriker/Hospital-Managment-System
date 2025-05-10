/**Project title: Hospital management system
 * These project is done by the following group members with collaboration using github:
 * 1. Bereket Desalegn   ID: ETS0251/16
 * 2. Bekam Yoseph       ID: ETS0240/16
 * 3. Befiker Kassahun   ID: ETS0236/16
 * 4. Barok Yeshiber     ID: ETS0224/16
 * 5. Bethelhem Degefu   ID: ETS0283/16
 */
#include <iostream>
#include <unordered_map> // For storing user credentials and other mappings
#include <string>
#include <cctype>  // For character checks
#include <iomanip> // For setw
#include <cstdlib> // For random number generation
#include <ctime>   // For seeding the random number generator

using namespace std;

// Displays the admin panel interface.
void adminPanel();

// Displays the patient panel interface.
void patientPanel();

// Functions used in user authentication
bool isStrongPassword(const string &password);
bool authenticate(int userId, const string &password);
void registerUser(int userId, const string &name, const string &password, const string &role);
void loginUser();
void registerNewUser();

// Functions used in patient management
void patientManagement();
void add();             // Add new patients
void updatePatient();   // Update patient information
void displayPatients(); // Display all patients' information
void search();          // To display the patient's information by searching its name

// Functions used in appiontment scheduling
void bookAppointment();           // Book an appiontment by entering patient name and slot number
void cancelAppointment();         // Cancel an appointment by clearing slots
void displayAppointments();       // Display all appointment slots and their status (booked or available).
void displayBookedAppointments(); // Display all booked appointments for the admin

// Functions used in staff management
void staffManagement();
void markAttendance(struct Staff staffList[], int size, const string &name, bool status);
void displayAttendance(const Staff staffList[], int size);

    // variables
int choice;              // Global variable to store user choice
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
// const and structure for appointment scheduling
struct Appointment
{
    int id;
    string patientName;
};
Appointment appointments[daysInWeek][appointmentsPerDay];

// const and structure for staff attendance
const int MAX_STAFF = 10;

struct Staff
{
    string name;
    bool isPresent; // true = present, false = absent
};

// Functions used in appiontment scheduling
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
// A map to store user credentials (ID, Password, Name, Role)
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
    userCredentials[userId] = make_pair(name, make_pair(password, role));
    cout << "User " << name << " (" << role << ") registered successfully with ID: " << userId << endl;
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

    do
    {
        // use idGenerator function
        userId = rand() % 9000 + 1000; // Generate a unique 4-digit ID
    } while (userCredentials.count(userId));

    registerUser(userId, name, password, role);
}

// Function to authenticate a user
bool authenticate(int userId, const string &password)
{
    return userCredentials.count(userId) && userCredentials[userId].second.first == password;
}

// Function to login user
void loginUser()
{
    // why not we use this variable globally
    int userId;
    string password;
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
        string role = userCredentials[userId].second.second;
        cout << "Login successful! Welcome, " << userCredentials[userId].first << "!\n";

        if (role == "Admin")
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
            displayBookedAppointments();
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
        cout << "\n\t\t1. Add Patients   \n\t\t2. Update Patients' Information   \n\t\t3. View Patients' Information   \n\t\t4. Search for a patient  \n\t\t5. Back to Admin Portal Menu";
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
            updatePatient();
            break;
        case 3:
            displayPatients();
            break;
        case 4:
            search();
            break;
        case 5:
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
    }
}

// updating the patients data
void updatePatient()
{
    int num2;
a:
    cout << "Enter the number of patients you want to update\n";
    cin >> num2;
    cin.get();
    if (cin.fail() || num2 <= 0)
    {
        cout << "Please enter a valid number!\n";
        goto a;
    }
    for (int k = 0; k < num2; k++)
    {
    d:
        cout << "Enter the name of patient " << k + 1 << " to update: ";
        string name;
        getline(cin, name);

        bool found = false;
        for (int i = 0; i < num; i++)
        {
            if (name == pname[i])
            {
                found = true;
                cout << "Patient found. Current details:\n";
                cout << left << setw(20) << "Name"
                     << setw(10) << "Gender"
                     << setw(15) << "ID"
                     << setw(10) << "Age"
                     << setw(10) << "Weight"
                     << setw(10) << "Height"
                     << setw(20) << "Medical History" << "\n";
                cout << string(95, '-') << "\n";
                cout << left << setw(20) << pname[i]
                     << setw(10) << gender[i]
                     << setw(15) << pid[i]
                     << setw(10) << age[i]
                     << setw(10) << fixed << setprecision(1) << weight[i]
                     << setw(10) << fixed << setprecision(2) << height[i]
                     << setw(20) << p_discription[i] << "\n";

                // Updating details
                cout << "Enter new details for the patient (leave blank to retain current value):\n";

                cout << "Name [" << pname[i] << "]: ";
                string newName;
                getline(cin, newName);
                if (!newName.empty())
                    pname[i] = newName;

                cout << "Gender (1. Male, 2. Female) [" << gender[i] << "]: ";
                int newGender;
                cin >> newGender;
                cin.ignore(); // Clear input buffer
                if (newGender == 1)
                    gender[i] = "Male";
                else if (newGender == 2)
                    gender[i] = "Female";

                cout << "ID [" << pid[i] << "]: ";
                string newID;
                getline(cin, newID);
                if (!newID.empty())
                    pid[i] = stoi(newID);

                cout << "Age [" << age[i] << "]: ";
                int newAge;
                cin >> newAge;
                cin.ignore();
                if (newAge > 0)
                    age[i] = newAge;

                cout << "Weight [" << weight[i] << "]: ";
                double newWeight;
                cin >> newWeight;
                cin.ignore();
                if (newWeight > 0)
                    weight[i] = newWeight;

                cout << "Height [" << height[i] << "]: ";
                double newHeight;
                cin >> newHeight;
                cin.ignore();
                if (newHeight > 0)
                    height[i] = newHeight;

                cout << "Medical History [" << p_discription[i] << "]: ";
                string newHistory;
                getline(cin, newHistory);
                if (!newHistory.empty())
                    p_discription[i] = newHistory;

                cout << "Patient record updated successfully!\n";
                break;
            }
        }

        if (!found)
        {
            cout << "Patient with the name \"" << name << "\" not found.\n";
            goto d;
        }
    }
}

// Function to display all patients' information
void displayPatients()
{
    cout << "\n================= Patient Medical Records ================\n";
    cout << left << setw(20) << " Name" << setw(10) << "Gender" << setw(15) << "ID" << setw(10) << "Age" << setw(10) << "Weight" << setw(10) << "Height" << setw(20) << "Medical history" << "\n";
    cout << string(95, '-') << "\n";

    for (int i = 0; i < num; i++)
    {
        cout << left << setw(20) << pname[i]
             << setw(10) << gender[i]
             << setw(15) << pid[i]
             << setw(10) << age[i] << setw(10) << setprecision(2) << weight[i] << setw(10) << setprecision(2) << height[i] << setw(20) << fixed << p_discription[i] << "\n";
    }
}

// Function to search for a patient
void search()
{
    cout << "Enter the name of the patient: ";
    getline(cin, name);
    cin.ignore();
    for (int i = 0; i < num; i++)
    {
        if (name == pname[i])
        {
            cout << "\n=== Patient Medical Records ===\n";
            cout << left << setw(20) << "Name" << setw(10) << "Gender" << setw(20) << "ID" << setw(10) << "Age" << setw(10) << "Weight" << setw(10) << "Height" << setw(20) << "Medical history" << "\n";
            cout << string(95, '-') << "\n";
            cout << left << setw(20) << pname[i]
                 << setw(10) << gender[i]
                 << setw(20) << pid[i]
                 << setw(10) << age[i] << setw(10) << setprecision(2) << weight[i] << setw(10) << setprecision(2) << height[i] << setw(20) << fixed << p_discription[i] << "\n";
            break;
        }
    }
}

// Function to display the staff attendance system

// Sub menu for staff management
void staffManagement()
{
    Staff staffList[MAX_STAFF] = {
        {"Aschalew", false}, {"Emebet", false}, {"Solomon", false}, {"Ahmed", false}, {"Hanan", false}, {"Askalech", false}, {"Tsion", false}, {"Mulugeta", false}, {"Alemayew", false}, {"Abebe", false}};
    bool status;
    do
    {
        cout << setfill('=') << setw(displayWidth) << "=" << endl;
        cout << setfill(' ')
             << setw((displayWidth + 24) / 2) << "Staff Management System"
             << endl;
        cout << setfill('=') << setw(displayWidth) << "=" << endl;

        cout << "\n\t\t1. Mark Attendance   \n\t\t2. Display Attendance   \n\t\t3. Back to Admin Portal Menu \n\t\tEnter your choice: ";
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
            cout << "Enter staff name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Enter status (1 for Present, 0 for Absent): ";
            cin >> status;
            markAttendance(staffList, MAX_STAFF, name, status);
            break;
        case 2:
            displayAttendance(staffList, MAX_STAFF);
            break;
        case 3:
            return;
        default:
            cout << "\nInvalid choice. Please try again.\n";
            break;
        }
    } while (choice != 4);
}

// Function to manage attendance
void markAttendance(Staff staffList[], int size, const string &name, bool status)
{
    if (cin.fail())
    {
        cin.clear();
        cin.ignore();
    }
    for (int i = 0; i < size; ++i)
    {
        if (staffList[i].name == name)
        {
            staffList[i].isPresent = status;
            cout << "\n\t" << name << " marked as " << (status ? "Present" : "Absent") << "." << endl;
            return;
        }
    }
    cout << "\n\tStaff member " << name << " not found!" << endl;
}

// Function to display attendance
void displayAttendance(const Staff staffList[], int size)
{
    cout << "\nAttendance Status:\n";
    for (int i = 0; i < size; ++i)
    {
        cout << "\t\tName: " << staffList[i].name
             << ", Status: " << (staffList[i].isPresent ? "Present" : "Absent") << endl;
    }
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
        cout << "\n\t\t1. Display available Appointments  \n\t\t2. Book an Appointment   \n\t\t3. Cancel an Appointment  \n\t\t4. Back to main menu";
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
            displayAppointments();
            break;
        case 2:
            bookAppointment();
            break;
        case 3:
            cancelAppointment();
            break;
        case 4:
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
    for (int day = 0; day < daysInWeek; ++day)
    {
        cout << setfill('=') << setw(displayWidth) << endl;
        cout << daysOfWeek[day] << ":\n";
        for (int slot = 0; slot < appointmentsPerDay; ++slot)
        {
            cout << "  " << times[slot] << ": " << (slots[day][slot] == "Available" ? "Available" : "Booked") << "\n";
        }
    }
}

// Function to book an appointment
void bookAppointment()
{
    int dayChoice, timeChoice;

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

        bool slotsAvailable = false;
        for (int i = 0; i < appointmentsPerDay; ++i)
        {
            if (slots[dayChoice - 1][i] == "Available")
            {
                slotsAvailable = true;
                cout << i + 1 << ". " << times[i] << "\n";
            }
        }

        if (!slotsAvailable)
        {
            cout << "No available time slots for " << daysOfWeek[dayChoice - 1] << ".\n";
            continue;
        }

        cout << "Enter the number of the time slot you want to book: ";
        cin >> timeChoice;

        if (timeChoice < 1 || timeChoice > appointmentsPerDay)
        {
            cout << "Invalid time slot choice. Please select a valid time slot.\n";
            continue;
        }

        if (slots[dayChoice - 1][timeChoice - 1] != "Available")
        {
            cout << "Appointment already booked. Please choose a different time.\n";
            continue;
        }

        string patientName;
        getline(cin, patientName);
        cin.ignore();

        int appointmentID = idGenerator(); // how to use this function

        slots[dayChoice - 1][timeChoice - 1] = patientName;
        appointments[dayChoice - 1][timeChoice - 1] = {appointmentID, patientName};

        cout << "Appointment booked successfully for " << daysOfWeek[dayChoice - 1] << " at " << times[timeChoice - 1] << "\n";
        cout << "Your Appointment ID is: " << appointmentID << ". Please do not lose or share your ID with others for private information.\n";

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

    bool found = false;

    for (int day = 0; day < daysInWeek; ++day)
    {
        for (int slot = 0; slot < appointmentsPerDay; ++slot)
        {
            if (appointments[day][slot].id == idToCancelAppointment)
            {
                found = true;
                cout << "Appointment found on " << daysOfWeek[day] << " at " << times[slot] << ".\n";
                cout << "Canceling your appointment...\n";

                slots[day][slot] = "Available";
                appointments[day][slot] = {0, ""}; // Reset appointment details
                cout << "Appointment canceled successfully.\n";
                break;
            }
        }
        if (found)
            break;
    }

    if (!found)
    {
        cout << "Appointment ID not found. Please check your ID and try again.\n";
        goto i;
    }
}

int main()
{

    srand(static_cast<unsigned int>(time(0))); // Seed for random number generation

    initialSlots(); // Initialize appointment slots

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

    return 0;
}