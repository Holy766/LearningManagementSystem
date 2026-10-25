
#include "admin_setup.h"

#include <iostream>
#include <string>

using std::cin;
using std::cout;
using std::string;


void setupInitialAdministrator(SQLiteUserRepository& repository) {
    if (repository.administratorExists()) {
        return;
    }

    cout << "\n===== Initial Administrator Setup =====\n";

    int id;
    string username;
    string password;

    cout << "Enter administrator ID: ";
    if (!(cin >> id) || id <= 0) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid ID.\n";
        return;
    }

    if (repository.userIdExists(id)) {
        cout << "That user ID already exists.\n";
        return;
    }

    cout << "Choose administrator username: ";
    cin >> username;

    if (repository.usernameExists(username)) {
        cout << "That username already exists.\n";
        return;
    }

    cout << "Choose administrator password: ";
    cin >> password;

    if (repository.saveUser(id, username, password, "Administrator")) {
        cout << "Initial administrator created successfully.\n";
    } else {
        cout << "Administrator creation failed. Check the ID and details.\n";
    }
}
