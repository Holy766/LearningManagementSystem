
#include <gtest/gtest.h>

#include "database.h"
#include "sqlite_user_repository.h"

#include <optional>
#include <string>

class SQLiteUserRepositoryTest : public ::testing::Test {
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

TEST_F(SQLiteUserRepositoryTest, SavesValidUser) {
    EXPECT_TRUE(repository->saveUser(
        1, "student1", "Password123", "Student"
    ));

    EXPECT_TRUE(repository->usernameExists("student1"));
    EXPECT_TRUE(repository->userIdExists(1));
}

TEST_F(SQLiteUserRepositoryTest, RejectsEmptyUsername) {
    EXPECT_FALSE(repository->saveUser(
        1, "", "Password123", "Student"
    ));
}

TEST_F(SQLiteUserRepositoryTest, RejectsEmptyPassword) {
    EXPECT_FALSE(repository->saveUser(
        1, "student1", "", "Student"
    ));
}

TEST_F(SQLiteUserRepositoryTest, RejectsInvalidRole) {
    EXPECT_FALSE(repository->saveUser(
        1, "student1", "Password123", "UnknownRole"
    ));
}

TEST_F(SQLiteUserRepositoryTest, RejectsDuplicateUsername) {
    ASSERT_TRUE(repository->saveUser(
        1, "student1", "Password123", "Student"
    ));

    EXPECT_FALSE(repository->saveUser(
        2, "student1", "AnotherPassword", "Lecturer"
    ));
}

TEST_F(SQLiteUserRepositoryTest, AuthenticatesCorrectPassword) {
    ASSERT_TRUE(repository->saveUser(
        1, "student1", "Password123", "Student"
    ));

    EXPECT_EQ(
        repository->authenticateRole("student1", "Password123"),
        std::optional<std::string>("Student")
    );
}

TEST_F(SQLiteUserRepositoryTest, RejectsIncorrectPassword) {
    ASSERT_TRUE(repository->saveUser(
        1, "student1", "Password123", "Student"
    ));

    EXPECT_FALSE(
        repository->authenticateRole("student1", "WrongPassword")
            .has_value()
    );
}

TEST_F(SQLiteUserRepositoryTest, RejectsUnknownUsername) {
    EXPECT_FALSE(
        repository->authenticateRole("unknown", "Password123")
            .has_value()
    );
}

TEST_F(SQLiteUserRepositoryTest, RejectsDuplicateUserID) {
    ASSERT_TRUE(repository->saveUser(
        1, "student1", "Password123", "Student"
    ));

    EXPECT_FALSE(repository->saveUser(
        1, "student2", "Password456", "Lecturer"
    ));
}


TEST_F(SQLiteUserRepositoryTest, ReturnsFalseWhenNoAdministratorExists) {
    EXPECT_FALSE(repository->administratorExists());
}

TEST_F(SQLiteUserRepositoryTest, ReturnsTrueWhenAdministratorExists) {
    ASSERT_TRUE(repository->saveUser(
        1, "admin1", "Password123", "Administrator"
    ));

    EXPECT_TRUE(repository->administratorExists());
}


TEST_F(SQLiteUserRepositoryTest, ReturnsTrueForExistingUsername) {
    ASSERT_TRUE(repository->saveUser(
        1, "student1", "Password123", "Student"
    ));

    EXPECT_TRUE(repository->usernameExists("student1"));
}

TEST_F(SQLiteUserRepositoryTest, ReturnsFalseForUnknownUsername) {
    EXPECT_FALSE(repository->usernameExists("unknown"));
}

TEST_F(SQLiteUserRepositoryTest, ReturnsTrueForExistingUserID) {
    ASSERT_TRUE(repository->saveUser(
        1, "student1", "Password123", "Student"
    ));

    EXPECT_TRUE(repository->userIdExists(1));
}

TEST_F(SQLiteUserRepositoryTest, ReturnsFalseForUnknownUserID) {
    EXPECT_FALSE(repository->userIdExists(99));
}


TEST_F(SQLiteUserRepositoryTest, RejectsInactiveUser) {
    ASSERT_TRUE(repository->saveUser(
        1, "student1", "Password123", "Student"
    ));

    connection->execute(
        "UPDATE users SET active = 0 WHERE username = 'student1';"
    );

    EXPECT_FALSE(
        repository->authenticateRole("student1", "Password123")
            .has_value()
    );
}

TEST_F(SQLiteUserRepositoryTest, RejectsZeroUserID) {
    EXPECT_FALSE(repository->saveUser(
        0, "student1", "Password123", "Student"
    ));
}

TEST_F(SQLiteUserRepositoryTest, RejectsNegativeUserID) {
    EXPECT_FALSE(repository->saveUser(
        -1, "student1", "Password123", "Student"
    ));
}

TEST_F(SQLiteUserRepositoryTest, AcceptsLecturerRole) {
    EXPECT_TRUE(repository->saveUser(
        2, "lecturer1", "Password123", "Lecturer"
    ));
}

TEST_F(SQLiteUserRepositoryTest, AcceptsHoDRole) {
    EXPECT_TRUE(repository->saveUser(
        3, "hod1", "Password123", "HoD"
    ));
}

TEST_F(SQLiteUserRepositoryTest, AcceptsAdministratorRole) {
    EXPECT_TRUE(repository->saveUser(
        4, "admin1", "Password123", "Administrator"
    ));
}

TEST_F(SQLiteUserRepositoryTest, AllowsMultipleUsersWithSameRole) {
    ASSERT_TRUE(repository->saveUser(
        1, "student1", "Password123", "Student"
    ));

    EXPECT_TRUE(repository->saveUser(
        2, "student2", "Password456", "Student"
    ));
}

TEST_F(SQLiteUserRepositoryTest, CreatesTableMoreThanOnce) {
    EXPECT_NO_THROW(repository->createTable());
    EXPECT_NO_THROW(repository->createTable());
}

TEST_F(SQLiteUserRepositoryTest, RejectsLoginWithEmptyPassword) {
    ASSERT_TRUE(repository->saveUser(
        1, "student1", "Password123", "Student"
    ));

    EXPECT_FALSE(
        repository->authenticateRole("student1", "").has_value()
    );
}

TEST_F(SQLiteUserRepositoryTest, ThrowsWhenUsersTableIsMissing) {
    connection->execute("DROP TABLE users;");

    EXPECT_THROW(
        repository->usernameExists("student1"),
        std::runtime_error
    );
}