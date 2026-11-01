# LMS v1.1 – User Manual Addendum

This addendum updates the Lecturer and Student sections of the LMS v1.0 user manual.

## Lecturer – Grant an assignment extension

1. Log in using the lecturer account.
2. Select **Grant Student Extension** from the Lecturer Menu.
3. Enter the assignment ID. The assignment must belong to a course assigned to the lecturer.
4. Choose one of the extension scopes:
   - **One student:** enter the student's ID. The student must be enrolled in the assignment's course.
   - **Whole class:** the LMS obtains all students enrolled in the assignment's course and applies the extension to them.
5. Enter the reason for the extension.
6. Enter the revised deadline date and time.
7. Read the confirmation message. For a class-wide extension, the LMS reports the number of affected students.

A class-wide extension updates an existing extension for an affected student or creates a record if that student did not previously have one. The operation is saved as a single database transaction.

## Student – View assignments

1. Log in using the student account.
2. Select **View Assignments**.
3. Review each assignment's deadline.
4. If an extension has been granted, the revised deadline is displayed with **Extension: Granted**.
5. If no extension exists, the original assignment deadline is displayed.
