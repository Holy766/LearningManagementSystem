# Phase 8 – Change Request Analysis (CR1)

## 1. Change request

> A lecturer should be able to extend the deadline for a whole class, not just an individual student, and extensions should be shown on the student's assignment list.

## 2. Maintenance classification

**Type: Perfective maintenance (functional enhancement).**

The LMS already supports extending an individual student's deadline. CR1 improves this existing capability by adding class-wide extensions and displaying the effective deadline on the student's assignment list. It does not primarily correct a defect or adapt the application to a new operating environment.

## 3. Affected requirements

- **FR-EXT-01 – Individual extension:** A lecturer can grant an extension to an enrolled individual student. This existing requirement must continue to work.
- **FR-EXT-02 – Class-wide extension:** A lecturer can grant one revised deadline to all students enrolled in the course associated with a selected assignment.
- **FR-EXT-03 – Extension persistence:** The revised deadline, reason, assignment, affected student and granting lecturer are stored in the database.
- **FR-STU-01 – Effective deadline display:** A student's assignment list displays the revised deadline when an extension exists; otherwise it displays the original assignment deadline.
- **FR-EXT-04 – Class membership:** Only students enrolled in the assignment's course are included in a class-wide extension.
- **FR-EXT-05 – Consistency:** The class-wide operation is transactional so that a failure does not leave only some students updated.

## 4. Impact analysis

| Project area | Affected item | Impact |
|---|---|---|
| Requirements | Lecturer extension requirements | Add the class-wide extension option while retaining individual extensions. |
| Requirements | Student assignment-list requirement | Display the effective deadline and indicate when an extension is granted. |
| Use-case diagram | Grant assignment extension | Add the choice between one student and the whole class. |
| Use-case diagram | View assignments | Show the student's effective deadline. |
| Class design | `Extension` | Existing per-student extension data is reused. |
| Class design | `SQLiteExtensionRepository` | Add an atomic operation to save/update extension records for a list of students. |
| Class design | `SQLiteEnrollmentRepository` | Reuse course enrolment records to determine class membership. |
| User interface | `grantStudentExtension()` | Add individual/class-wide selection and process all enrolled students. |
| User interface | `viewStudentAssignments()` | Retrieve and display the student's revised deadline. |
| User interface | `studentMenu()` and `main()` | Pass the extension repository to the student assignment-list operation. |
| Database | `extensions` table | Reuse the existing per-student records; no new table or column is required. Existing rows are updated for students who already have an extension. |
| Tests | Extension repository | Test class-wide persistence, update of an existing individual extension, and invalid/empty class input. |
| Tests | Existing individual extension flow | Confirm that the original individual extension path remains available. |

The deployment diagram does not need a structural change: the application and SQLite database remain on the same school computer. The installation guide and user manual should be updated to describe the new lecturer option and the revised student assignment list.

## 5. Data handling decision

The existing `extensions` table stores one extension record per student and assignment. CR1 is implemented by applying the same revised deadline and reason to every student enrolled in the selected assignment's course. Existing extension records for those students are updated; missing records are inserted. This preserves the existing schema and allows the student assignment-list lookup to remain per student.

The class-wide save operation runs inside a SQLite transaction. If any operation fails, the transaction is rolled back.

## 6. Regression test cases

| Test ID | Scenario | Expected result |
|---|---|---|
| CR1-T01 | Apply a class-wide extension to three enrolled students with no previous extensions. | One extension record exists for each student with the same revised deadline. |
| CR1-T02 | Apply a class-wide extension when one student already has an individual extension. | The existing record is updated and the other students receive records. |
| CR1-T03 | Submit an empty class list. | The operation is rejected and no records are created. |
| CR1-T04 | Submit a list containing invalid or duplicate student IDs. | The operation is rejected before any database changes. |
| CR1-T05 | Look up extensions for an affected student. | The revised deadline and granting lecturer can be retrieved. |
| CR1-T06 | Use the original individual extension operation. | Individual extension remains available and stored as before. |

## 7. Release

The change is intended for **LMS v1.1**, following the v1.0 release. The v1.1 tag should only be created after the updated build and regression tests pass.
