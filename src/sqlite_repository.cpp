
#include "sqlite_repository.h"

SQLiteRepository::SQLiteRepository(SQLiteConnection& connection)
    : connection(connection) {
}

sqlite3* SQLiteRepository::getConnection() const {
    return connection.getConnection();
}
