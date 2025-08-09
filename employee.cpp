#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>
#include <regex>
#include <sstream>
using namespace std;

struct Employee {
    int id;
    string name;
    double salary;
    double bonus;
    string inTime;   // "HH:MM"
    string outTime;  // "HH:MM"

    double totalSalary() const {
        return salary + bonus;
    }
};

// filename for binary persistence
const string filename = "employees.dat";

// prototypes
void addEmployee();
void displayAll();
void searchEmployee();
void updateEmployee();
void deleteEmployee();
vector<Employee> loadEmployees();
void saveEmployees(const vector<Employee>& employees);

int getValidatedInt(const string& prompt, int min = numeric_limits<int>::min(), int max = numeric_limits<int>::max());
double getValidatedDouble(const string& prompt, double min = numeric_limits<double>::lowest(), double max = numeric_limits<double>::max());
string getValidatedString(const string& prompt);
string getValidatedTime(const string& prompt);

int minutesBetween(const string& inTime, const string& outTime);
string formatMinutesToHHMM(int minutes);
double minutesToDecimalHours(int minutes);

int main() {
    int choice;
    do {
        cout << "\n====== Employee Management System ======\n";
        cout << "1. Add Employee\n";
        cout << "2. Display All Employees\n";
        cout << "3. Search Employee by ID\n";
        cout << "4. Update Employee\n";
        cout << "5. Delete Employee\n";
        cout << "6. Exit\n";
        choice = getValidatedInt("Enter your choice: ", 1, 6);

        switch(choice) {
            case 1: addEmployee(); break;
            case 2: displayAll(); break;
            case 3: searchEmployee(); break;
            case 4: updateEmployee(); break;
            case 5: deleteEmployee(); break;
            case 6: cout << "Exiting program...\n"; break;
        }
    } while(choice != 6);

    return 0;
}

// ---------- Core features ----------

void addEmployee() {
    Employee emp;
    cout << "\nAdd New Employee:\n";

    emp.id = getValidatedInt("Enter Employee ID (positive integer): ", 1);

    vector<Employee> employees = loadEmployees();
    for (const auto& e : employees) {
        if (e.id == emp.id) {
            cout << "Employee with ID " << emp.id << " already exists. Cannot add duplicate.\n";
            return;
        }
    }

    emp.name = getValidatedString("Enter Name (non-empty): ");
    emp.salary = getValidatedDouble("Enter Salary (>= 0): ", 0);
    emp.bonus = getValidatedDouble("Enter Bonus (>= 0): ", 0);

    // Get in/out times and ensure logical (we allow overnight by design)
    emp.inTime = getValidatedTime("Enter In-Time (HH:MM 24-hour format): ");
    emp.outTime = getValidatedTime("Enter Out-Time (HH:MM 24-hour format): ");

    employees.push_back(emp);
    saveEmployees(employees);

    cout << "Employee added successfully.\n";
}

void displayAll() {
    vector<Employee> employees = loadEmployees();
    if (employees.empty()) {
        cout << "\nNo employees to display.\n";
        return;
    }

    cout << "\nAll Employees:\n";
    cout << left << setw(6) << "ID"
         << setw(20) << "Name"
         << setw(12) << "Salary"
         << setw(10) << "Bonus"
         << setw(8)  << "In"
         << setw(8)  << "Out"
         << setw(10) << "Worked(HH:MM)"
         << setw(10) << "HoursDec"
         << setw(12) << "TotalSalary" << endl;

    cout << string(96, '-') << endl;

    for (const auto& emp : employees) {
        int mins = minutesBetween(emp.inTime, emp.outTime);
        string hhmm = formatMinutesToHHMM(mins);
        double decHours = minutesToDecimalHours(mins);

        cout << left << setw(6) << emp.id
             << setw(20) << emp.name
             << setw(12) << fixed << setprecision(2) << emp.salary
             << setw(10) << emp.bonus
             << setw(8)  << emp.inTime
             << setw(8)  << emp.outTime
             << setw(10) << hhmm
             << setw(10) << fixed << setprecision(2) << decHours
             << setw(12) << emp.totalSalary() << endl;
    }
}

void searchEmployee() {
    int id = getValidatedInt("\nEnter Employee ID to search: ", 1);

    vector<Employee> employees = loadEmployees();
    bool found = false;
    for (const auto& emp : employees) {
        if (emp.id == id) {
            int mins = minutesBetween(emp.inTime, emp.outTime);
            string hhmm = formatMinutesToHHMM(mins);
            double decHours = minutesToDecimalHours(mins);

            cout << "\nEmployee Found:\n";
            cout << "ID: " << emp.id << "\nName: " << emp.name
                 << "\nSalary: " << emp.salary << "\nBonus: " << emp.bonus
                 << "\nIn-Time: " << emp.inTime << "\nOut-Time: " << emp.outTime
                 << "\nWorked (HH:MM): " << hhmm
                 << "\nWorked (decimal hours): " << fixed << setprecision(2) << decHours
                 << "\nTotal Salary: " << emp.totalSalary() << endl;
            found = true;
            break;
        }
    }
    if (!found)
        cout << "Employee with ID " << id << " not found.\n";
}

