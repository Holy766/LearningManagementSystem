
#include <iostream>
#include <limits>
#include <string>

#include "database.h"
#include "sqlite_user_repository.h"
#include "admin_setup.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;

void studentMenu() {
    int choice;
    do {
        cout << "\n===== Student Menu =====\n";
        cout << "1. View Assignments\n";
        cout << "2. Submit Work\n";
        cout << "3. Resubmit Work\n";
        cout << "4. View Released Marks and Feedback\n";
        cout << "0. Logout\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: cout << "View assignments selected.\n"; break;
            case 2: cout << "Submit work selected.\n"; break;
            case 3: cout << "Resubmit work selected.\n"; break;
            case 4: cout << "View released marks selected.\n"; break;
            case 0: break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

void lecturerMenu() {
    int choice;
    do {
        cout << "\n===== Lecturer Menu =====\n";
        cout << "1. Create Assignment\n";
        cout << "2. Set Assignment Deadline\n";
        cout << "3. Grant Student Extension\n";
        cout << "4. View Submissions\n";
        cout << "5. Mark Submission and Give Feedback\n";
        cout << "0. Logout\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: cout << "Create assignment selected.\n"; break;
            case 2: cout << "Set deadline selected.\n"; break;
            case 3: cout << "Grant extension selected.\n"; break;
            case 4: cout << "View submissions selected.\n"; break;
            case 5: cout << "Mark submission selected.\n"; break;
            case 0: break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

void hodMenu() {
    int choice;
    do {
        cout << "\n===== Head of Department Menu =====\n";
        cout << "1. View Marked Submissions\n";
        cout << "2. Moderate Marks\n";
        cout << "3. Release Marks\n";
        cout << "4. View Mark Audit Records\n";
        cout << "0. Logout\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: cout << "View marked submissions selected.\n"; break;
            case 2: cout << "Moderate marks selected.\n"; break;
            case 3: cout << "Release marks selected.\n"; break;
            case 4: cout << "View audit records selected.\n"; break;
            case 0: break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

void administratorMenu(SQLiteUserRepository& repository) {
    int choice;

    do {
        cout << "\n===== Administrator Menu =====\n";
        cout << "1. Register User\n";
        cout << "2. Manage Departments\n";
        cout << "3. Manage Courses\n";
        cout << "4. Assign Lecturers to Courses\n";
        cout << "5. Enrol Students in Courses\n";
        cout << "0. Logout\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: {
                int id;
                int roleChoice;
                string username, password, role;

                cout << "New user ID: ";
                if (!(cin >> id) || id <= 0) {
                    cin.clear();
                    cin.ignore(
                        std::numeric_limits<std::streamsize>::max(), '\n'
                    );
                    cout << "Invalid ID.\n";
                    break;
                }

                cout << "Choose role:\n";
                cout << "1. Student\n2. Lecturer\n";
                cout << "3. HoD\n4. Administrator\nChoice: ";

                if (!(cin >> roleChoice)) {
                    cin.clear();
                    cin.ignore(
                        std::numeric_limits<std::streamsize>::max(), '\n'
                    );
                    cout << "Invalid role.\n";
                    break;
                }

                switch (roleChoice) {
                    case 1: role = "Student"; break;
                    case 2: role = "Lecturer"; break;
                    case 3: role = "HoD"; break;
                    case 4: role = "Administrator"; break;
                    default:
                        cout << "Invalid role.\n";
                        continue;
                }

                cout << "New username: ";
                cin >> username;
                if (repository.userIdExists(id)) {
                cout << "That user ID already exists.\n";
                break;
             }

               if (repository.usernameExists(username)) {
              cout << "That username already exists.\n";
              break;
              }
                
                cout << "New password: ";
                cin >> password;

               if (password.empty()) {
               cout << "Password cannot be empty.\n";
               break;
              }
                

                if (repository.saveUser(id, username, password, role)) {
                    cout << "User registered successfully.\n";
                } else {
                    cout << "Registration failed. Check the ID and details.\n";
                }
                break;
            }

            case 2: cout << "Manage departments selected.\n"; break;
            case 3: cout << "Manage courses selected.\n"; break;
            case 4: cout << "Assign lecturers selected.\n"; break;
            case 5: cout << "Enrol students selected.\n"; break;
            case 0: break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

int main() {
    try {
        SQLiteConnection connection("lms.db");
        SQLiteUserRepository repository(connection);
        repository.createTable();

        setupInitialAdministrator(repository); 

        int choice;

        do {
            cout << "\n===== Learning Management System =====\n";
            cout << "1. Login\n";
            cout << "0. Exit\nChoice: ";

            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n'
                );
                choice = -1;
            }

            if (choice == 1) {
                string username, password;

                cout << "Username: ";
                cin >> username;

                cout << "Password: ";
                cin >> password;

                auto role = repository.authenticateRole(username, password);

                if (!role.has_value()) {
                    cout << "Login failed. Check your credentials.\n";
                    continue;
                }

                cout << "Login successful. Role: " << *role << '\n';

                if (*role == "Student") {
                    studentMenu();
                } else if (*role == "Lecturer") {
                    lecturerMenu();
                } else if (*role == "HoD") {
                    hodMenu();
                } else if (*role == "Administrator") {
                    administratorMenu(repository);
                }
            } else if (choice == 0) {
                cout << "Goodbye!\n";
            } else {
                cout << "Invalid choice.\n";
            }

        } while (choice != 0);

    } catch (const std::exception& error) {
        cout << "Database error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
