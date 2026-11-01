# Phase 8 – Updated Design Documents for CR1

## Updated use-case view

```mermaid
flowchart LR
    Lecturer([Lecturer]) --> Grant[Grant assignment extension]
    Grant --> Scope{Extension scope}
    Scope --> Individual[One student]
    Scope --> WholeClass[Whole class]
    WholeClass --> Enrolments[Get students enrolled in assignment course]
    Individual --> Save[Save revised deadline]
    Enrolments --> Save
    Student([Student]) --> View[View assignment list]
    View --> Effective[Display effective deadline]
```

## Updated class view

```mermaid
classDiagram
    class SQLiteEnrollmentRepository {
      +getAllEnrollments()
      +isStudentEnrolled(studentId, courseId)
    }
    class SQLiteExtensionRepository {
      +save(extension)
      +saveForStudents(assignmentId, studentIds, revisedDeadline, reason, grantedBy)
      +findByAssignmentAndStudent(assignmentId, studentId)
    }
    class Extension {
      -id
      -assignmentId
      -studentId
      -revisedDeadline
      -reason
      -grantedBy
      +getRevisedDeadline()
    }
    class Assignment {
      -id
      -courseId
      -deadline
      +getDeadline()
    }
    SQLiteEnrollmentRepository ..> SQLiteExtensionRepository : supplies class membership
    SQLiteExtensionRepository ..> Extension : stores and retrieves
    Assignment ..> Extension : effective deadline is overridden per student
```

## Class-wide extension sequence

```mermaid
sequenceDiagram
    actor Lecturer
    participant UI as LMS Lecturer Menu
    participant Enrol as SQLiteEnrollmentRepository
    participant Ext as SQLiteExtensionRepository
    participant DB as SQLite Database

    Lecturer->>UI: Select assignment and whole-class extension
    UI->>Enrol: Get enrolments for assignment course
    Enrol-->>UI: Enrolled student IDs
    Lecturer->>UI: Enter reason and revised deadline
    UI->>Ext: saveForStudents(assignment, students, deadline, reason, lecturer)
    Ext->>DB: BEGIN TRANSACTION
    loop Each enrolled student
      Ext->>DB: Update existing extension or insert a new one
    end
    Ext->>DB: COMMIT
    Ext-->>UI: Success
    UI-->>Lecturer: Class extension confirmation
```

## Student assignment-list sequence

```mermaid
sequenceDiagram
    actor Student
    participant UI as Student Menu
    participant Assign as SQLiteAssignmentRepository
    participant Ext as SQLiteExtensionRepository

    Student->>UI: View assignments
    UI->>Assign: Get assignments for enrolled course
    Assign-->>UI: Assignment list and original deadlines
    loop Each assignment
      UI->>Ext: findByAssignmentAndStudent(assignmentId, studentId)
      Ext-->>UI: Extension record or no record
    end
    UI-->>Student: Display revised deadline when present; otherwise original deadline
```

## Database impact

No schema migration is required. The existing `extensions` table continues to store an extension for an assignment/student pair. A class-wide extension applies the same revised deadline, reason and granting lecturer to each student in the selected course. Existing records are updated and missing records are inserted within one transaction.

## Deployment impact

No change to the deployment topology. The LMS executable and SQLite database remain deployed on the single Windows school computer.