void updateEmployee() {
    int id = getValidatedInt("\nEnter Employee ID to update: ", 1);

    vector<Employee> employees = loadEmployees();
    bool found = false;

    for (auto& emp : employees) {
        if (emp.id == id) {
            cout << "Editing Employee: " << emp.name << "\n";
            cout << "Press enter without typing anything to keep the current value.\n";

            string temp;

            cout << "Current Name: " << emp.name << "\nNew Name: ";
            getline(cin, temp);
            if (!temp.empty()) emp.name = temp;

            cout << "Current Salary: " << emp.salary << "\nNew Salary (or blank to keep): ";
            getline(cin, temp);
            if (!temp.empty()) {
                try {
                    double val = stod(temp);
                    if (val >= 0) emp.salary = val;
                    else cout << "Invalid salary. Keeping old value.\n";
                } catch (...) { cout << "Invalid input. Keeping old value.\n"; }
            }

            cout << "Current Bonus: " << emp.bonus << "\nNew Bonus (or blank to keep): ";
            getline(cin, temp);
            if (!temp.empty()) {
                try {
                    double val = stod(temp);
                    if (val >= 0) emp.bonus = val;
                    else cout << "Invalid bonus. Keeping old value.\n";
                } catch (...) { cout << "Invalid input. Keeping old value.\n"; }
            }

            cout << "Current In-Time: " << emp.inTime << "\nNew In-Time (HH:MM or blank to keep): ";
            getline(cin, temp);
            if (!temp.empty()) {
                regex timeFormat("^([01]\\d|2[0-3]):([0-5]\\d)$");
                if (regex_match(temp, timeFormat)) emp.inTime = temp;
                else cout << "Invalid time format. Keeping old value.\n";
            }

            cout << "Current Out-Time: " << emp.outTime << "\nNew Out-Time (HH:MM or blank to keep): ";
            getline(cin, temp);
            if (!temp.empty()) {
                regex timeFormat("^([01]\\d|2[0-3]):([0-5]\\d)$");
                if (regex_match(temp, timeFormat)) emp.outTime = temp;
                else cout << "Invalid time format. Keeping old value.\n";
            }

            saveEmployees(employees);
            cout << "Employee updated successfully.\n";
            found = true;
            break;
        }
    }
    if (!found) cout << "Employee with ID " << id << " not found.\n";
}

void deleteEmployee() {
    int id = getValidatedInt("\nEnter Employee ID to delete: ", 1);

    vector<Employee> employees = loadEmployees();
    bool found = false;

    for (auto it = employees.begin(); it != employees.end(); ++it) {
        if (it->id == id) {
            employees.erase(it);
            saveEmployees(employees);
            cout << "Employee deleted successfully.\n";
            found = true;
            break;
        }
    }
    if (!found) cout << "Employee with ID " << id << " not found.\n";
}

// ---------- File I/O ----------

vector<Employee> loadEmployees() {
    vector<Employee> employees;
    ifstream fin(filename, ios::binary);
    if (!fin) return employees;

    Employee emp;
    while (fin.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
        employees.push_back(emp);
    }
    fin.close();
    return employees;
}

void saveEmployees(const vector<Employee>& employees) {
    ofstream fout(filename, ios::binary | ios::trunc);
    for (const auto& emp : employees) {
        fout.write(reinterpret_cast<const char*>(&emp), sizeof(emp));
    }
    fout.close();
}

// ---------- Input validation helpers ----------

int getValidatedInt(const string& prompt, int min, int max) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (value >= min && value <= max) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            } else {
                cout << "Input must be between " << min << " and " << max << ". Try again.\n";
            }
        } else {
            cout << "Invalid input. Please enter a valid integer.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

double getValidatedDouble(const string& prompt, double min, double max) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (value >= min && value <= max) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            } else {
                cout << "Input must be between " << min << " and " << max << ". Try again.\n";
            }
        } else {
            cout << "Invalid input. Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

string getValidatedString(const string& prompt) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        if (!input.empty()) return input;
        cout << "Input cannot be empty. Try again.\n";
    }
}

string getValidatedTime(const string& prompt) {
    string time;
    regex timeFormat("^([01]\\d|2[0-3]):([0-5]\\d)$");
    while (true) {
        cout << prompt;
        getline(cin, time);
        if (regex_match(time, timeFormat)) return time;
        cout << "Invalid time format. Please enter in HH:MM 24-hour format (e.g., 09:30).\n";
    }
}

// ---------- Time utilities ----------

// returns minutes worked between inTime and outTime.
// handles overnight shifts: if out < in, counts as next day
int minutesBetween(const string& inTime, const string& outTime) {
    auto parse = [](const string& t) -> pair<int,int> {
        int hh = 0, mm = 0;
        char colon;
        istringstream ss(t);
        ss >> hh >> colon >> mm;
        return {hh, mm};
    };

    pair<int,int> a = parse(inTime);
    pair<int,int> b = parse(outTime);

    int minsIn = a.first * 60 + a.second;
    int minsOut = b.first * 60 + b.second;

    int diff = minsOut - minsIn;
    if (diff < 0) diff += 24 * 60; // overnight shift

    return diff;
}

string formatMinutesToHHMM(int minutes) {
    int hh = minutes / 60;
    int mm = minutes % 60;
    // allow hours > 24 for clarity (though with single day this won't exceed 24)
    ostringstream out;
    out << hh << ":" << setw(2) << setfill('0') << mm;
    return out.str();
}

double minutesToDecimalHours(int minutes) {
    return minutes / 60.0;
}
