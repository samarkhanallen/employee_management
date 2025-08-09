# employee_management
The Employee Management System is a C++ console-based program that helps store, view, search, update, and delete employee records.

Employee Management System (C++)
Overview
This Employee Management System is a console-based application written in C++ that allows the user to manage employee records efficiently.
The program uses file handling for persistent data storage and supports input validation to avoid incorrect entries.

It is a complete implementation — not just a basic skeleton — and includes:

Adding new employees

Viewing all employees

Searching by Employee ID

Updating employee details

Deleting an employee record

Automatic data saving to file

Project Structure
bash
Copy
Edit
employee-management/
│
├── employee_management.cpp   # Main C++ source code
├── employees.txt             # Employee records (auto-created)
└── README.md                 # Project documentation
⚙️ Features
✅ Add Employee — Stores Name, ID, Salary, Bonus, In-time, and Out-time.
✅ View All Employees — Displays all stored records in a table format.
✅ Search Employee — Look up by Employee ID.
✅ Update Employee — Modify any employee detail.
✅ Delete Employee — Remove a record by ID.
✅ File Handling — Data saved permanently to employees.txt.
✅ Input Validation — Prevents empty names, invalid salary/bonus values, or duplicate IDs.
