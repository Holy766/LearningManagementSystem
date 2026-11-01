#ifndef SQLITE_EXTENSION_REPOSITORY_H
#define SQLITE_EXTENSION_REPOSITORY_H

#include "sqlite_repository.h"
#include "extension.h"
#include <vector>
#include <optional>
#include <string>

class SQLiteExtensionRepository : public SQLiteRepository {
public:
    explicit SQLiteExtensionRepository(SQLiteConnection& connection);

    void initializeTable();

    bool save(const Extension& extension);

    std::optional<Extension> findById(ExtensionId id);

    // Saves one extension record for every student in the supplied class.
    // The operation is atomic: either every student receives the extension or none do.
    bool saveForStudents(
        AssignmentId assignmentId,
        const std::vector<UserId>& studentIds,
        const Datetime& revisedDeadline,
        const std::string& reason,
        UserId grantedBy
    );

    std::vector<Extension> findByAssignmentAndStudent(
        AssignmentId assignmentId,
        UserId studentId
    );
};

#endif