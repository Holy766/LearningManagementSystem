
#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>

class SQLiteConnection {
private:
    std::string dataPath;
    sqlite3* db;

public:
    explicit SQLiteConnection(const std::string& dataPath);
    ~SQLiteConnection();

    SQLiteConnection(const SQLiteConnection&) = delete;
    SQLiteConnection& operator=(const SQLiteConnection&) = delete;

    void open();
    void close();
    void execute(const std::string& sql);
    sqlite3* getConnection() const;
};

#endif
