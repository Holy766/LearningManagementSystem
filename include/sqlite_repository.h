
#ifndef SQLITE_REPOSITORY_H
#define SQLITE_REPOSITORY_H

#include "database.h"

class SQLiteRepository {
protected:
    SQLiteConnection& connection;

public:
    explicit SQLiteRepository(SQLiteConnection& connection);
    virtual ~SQLiteRepository() = default;

protected:
    sqlite3* getConnection() const;
};

#endif
