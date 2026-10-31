
#include <gtest/gtest.h>
#include "database.h"
#include <stdexcept>

TEST(DatabaseTest, OpensDatabaseSuccessfully) {
    SQLiteConnection connection(":memory:");

    EXPECT_NE(connection.getConnection(), nullptr);
}

TEST(DatabaseTest, ClosesDatabaseSuccessfully) {
    SQLiteConnection connection(":memory:");

    connection.close();

    EXPECT_EQ(connection.getConnection(), nullptr);
}

TEST(DatabaseTest, DoesNotOpenDatabaseTwice) {
    SQLiteConnection connection(":memory:");

    sqlite3* originalConnection = connection.getConnection();

    connection.open();

    EXPECT_EQ(connection.getConnection(), originalConnection);
}

TEST(DatabaseTest, DoesNotCloseWhenAlreadyClosed) {
    SQLiteConnection connection(":memory:");

    connection.close();

    EXPECT_NO_THROW(connection.close());
}

TEST(DatabaseTest, RejectsInvalidDatabasePath) {
    EXPECT_THROW(
        SQLiteConnection("invalid_folder/database.db"),
        std::runtime_error
    );
}


TEST(DatabaseTest, ExecutesValidSQL) {
    SQLiteConnection connection(":memory:");

    EXPECT_NO_THROW(
        connection.execute("CREATE TABLE students (id INTEGER);")
    );
}

TEST(DatabaseTest, RejectsInvalidSQL) {
    SQLiteConnection connection(":memory:");

    EXPECT_THROW(
        connection.execute("INVALID SQL STATEMENT"),
        std::runtime_error
    );
}

TEST(DatabaseTest, RejectsExecutionAfterClosingDatabase) {
    SQLiteConnection connection(":memory:");

    connection.close();

    EXPECT_THROW(
        connection.execute("CREATE TABLE students (id INTEGER);"),
        std::runtime_error
    );
}

