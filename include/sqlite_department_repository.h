
#ifndef SQLITE_DEPARTMENT_REPOSITORY_H
#define SQLITE_DEPARTMENT_REPOSITORY_H

#include "sqlite_repository.h"
#include "department.h"
#include <vector>

class SQLiteDepartmentRepository : public SQLiteRepository {
public:
    explicit SQLiteDepartmentRepository(SQLiteConnection& connection);

    void initializeTable();

    bool addDepartment(const Department& department);

    std::vector<Department> getAllDepartments();
};

#endif
