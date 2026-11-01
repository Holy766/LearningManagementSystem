#include <iostream>
#include <stdexcept>
#include <vector>

#include "database.h"
#include "sqlite_extension_repository.h"

namespace {
void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

Datetime revisedDeadline() {
    Datetime deadline;
    deadline.setday(20);
    deadline.setmonth(11);
    deadline.setyear(2026);
    deadline.sethours(23);
    deadline.setminutes(59);
    deadline.setseconds(0);
    return deadline;
}

void testClassExtensionSavedForEveryStudent() {
    SQLiteConnection connection(":memory:");
    SQLiteExtensionRepository repository(connection);
    repository.initializeTable();

    const std::vector<UserId> students{101, 102, 103};
    require(repository.saveForStudents(
        501, students, revisedDeadline(), "School-approved class extension", 900
    ), "Class extension should be saved.");

    for (UserId studentId : students) {
        const auto extensions = repository.findByAssignmentAndStudent(501, studentId);
        require(extensions.size() == 1, "Each student should have one extension.");
        Datetime storedDeadline = extensions.front().getRevisedDeadline();
        require(storedDeadline.getday() == 20,
                "The revised day should be stored.");
        require(extensions.front().getGrantedBy() == 900,
                "The lecturer ID should be stored.");
    }
}

void testClassExtensionUpdatesExistingIndividualExtension() {
    SQLiteConnection connection(":memory:");
    SQLiteExtensionRepository repository(connection);
    repository.initializeTable();

    Extension existing(
        1, 501, 102, revisedDeadline(), "Individual extension", 900
    );
    require(repository.save(existing), "Initial individual extension should save.");

    Datetime classDeadline;
    classDeadline.setday(25);
    classDeadline.setmonth(11);
    classDeadline.setyear(2026);
    classDeadline.sethours(23);
    classDeadline.setminutes(59);
    classDeadline.setseconds(0);

    require(repository.saveForStudents(
        501, std::vector<UserId>{101, 102, 103}, classDeadline,
        "Class-wide extension", 901
    ), "Class extension should update existing records and add missing records.");

    for (UserId studentId : std::vector<UserId>{101, 102, 103}) {
        const auto extensions = repository.findByAssignmentAndStudent(501, studentId);
        require(extensions.size() == 1, "Each student should have one current extension record.");
        Datetime storedDeadline = extensions.front().getRevisedDeadline();
        require(storedDeadline.getday() == 25, "Class deadline should replace the earlier deadline.");
        require(extensions.front().getGrantedBy() == 901, "Class extension should record the lecturer.");
    }
}

void testClassExtensionRejectsDuplicateStudentIDs() {
    SQLiteConnection connection(":memory:");
    SQLiteExtensionRepository repository(connection);
    repository.initializeTable();

    require(!repository.saveForStudents(
        501, std::vector<UserId>{101, 101}, revisedDeadline(), "Class extension", 900
    ), "Duplicate student IDs should be rejected.");
    require(repository.findByAssignmentAndStudent(501, 101).empty(),
            "Rejected input should not create an extension record.");
}

void testClassExtensionRejectsEmptyClass() {
    SQLiteConnection connection(":memory:");
    SQLiteExtensionRepository repository(connection);
    repository.initializeTable();

    require(!repository.saveForStudents(
        501, std::vector<UserId>{}, revisedDeadline(), "Class extension", 900
    ), "An empty class should not receive an extension.");
}
}

int main() {
    try {
        testClassExtensionSavedForEveryStudent();
        std::cout << "PASS: class extension is saved for every student.\n";
        testClassExtensionUpdatesExistingIndividualExtension();
        std::cout << "PASS: class extension updates an existing individual extension.\n";
        testClassExtensionRejectsEmptyClass();
        std::cout << "PASS: empty classes are rejected.\n";
        testClassExtensionRejectsDuplicateStudentIDs();
        std::cout << "PASS: duplicate student IDs are rejected.\n";
        std::cout << "All 4 class extension regression tests passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Regression test failed: " << error.what() << '\n';
        return 1;
    }
}
