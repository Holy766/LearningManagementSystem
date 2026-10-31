#ifndef SQLITE_EXTENSION_REPOSITORY_H
#define SQLITE_EXTENSION_REPOSITORY_H

#include "sqlite_repository.h"
#include "extension.h"
#include <vector>
#include <optional>

class SQLiteExtensionRepository : public SQLiteRepository {
public:
    explicit SQLiteExtensionRepository(SQLiteConnection& connection);

    void initializeTable();

    bool save(const Extension& extension);

    std::optional<Extension> findById(ExtensionId id);

    std::vector<Extension> findByAssignmentAndStudent(
        AssignmentId assignmentId,
        UserId studentId
    );
};

#endif