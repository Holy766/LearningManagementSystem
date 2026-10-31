
#include <gtest/gtest.h>
#include <cstdio>

#include "database.h"
#include "sqlite_user_repository.h"

class UserIntegrationTest : public ::testing::Test {
protected:
    SQLiteConnection* connection = nullptr;
    SQLiteUserRepository* repository = nullptr;

    void SetUp() override {
        connection = new SQLiteConnection(":memory:");
        repository = new SQLiteUserRepository(*connection);
        repository->createTable();
    }

    void TearDown() override {
        delete repository;
        delete connection;
    }
};

TEST_F(UserIntegrationTest, RegistersUserAndAuthenticatesSuccessfully) {

    // Step 1: Register a user
    bool registered = repository->saveUser(
        101,
        "student101",
        "Password123",
        "Student"
    );

    ASSERT_TRUE(registered);

    // Step 2: Check that the user exists
    EXPECT_TRUE(repository->usernameExists("student101"));
    EXPECT_TRUE(repository->userIdExists(101));

    // Step 3: Authenticate the registered user
    auto role = repository->authenticateRole(
        "student101",
        "Password123"
    );

    ASSERT_TRUE(role.has_value());

    // Step 4: Check the returned role
    EXPECT_EQ(role.value(), "Student");
}

TEST_F(UserIntegrationTest, RejectsAuthenticationWithIncorrectPassword) {

    // Register a user
    ASSERT_TRUE(repository->saveUser(
        102,
        "student102",
        "Password123",
        "Student"
    ));

    // Attempt login with an incorrect password
    auto role = repository->authenticateRole(
        "student102",
        "WrongPassword"
    );

    // Authentication should fail
    EXPECT_FALSE(role.has_value());
}


TEST_F(UserIntegrationTest, RejectsAuthenticationForInactiveUser) {

    // Step 1: Register a user
    ASSERT_TRUE(repository->saveUser(
        103,
        "student103",
        "Password123",
        "Student"
    ));

    // Step 2: Make the user inactive
    connection->execute(
        "UPDATE users SET active = 0 WHERE id = 103;"
    );

    // Step 3: Attempt to authenticate
    auto role = repository->authenticateRole(
        "student103",
        "Password123"
    );

    // Step 4: Authentication should fail
    EXPECT_FALSE(role.has_value());
}


TEST_F(UserIntegrationTest, RejectsDuplicateUsernameDuringRegistration) {

    // Step 1: Register the first user
    ASSERT_TRUE(repository->saveUser(
        104,
        "student104",
        "Password123",
        "Student"
    ));

    // Step 2: Attempt to register another user with the same username
    bool registered = repository->saveUser(
        105,
        "student104",
        "AnotherPassword123",
        "Lecturer"
    );

    // Step 3: The second registration should fail
    EXPECT_FALSE(registered);

    // Step 4: The original user should still exist
    EXPECT_TRUE(repository->userIdExists(104));

    // Step 5: The second user's ID should not exist
    EXPECT_FALSE(repository->userIdExists(105));
}


TEST_F(UserIntegrationTest, RejectsDuplicateUserIDDuringRegistration) {

    // Step 1: Register the first user
    ASSERT_TRUE(repository->saveUser(
        106,
        "student106",
        "Password123",
        "Student"
    ));

    // Step 2: Attempt to register another user with the same ID
    bool registered = repository->saveUser(
        106,
        "lecturer106",
        "Password456",
        "Lecturer"
    );

    // Step 3: Registration should fail
    EXPECT_FALSE(registered);

    // Step 4: The original user should still exist
    EXPECT_TRUE(repository->usernameExists("student106"));

    // Step 5: The second username should not exist
    EXPECT_FALSE(repository->usernameExists("lecturer106"));
}


TEST_F(UserIntegrationTest, DetectsAdministratorAfterRegistration) {

    // Step 1: Confirm that no administrator exists
    EXPECT_FALSE(repository->administratorExists());

    // Step 2: Register an administrator
    ASSERT_TRUE(repository->saveUser(
        107,
        "admin107",
        "AdminPassword123",
        "Administrator"
    ));

    // Step 3: Confirm that an administrator now exists
    EXPECT_TRUE(repository->administratorExists());
}


TEST_F(UserIntegrationTest, AuthenticatesLecturerAndReturnsCorrectRole) {

    // Step 1: Register a lecturer
    ASSERT_TRUE(repository->saveUser(
        108,
        "lecturer108",
        "LecturerPassword123",
        "Lecturer"
    ));

    // Step 2: Authenticate the lecturer
    auto role = repository->authenticateRole(
        "lecturer108",
        "LecturerPassword123"
    );

    // Step 3: Confirm authentication succeeded
    ASSERT_TRUE(role.has_value());

    // Step 4: Confirm the correct role is returned
    EXPECT_EQ(role.value(), "Lecturer");
}


TEST_F(UserIntegrationTest, RejectsPasswordWithIncorrectCapitalization) {

    // Step 1: Register a user
    ASSERT_TRUE(repository->saveUser(
        109,
        "student109",
        "Password123",
        "Student"
    ));

    // Step 2: Attempt login with different capitalization
    auto role = repository->authenticateRole(
        "student109",
        "password123"
    );

    // Step 3: Authentication should fail
    EXPECT_FALSE(role.has_value());
}


TEST_F(UserIntegrationTest, RejectsAuthenticationForUnknownUser) {

    // Step 1: Attempt login with a username that was never registered
    auto role = repository->authenticateRole(
        "unknownuser",
        "Password123"
    );

    // Step 2: Authentication should fail
    EXPECT_FALSE(role.has_value());
}


TEST_F(UserIntegrationTest, RejectsAuthenticationWithEmptyUsername) {

    // Step 1: Attempt login with an empty username
    auto role = repository->authenticateRole(
        "",
        "Password123"
    );

    // Step 2: Authentication should fail
    EXPECT_FALSE(role.has_value());
}

TEST_F(UserIntegrationTest, UserDataPersistsAfterReopeningDatabase) {

    // Step 1: Remove any previous test database
    std::remove("integration_test.db");

    // Step 2: Create a file-based database
    SQLiteConnection fileConnection("integration_test.db");
    fileConnection.open();

    // Step 3: Create the repository and users table
    SQLiteUserRepository fileRepository(fileConnection);
    fileRepository.createTable();

    // Step 4: Register a user
    ASSERT_TRUE(fileRepository.saveUser(
        110,
        "student110",
        "Password123",
        "Student"
    ));

    // Step 5: Close the database
    fileConnection.close();

    // Step 6: Reopen the database
    fileConnection.open();

    // Step 7: Authenticate the registered user
    auto role = fileRepository.authenticateRole(
        "student110",
        "Password123"
    );

    // Step 8: Confirm authentication succeeds
    ASSERT_TRUE(role.has_value());
    EXPECT_EQ(role.value(), "Student");

    // Step 9: Close the database
    fileConnection.close();

    // Step 10: Clean up the test database
    std::remove("integration_test.db");
}
