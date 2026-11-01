# LMS Release Notes – Version 1.1

**Release tag:** `v1.1`  
**Release type:** Maintenance release / functional enhancement  
**Change request:** CR1 – Class-wide assignment deadline extensions

## New functionality

- Lecturers can choose to extend an assignment deadline for one student or for the whole class.
- A whole-class extension applies to students enrolled in the course associated with the selected assignment.
- The same revised deadline and reason are saved for the affected students.
- The lecturer receives a confirmation showing how many students were included.
- A student's assignment list displays the revised deadline and an extension indicator when an extension exists.

## Existing functionality retained

- Lecturers can still grant an extension to an individual student.
- Students can still view assignments for their enrolled courses.
- Existing assignment, submission, marking and feedback functions are not intentionally changed by CR1.

## Technical changes

- Added `SQLiteExtensionRepository::saveForStudents()`.
- The class-wide persistence operation uses a SQLite transaction.
- Existing per-student extension records are updated; records are inserted for students who do not yet have an extension.
- The student assignment-list operation now retrieves extension information before displaying the deadline.

## Regression testing

The new standalone regression test executable covers class-wide extension persistence, updating an existing individual extension, and rejection of an empty class. The test result must be recorded from the final build and test run before publishing v1.1.
