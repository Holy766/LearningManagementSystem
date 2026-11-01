
#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <exception>
#include <limits>
#include <algorithm>
#include <iterator>

#include "database.h"
#include "sqlite_user_repository.h"
#include "admin_setup.h"
#include "sqlite_department_repository.h"
#include "department.h"
#include "sqlite_course_repository.h"
#include "course.h"
#include "sqlite_lecturer_assignment_repository.h"
#include "sqlite_enrollment_repository.h"
#include "sqlite_assignment_repository.h"
#include "sqlite_extension_repository.h"
#include "sqlite_submission_repository.h"
#include "mark_audit_manager.h"
#include "sqlite_mark_audit_repository.h"
using std::cin;
using std::cout;
using std::endl;
using std::string;

void viewReleasedMarks(UserId studentId,SQLiteSubmissionRepository& submissionRepository,SQLiteAssignmentRepository& assignmentRepository,
    SQLiteEnrollmentRepository& enrollmentRepository,SQLiteCourseRepository& courseRepository) {
    bool found = false;

    auto enrollments = enrollmentRepository.getAllEnrollments();
    auto courses = courseRepository.getAllCourses();

    for (const auto& enrollment : enrollments) {

        if (enrollment.getStudentId() != studentId) {
            continue;
        }

        auto assignments =
            assignmentRepository.findByCourseId(enrollment.getCourseId());

        for (const auto& assignment : assignments) {

            auto submissions =
                submissionRepository.findByAssignmentId(assignment.getId());

            for (const auto& submission : submissions) {

                if (submission.getStudentId() == studentId &&
                    submission.getState() == SubmissionState::RELEASED) {

                    found = true;

                    std::cout << "\nAssignment: "
                              << assignment.getTitle();

                    std::cout << "\nAssignment ID: "
                              << assignment.getId();

                    std::cout << "\nMark: "
                              << submission.getMark();

                    std::cout << "\nFeedback: "
                              << submission.getFeedback();

                    std::cout << "\n--------------------------\n";
                }
            }
        }
    }

    if (!found) {
        std::cout << "No marks have been released yet.\n";
    }
}

void resubmitStudentWork(
    UserId studentId,
    SQLiteEnrollmentRepository& enrollmentRepository,
    SQLiteCourseRepository& courseRepository,
    SQLiteAssignmentRepository& assignmentRepository,
    SQLiteSubmissionRepository& submissionRepository
) {
    auto enrollments = enrollmentRepository.getAllEnrollments();
    auto courses = courseRepository.getAllCourses();

    std::vector<Assignment> availableAssignments;

    // Find assignments from the student's enrolled courses
    for (const auto& enrollment : enrollments) {
        if (enrollment.getStudentId() != studentId) {
            continue;
        }

        for (const auto& course : courses) {
            if (course.getId() == enrollment.getCourseId()) {
                auto assignments =
                    assignmentRepository.findByCourseId(course.getId());

                std::copy(
                    assignments.begin(),
                    assignments.end(),
                    std::back_inserter(availableAssignments)
                );
            }
        }
    }

    if (availableAssignments.empty()) {
        std::cout << "No assignments available.\n";
        return;
    }

    // Display available assignments
    for (const auto& assignment : availableAssignments) {
        std::cout << "\nAssignment ID: " << assignment.getId()
                  << "\nTitle: " << assignment.getTitle()
                  << "\n--------------------------\n";
    }

    int assignmentId;
    std::cout << "\nEnter Assignment ID: ";
    std::cin >> assignmentId;

    // Find selected assignment
    const Assignment* selectedAssignment = nullptr;

    auto assignmentIt = std::find_if(
        availableAssignments.begin(),
        availableAssignments.end(),
        [assignmentId](const Assignment& assignment) {
            return assignment.getId() == assignmentId;
        }
    );

    if (assignmentIt != availableAssignments.end()) {
        selectedAssignment = &(*assignmentIt);
    }

    if (selectedAssignment == nullptr) {
        std::cout << "Invalid assignment ID.\n";
        return;
    }

    // Find the student's existing submission
    auto submissions =
        submissionRepository.findByAssignmentId(assignmentId);

    Submission* selectedSubmission = nullptr;

    auto studentSubmissionIt = std::find_if(
        submissions.begin(),
        submissions.end(),
        [studentId](const Submission& submission) {
            return submission.getStudentId() == studentId;
        }
    );

    if (studentSubmissionIt != submissions.end()) {
        selectedSubmission = &(*studentSubmissionIt);
    }

    if (selectedSubmission == nullptr) {
        std::cout << "You have not submitted this assignment yet.\n";
        return;
    }

    // Collect new file reference
    std::string fileReference;

    std::cout << "Enter new file reference: ";
    std::cin >> fileReference;

    // Collect new submission date
    int year, month, day;
    int hours, minutes, seconds;

    std::cout << "Enter submission date (year month day): ";
    std::cin >> year >> month >> day;

    std::cout << "Enter submission time (hours minutes seconds): ";
    std::cin >> hours >> minutes >> seconds;

    Datetime submittedAt;

    submittedAt.setyear(year);
    submittedAt.setmonth(month);
    submittedAt.setday(day);
    submittedAt.sethours(hours);
    submittedAt.setminutes(minutes);
    submittedAt.setseconds(seconds);

    // Resubmit existing submission
    bool success = selectedSubmission->resubmit(
        fileReference,
        submittedAt,
        *selectedAssignment
    );

    if (!success) {
        return;
    }

    // Update existing database record
    if (submissionRepository.update(*selectedSubmission)) {
        std::cout << "Work resubmitted successfully.\n";
    } else {
        std::cout << "Could not update submission.\n";
    }
}
void viewStudentAssignments(UserId studentId,SQLiteEnrollmentRepository& enrollmentRepository,SQLiteCourseRepository& courseRepository,
    SQLiteAssignmentRepository& assignmentRepository, SQLiteExtensionRepository& extensionRepository) {
    // Get all enrollments
    vector<Enrollment> enrollments =
        enrollmentRepository.getAllEnrollments();

    // Get all courses
    vector<Course> courses =
        courseRepository.getAllCourses();

    bool foundAssignments = false;

    // Check the courses in which this student is enrolled
    for (const Enrollment& enrollment : enrollments) {

        if (enrollment.getStudentId() == studentId) {

            CourseId courseId = enrollment.getCourseId();

            // Find the course details
            for (const Course& course : courses) {

                if (course.getId() == courseId) {

                    cout << "\nCourse: " << course.getTitle()
                         << " (" << course.getCode() << ")\n";

                    // Get assignments belonging to this course
                    vector<Assignment> assignments =
                        assignmentRepository.findByCourseId(courseId);

                    if (assignments.empty()) {
                        cout << "No assignments available for this course.\n";
                    }

                    for (const Assignment& assignment : assignments) {

                        Datetime deadline = assignment.getDeadline();
                        auto extensions = extensionRepository.findByAssignmentAndStudent(
                            assignment.getId(), studentId
                        );
                        bool hasExtension = !extensions.empty();
                        if (hasExtension) {
                            deadline = extensions.back().getRevisedDeadline();
                        }

                        cout << "\nAssignment ID: "
                             << assignment.getId();

                        cout << "\nTitle: "
                             << assignment.getTitle();

                        cout << "\nDeadline: "
                             << deadline.getday() << "/"
                             << deadline.getmonth() << "/"
                             << deadline.getyear() << " "
                             << deadline.gethours() << ":"
                             << deadline.getminutes();
                        if (hasExtension) {
                            cout << "\nExtension: Granted";
                        }

                        cout << "\nMaximum Mark: "
                             << assignment.getMaximumMark();

                        cout << "\n--------------------------\n";

                        foundAssignments = true;
                    }
                }
            }
        }
    }

    if (!foundAssignments) {
        cout << "\nNo assignments found for your enrolled courses.\n";
    }
}

void submitStudentWork(UserId studentId,SQLiteEnrollmentRepository& enrollmentRepository,SQLiteCourseRepository& courseRepository,
    SQLiteAssignmentRepository& assignmentRepository,SQLiteSubmissionRepository& submissionRepository) {
    // Get the student's enrollments
    vector<Enrollment> enrollments =
        enrollmentRepository.getAllEnrollments();

    vector<Course> courses =
        courseRepository.getAllCourses();

    vector<Assignment> availableAssignments;

    // Find assignments from the student's enrolled courses
    for (const Enrollment& enrollment : enrollments) {

        if (enrollment.getStudentId() == studentId) {

            CourseId courseId = enrollment.getCourseId();

            for (const Course& course : courses) {

                if (course.getId() == courseId) {

                    cout << "\nCourse: " << course.getTitle()
                         << " (" << course.getCode() << ")\n";

                    vector<Assignment> assignments =
                        assignmentRepository.findByCourseId(courseId);

                    for (const Assignment& assignment : assignments) {

                        cout << "\nAssignment ID: "
                             << assignment.getId();

                        cout << "\nTitle: "
                             << assignment.getTitle();

                        cout << "\n--------------------------\n";

                        availableAssignments.push_back(assignment);
                    }
                }
            }
        }
    }

    if (availableAssignments.empty()) {
        cout << "\nNo assignments available for submission.\n";
        return;
    }

    // Ask the student to select an assignment
    AssignmentId selectedId;

    cout << "\nEnter Assignment ID: ";
    cin >> selectedId;

    bool found = false;

    for (const Assignment& assignment : availableAssignments) {

        if (assignment.getId() == selectedId) {

            found = true;

            // Check whether the student has already submitted
            vector<Submission> submissions =
                submissionRepository.findByAssignmentId(selectedId);

            bool alreadySubmitted = std::any_of(
                submissions.begin(),
                submissions.end(),
                [studentId](const Submission& submission) {
                     return submission.getStudentId() == studentId;
            }
    );

    if (alreadySubmitted) {
        cout << "\nYou have already submitted this assignment."
            << "\nPlease use the Resubmit Work option.\n";
        return;
    }

            // Get the file reference
            string fileReference;

            cout << "Enter your file reference: ";
            cin >> fileReference;

            // Get submission date and time
            int year, month, day;
            int hours, minutes, seconds;

            cout << "Enter submission date (year month day): ";
            cin >> year >> month >> day;

            cout << "Enter submission time (hours minutes seconds): ";
            cin >> hours >> minutes >> seconds;

            Datetime submittedAt;
            submittedAt.setyear(year);
            submittedAt.setmonth(month);
            submittedAt.setday(day);
            submittedAt.sethours(hours);
            submittedAt.setminutes(minutes);
            submittedAt.setseconds(seconds);

            // Create a new submission
            Submission submission(
                submissionRepository.getNextSubmissionId(),
                selectedId,
                studentId
            );

            // Submit the work
            submission.submit(
                fileReference,
                submittedAt,
                assignment
            );

            // Save only if submission was accepted
            if (submission.hasSubmitted()) {

                submissionRepository.save(submission);

                cout << "\nSubmission saved successfully.\n";

                if (submission.getState() == SubmissionState::LATE) {
                    cout << "Submission status: LATE\n";
                } else {
                    cout << "Submission status: SUBMITTED\n";
                }
            }

            break;
        }
    }

    if (!found) {
        cout << "\nInvalid assignment ID.\n";
    }
}
void studentMenu(UserId studentId,SQLiteEnrollmentRepository& enrollmentRepository,SQLiteCourseRepository& courseRepository,
    SQLiteAssignmentRepository& assignmentRepository, SQLiteExtensionRepository& extensionRepository,
    SQLiteSubmissionRepository& submissionRepository) {
    int choice;
    do {
        cout << "\n===== Student Menu =====\n";
        cout << "1. View Assignments\n";
        cout << "2. Submit Work\n";
        cout << "3. Resubmit Work\n";
        cout << "4. View Released Marks and Feedback\n";
        cout << "0. Logout\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: {
                viewStudentAssignments(studentId, enrollmentRepository, courseRepository, assignmentRepository, extensionRepository);
                 break;
            }
            case 2: {
                submitStudentWork(studentId, enrollmentRepository, courseRepository, assignmentRepository, submissionRepository);
                 break;
            }
            case 3:{ 
                resubmitStudentWork(studentId, enrollmentRepository, courseRepository, assignmentRepository, submissionRepository);
                 break;
            }
            case 4: {
                viewReleasedMarks(studentId, submissionRepository, assignmentRepository, enrollmentRepository,courseRepository);
                 break;
            }
            case 0: break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

void createAssignment(
    int lecturerId,
    SQLiteCourseRepository& courseRepository,
    SQLiteLecturerAssignmentRepository& lecturerAssignmentRepository,
    SQLiteAssignmentRepository& assignmentRepository
) {
    vector<int> assignedCourses =
        lecturerAssignmentRepository.getCoursesForLecturer(lecturerId);

    if (assignedCourses.empty()) {
        cout << "You have no courses assigned to you.\n";
        return;
    }

    vector<Course> allCourses = courseRepository.getAllCourses();

    cout << "\n===== YOUR ASSIGNED COURSES =====\n";

    
       for (const Course& course : allCourses) {
             bool isAssigned = std::any_of(
            assignedCourses.begin(),
            assignedCourses.end(),
             [&course](int courseId) {
            return course.getId() == courseId;
        }
    );

    if (isAssigned) {
        cout << "ID: " << course.getId()
             << " | Code: " << course.getCode()
             << " | Title: " << course.getTitle() << endl;
    }
    }


    cout << "\nEnter Course ID: ";
    int courseId;
    cin >> courseId;

    bool assigned = false;

    assigned = std::any_of(
      assignedCourses.begin(),
      assignedCourses.end(),
      [courseId](int id) {
        return id == courseId;
     }
    );

    if (!assigned) {
        cout << "You are not assigned to this course.\n";
        return;
    }
        int assignmentId;
    string title;
    int day, month, year;
    int hours, minutes, seconds;
    double maximumMark;

    cout << "\nEnter Assignment ID: ";
    cin >> assignmentId;

    if (assignmentId <= 0) {
        cout << "Invalid assignment ID.\n";
        return;
    }

    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    cout << "Enter Assignment Title: ";
    getline(cin, title);

    if (title.empty()) {
        cout << "Assignment title cannot be empty.\n";
        return;
    }

    cout << "\nEnter Deadline Details\n";
    cout << "Day: ";
    cin >> day;
    cout << "Month: ";
    cin >> month;
    cout << "Year: ";
    cin >> year;

    cout << "Hours (0-23): ";
    cin >> hours;
    cout << "Minutes (0-59): ";
    cin >> minutes;
    cout << "Seconds (0-59): ";
    cin >> seconds;

    if (month < 1 || month > 12 ||
        day < 1 || day > 31 ||
        year < 2026 ||
        hours < 0 || hours > 23 ||
        minutes < 0 || minutes > 59 ||
        seconds < 0 || seconds > 59) {

        cout << "Invalid deadline details.\n";
        return;
    }

    cout << "Enter Maximum Mark: ";
    cin >> maximumMark;

    if (maximumMark <= 0) {
        cout << "Maximum mark must be greater than zero.\n";
        return;
    }

    Datetime deadline;
    deadline.setday(day);
    deadline.setmonth(month);
    deadline.setyear(year);
    deadline.sethours(hours);
    deadline.setminutes(minutes);
    deadline.setseconds(seconds);

    try {
        Assignment assignment(
            assignmentId,
            courseId,
            title,
            deadline,
            maximumMark
        );

        if (assignmentRepository.save(assignment)) {
            cout << "\nAssignment created successfully!\n";
            assignment.display();
        } else {
            cout << "Could not create assignment. "
                 << "The ID may already exist.\n";
        }

    } catch (const std::exception& error) {
        cout << "Error: " << error.what() << '\n';
    }

    
}

void viewAssignments(
    int lecturerId,
    SQLiteLecturerAssignmentRepository& lecturerAssignmentRepository,
    SQLiteAssignmentRepository& assignmentRepository
) {
    vector<int> assignedCourses =
        lecturerAssignmentRepository.getCoursesForLecturer(lecturerId);

    if (assignedCourses.empty()) {
        cout << "You have no courses assigned to you.\n";
        return;
    }

    bool found = false;

    cout << "\n===== YOUR ASSIGNMENTS =====\n";

    for (int courseId : assignedCourses) {
        vector<Assignment> assignments =
            assignmentRepository.findByCourseId(courseId);

        if (!assignments.empty()) {
            found = true;

            cout << "\nCourse ID: " << courseId << endl;

            for (const Assignment& assignment : assignments) {
                assignment.display();
            }
        }
    }

    if (!found) {
        cout << "No assignments have been created yet.\n";
    }
}

void setAssignmentDeadline(
    int lecturerId,
    SQLiteLecturerAssignmentRepository& lecturerAssignmentRepository,
    SQLiteAssignmentRepository& assignmentRepository
) {
    vector<int> assignedCourses =
        lecturerAssignmentRepository.getCoursesForLecturer(lecturerId);

    if (assignedCourses.empty()) {
        cout << "You have no courses assigned to you.\n";
        return;
    }

    cout << "\n===== SET ASSIGNMENT DEADLINE =====\n";
    viewAssignments(
        lecturerId,
        lecturerAssignmentRepository,
        assignmentRepository
    );

    int assignmentId;

    cout << "\nEnter Assignment ID: ";
    cin >> assignmentId;

    try {
        auto assignment = assignmentRepository.findById(assignmentId);

        if (!assignment.has_value()) {
            cout << "Assignment not found.\n";
            return;
        }

        bool assigned = false;

        assigned = std::any_of(
            assignedCourses.begin(),
             assignedCourses.end(),
            [&assignment](int courseId) {
            return courseId == assignment->getCourseId();
        }
    );

        if (!assigned) {
            cout << "This assignment does not belong to your courses.\n";
            return;
        }

        int day, month, year;
        int hours, minutes, seconds;

        cout << "\nEnter New Deadline\n";

        cout << "Day: ";
        cin >> day;

        cout << "Month: ";
        cin >> month;

        cout << "Year: ";
        cin >> year;

        cout << "Hours (0-23): ";
        cin >> hours;

        cout << "Minutes (0-59): ";
        cin >> minutes;

        cout << "Seconds (0-59): ";
        cin >> seconds;

        if (month < 1 || month > 12 ||
            day < 1 || day > 31 ||
            year < 2026 ||
            hours < 0 || hours > 23 ||
            minutes < 0 || minutes > 59 ||
            seconds < 0 || seconds > 59) {

            cout << "Invalid deadline details.\n";
            return;
        }

        Datetime newDeadline;

        newDeadline.setday(day);
        newDeadline.setmonth(month);
        newDeadline.setyear(year);
        newDeadline.sethours(hours);
        newDeadline.setminutes(minutes);
        newDeadline.setseconds(seconds);

        assignment->updateDeadline(newDeadline);

        if (assignmentRepository.update(*assignment)) {
            cout << "\nDeadline updated successfully!\n";
            assignment->display();
        } else {
            cout << "Could not update deadline.\n";
        }

    } catch (const std::exception& error) {
        cout << "Error: " << error.what() << '\n';
    }
}
void grantStudentExtension(
    int lecturerId,
    SQLiteLecturerAssignmentRepository& lecturerAssignmentRepository,
    SQLiteAssignmentRepository& assignmentRepository,
    SQLiteEnrollmentRepository& enrollmentRepository,
    SQLiteExtensionRepository& extensionRepository
) {
    vector<int> assignedCourses =
        lecturerAssignmentRepository.getCoursesForLecturer(lecturerId);

    if (assignedCourses.empty()) {
        cout << "You have no courses assigned to you.\n";
        return;
    }

    cout << "\n===== GRANT ASSIGNMENT EXTENSION =====\n";
    viewAssignments(lecturerId, lecturerAssignmentRepository, assignmentRepository);

    int assignmentId;
    cout << "\nEnter Assignment ID: ";
    cin >> assignmentId;

    try {
        auto assignment = assignmentRepository.findById(assignmentId);
        if (!assignment.has_value()) {
            cout << "Assignment not found.\n";
            return;
        }

        bool assigned = std::any_of(
            assignedCourses.begin(), assignedCourses.end(),
            [&assignment](int courseId) {
                return courseId == assignment->getCourseId();
            }
        );

        if (!assigned) {
            cout << "You are not assigned to this assignment.\n";
            return;
        }

        int extensionType;
        cout << "1. Extend deadline for one student\n";
        cout << "2. Extend deadline for the whole class\n";
        cout << "Choose extension type: ";
        cin >> extensionType;

        if (extensionType != 1 && extensionType != 2) {
            cout << "Invalid extension type.\n";
            return;
        }

        vector<UserId> affectedStudents;
        if (extensionType == 1) {
            int studentId;
            cout << "Enter Student ID: ";
            cin >> studentId;
            if (studentId <= 0 || !enrollmentRepository.isStudentEnrolled(
                    studentId, assignment->getCourseId())) {
                cout << "Student is not enrolled in this course.\n";
                return;
            }
            affectedStudents.push_back(studentId);
        } else {
            vector<Enrollment> enrollments = enrollmentRepository.getAllEnrollments();
            for (const Enrollment& enrollment : enrollments) {
                if (enrollment.getCourseId() == assignment->getCourseId()) {
                    affectedStudents.push_back(enrollment.getStudentId());
                }
            }
            if (affectedStudents.empty()) {
                cout << "There are no students enrolled in this class.\n";
                return;
            }
            cout << "This extension will apply to " << affectedStudents.size()
                 << " enrolled student(s).\n";
        }

        string reason;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Enter Reason for Extension: ";
        getline(cin, reason);
        if (reason.empty()) {
            cout << "Reason cannot be empty.\n";
            return;
        }

        int day, month, year, hours, minutes, seconds;
        cout << "\nEnter Revised Deadline\n";
        cout << "Day: "; cin >> day;
        cout << "Month: "; cin >> month;
        cout << "Year: "; cin >> year;
        cout << "Hours (0-23): "; cin >> hours;
        cout << "Minutes (0-59): "; cin >> minutes;
        cout << "Seconds (0-59): "; cin >> seconds;

        if (month < 1 || month > 12 || day < 1 || day > 31 ||
            year < 2026 || hours < 0 || hours > 23 ||
            minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59) {
            cout << "Invalid deadline details.\n";
            return;
        }

        Datetime revisedDeadline;
        revisedDeadline.setday(day);
        revisedDeadline.setmonth(month);
        revisedDeadline.setyear(year);
        revisedDeadline.sethours(hours);
        revisedDeadline.setminutes(minutes);
        revisedDeadline.setseconds(seconds);

        if (extensionType == 1) {
            if (!extensionRepository.findByAssignmentAndStudent(
                    assignmentId, affectedStudents.front()).empty()) {
                cout << "This student already has an extension for this assignment.\n";
                return;
            }

            int extensionId;
            cout << "Enter Extension ID: ";
            cin >> extensionId;
            if (extensionId <= 0) {
                cout << "Invalid extension ID.\n";
                return;
            }

            Extension extension(
                extensionId, assignmentId, affectedStudents.front(),
                revisedDeadline, reason, lecturerId
            );
            if (extensionRepository.save(extension)) {
                cout << "Individual student extension granted successfully.\n";
            } else {
                cout << "Could not save the extension. Check the extension ID.\n";
            }
        } else {
            if (extensionRepository.saveForStudents(
                    assignmentId, affectedStudents, revisedDeadline, reason, lecturerId)) {
                cout << "Class-wide extension granted successfully to "
                     << affectedStudents.size() << " student(s).\n";
            } else {
                cout << "Could not save the class-wide extension. No changes were made.\n";
            }
        }
    } catch (const std::exception& error) {
        cout << "Error: " << error.what() << '\n';
    }
}

void viewSubmissions(int lecturerId,SQLiteLecturerAssignmentRepository& lecturerAssignmentRepository,
    SQLiteAssignmentRepository& assignmentRepository,
    SQLiteSubmissionRepository& submissionRepository
) {
    try {
        // Get courses assigned to this lecturer
        std::vector<int> courseIds =
            lecturerAssignmentRepository.getCoursesForLecturer(lecturerId);

        if (courseIds.empty()) {
            cout << "You have no assigned courses." << endl;
            return;
        }

        bool foundSubmissions = false;

        // Go through each assigned course
        for (int courseId : courseIds) {

            std::vector<Assignment> assignments =
                assignmentRepository.findByCourseId(courseId);

            // Go through assignments in the course
            for (const Assignment& assignment : assignments) {

                std::vector<Submission> submissions =
                    submissionRepository.findByAssignmentId(
                        assignment.getId()
                    );

                if (submissions.empty()) {
                    continue;
                }

                foundSubmissions = true;

                cout << "\n================================" << endl;
                cout << "Assignment ID: " << assignment.getId() << endl;
                cout << "Submissions:" << endl;

                for (const Submission& submission : submissions) {

                    cout << "\nSubmission ID: "
                         << submission.getId() << endl;

                    cout << "Student ID: "
                         << submission.getStudentId() << endl;

                    cout << "File: "
                         << submission.getFileReference() << endl;

                    Datetime date = submission.getSubmittedAt();

                    cout << "Submitted at: "
                         << date.getday() << "/"
                         << date.getmonth() << "/"
                         << date.getyear() << " "
                         << date.gethours() << ":"
                         << date.getminutes() << endl;

                    cout << "Status: ";

                    switch (submission.getState()) {
                        case SubmissionState::SUBMITTED:
                            cout << "Submitted";
                            break;

                        case SubmissionState::LATE:
                            cout << "Late";
                            break;

                        case SubmissionState::MARKED:
                            cout << "Marked";
                            break;

                        case SubmissionState::MODERATED:
                            cout << "Moderated";
                            break;

                        case SubmissionState::RELEASED:
                            cout << "Released";
                            break;
                    }

                    cout << endl;
                }
            }
        }

        if (!foundSubmissions) {
            cout << "No submissions found for your assignments."
                 << endl;
        }
    }
    catch (const std::exception& e) {
        cout << "Error viewing submissions: "
             << e.what() << endl;
    }
}
void markSubmissionAndGiveFeedback(int lecturerId,SQLiteLecturerAssignmentRepository& lecturerAssignmentRepository,SQLiteAssignmentRepository& assignmentRepository,
    SQLiteSubmissionRepository& submissionRepository,MarkAuditManager& auditManager) {try {
        // 1. Get courses assigned to the lecturer
        std::vector<int> courseIds =
            lecturerAssignmentRepository.getCoursesForLecturer(lecturerId);

        if (courseIds.empty()) {
            cout << "You have no assigned courses." << endl;
            return;
        }

        // 2. Display assignments belonging to the lecturer
        cout << "\n===== YOUR ASSIGNMENTS =====\n";

        bool foundAssignments = false;

        for (int courseId : courseIds) {
            std::vector<Assignment> assignments =
                assignmentRepository.findByCourseId(courseId);

            for (const Assignment& assignment : assignments) {
                foundAssignments = true;

                cout << "Assignment ID: " << assignment.getId() << endl;
                cout << "Title: " << assignment.getTitle() << endl;
                cout << "Course ID: " << courseId << endl;
                cout << "------------------------\n";
            }
        }

        if (!foundAssignments) {
            cout << "No assignments found." << endl;
            return;
        }

        // 3. Select assignment
        AssignmentId assignmentId;

        cout << "\nEnter Assignment ID: ";
        cin >> assignmentId;

        auto assignment = assignmentRepository.findById(assignmentId);

        if (!assignment) {
            cout << "Assignment not found." << endl;
            return;
        }

        // Verify lecturer owns the course
        bool ownsCourse = false;

        ownsCourse = std::any_of(
             courseIds.begin(),
            courseIds.end(),
            [&assignment](int courseId) {
             return courseId == assignment->getCourseId();
            }
        );

        if (!ownsCourse) {
            cout << "You are not assigned to this course." << endl;
            return;
        }

        // 4. Retrieve submissions
        std::vector<Submission> submissions =
            submissionRepository.findByAssignmentId(assignmentId);

        if (submissions.empty()) {
            cout << "No submissions found for this assignment." << endl;
            return;
        }

        cout << "\n===== SUBMISSIONS =====\n";

        for (const Submission& submission : submissions) {
            cout << "\nSubmission ID: " << submission.getId() << endl;
            cout << "Student ID: " << submission.getStudentId() << endl;
            cout << "File: " << submission.getFileReference() << endl;
            cout << "Status: " << static_cast<int>(submission.getState())
                 << endl;
        }

        // 5. Select submission
        SubmissionId submissionId;

        cout << "\nEnter Submission ID to mark: ";
        cin >> submissionId;

        bool foundSubmission = false;
        Submission selectedSubmission(
            0, assignmentId, 0
        );

        auto it = std::find_if(
            submissions.begin(),
            submissions.end(),
            [submissionId](const Submission& submission) {
          return submission.getId() == submissionId;
            }
    );

    if (it != submissions.end()) {
        selectedSubmission = *it;
        foundSubmission = true;
    }

        if (!foundSubmission) {
            cout << "Submission not found." << endl;
            return;
        }

        if (selectedSubmission.hasBeenMarked()) {
            cout << "This submission has already been marked."
                 << endl;
            return;
        }

        // 6. Enter mark
        double mark;

        cout << "Enter mark (maximum "
             << assignment->getMaximumMark() << "): ";
        cin >> mark;

        // 7. Enter feedback and reason
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        string feedback;
        string reason;

        cout << "Enter feedback: ";
        getline(cin, feedback);

        cout << "Enter reason for marking: ";
        getline(cin, reason);

        // 8. Enter date and time of marking
        int day, month, year, hours, minutes, seconds;

        cout << "Enter marking day: ";
        cin >> day;

        cout << "Enter marking month: ";
        cin >> month;

        cout << "Enter marking year: ";
        cin >> year;

        cout << "Enter hours: ";
        cin >> hours;

        cout << "Enter minutes: ";
        cin >> minutes;

        cout << "Enter seconds: ";
        cin >> seconds;

        if (day < 1 || day > 31 ||
            month < 1 || month > 12 ||
            year < 2026 ||
            hours < 0 || hours > 23 ||
            minutes < 0 || minutes > 59 ||
            seconds < 0 || seconds > 59) {

            cout << "Invalid date or time." << endl;
            return;
        }

        Datetime changedAt;
        changedAt.setday(day);
        changedAt.setmonth(month);
        changedAt.setyear(year);
        changedAt.sethours(hours);
        changedAt.setminutes(minutes);
        changedAt.setseconds(seconds);

        // 9. Mark submission and create audit entry
        selectedSubmission.mark(
            mark,
            feedback,
            lecturerId,
            changedAt,
            reason,
            auditManager,
            *assignment
        );

        // 10. Save updated submission
        if (submissionRepository.update(selectedSubmission)) {
            cout << "\nSubmission marked successfully." << endl;
            cout << "Status: MARKED" << endl;
            cout << "Feedback is awaiting HoD moderation." << endl;
        }
        else {
            cout << "Could not update submission in database." << endl;
        }
    }
    catch (const std::exception& e) {
        cout << "Error marking submission: "
             << e.what() << endl;
    }
}

void lecturerMenu(int lecturerId,SQLiteCourseRepository& courseRepository,SQLiteLecturerAssignmentRepository& lecturerAssignmentRepository,
    SQLiteAssignmentRepository& assignmentRepository,SQLiteExtensionRepository& extensionRepository,SQLiteEnrollmentRepository& enrollmentRepository,
    SQLiteSubmissionRepository& submissionRepository,  MarkAuditManager& auditManager) {
    int choice;
    do {
        cout << "\n===== Lecturer Menu =====\n";
        cout << "1. Create Assignment\n";
        cout << "2. Set Assignment Deadline\n";
        cout << "3. Grant Student Extension\n";
        cout << "4. View Submissions\n";
        cout << "5. Mark Submission and Give Feedback\n";
        cout << "6. View Assignments\n";
        cout << "0. Logout\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1:{
                 createAssignment(lecturerId, courseRepository, lecturerAssignmentRepository, assignmentRepository); 
                break;
            }
            case 2: {
                setAssignmentDeadline(lecturerId, lecturerAssignmentRepository, assignmentRepository);
                break;
            }
           case 3:{
                grantStudentExtension(lecturerId,lecturerAssignmentRepository,assignmentRepository,enrollmentRepository,extensionRepository);
                break;
           }
            case 4: {
                viewSubmissions(lecturerId, lecturerAssignmentRepository, assignmentRepository,submissionRepository);
                break;
            }
            case 5: {
                markSubmissionAndGiveFeedback(lecturerId, lecturerAssignmentRepository, assignmentRepository, submissionRepository, auditManager);
                break;
            }
            case 6: {
                viewAssignments(lecturerId, lecturerAssignmentRepository, assignmentRepository);
                break;
            }
            case 0: break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}
void viewMarkedSubmissions(SQLiteAssignmentRepository& assignmentRepository,SQLiteSubmissionRepository& submissionRepository) {
    CourseId courseId;

    cout << "\nEnter Course ID: ";
    cin >> courseId;

    vector<Assignment> assignments =
        assignmentRepository.findByCourseId(courseId);

    if (assignments.empty()) {
        cout << "No assignments found for this course.\n";
        return;
    }

    cout << "\n===== Assignments =====\n";

    for (const auto& assignment : assignments) {
        cout << "Assignment ID: " << assignment.getId()
             << " | Title: " << assignment.getTitle() << endl;
    }
    AssignmentId selectedId;

cout << "\nEnter Assignment ID: ";
cin >> selectedId;

vector<Submission> submissions =
    submissionRepository.findByAssignmentId(selectedId);

bool found = false;

for (const auto& submission : submissions) {
    if (submission.getState() == SubmissionState::MARKED) {
        found = true;

        cout << "\nSubmission ID: " << submission.getId() << endl;
        cout << "Student ID: " << submission.getStudentId() << endl;
        cout << "Mark: " << submission.getStoredMark() << endl;
        cout << "Feedback: " << submission.getStoredFeedback() << endl;
    }
}

if (!found) {
    cout << "No marked submissions found for this assignment.\n";
}
}
void moderateMarks(SQLiteAssignmentRepository& assignmentRepository,SQLiteSubmissionRepository& submissionRepository) {
    CourseId courseId;

    cout << "\nEnter Course ID: ";
    cin >> courseId;

    vector<Assignment> assignments =
        assignmentRepository.findByCourseId(courseId);

    if (assignments.empty()) {
        cout << "No assignments found for this course.\n";
        return;
    }

    cout << "\n===== Assignments =====\n";

    for (const auto& assignment : assignments) {
        cout << "Assignment ID: " << assignment.getId()
             << " | Title: " << assignment.getTitle() << endl;
    }

    AssignmentId assignmentId;

    cout << "\nEnter Assignment ID: ";
    cin >> assignmentId;

    // Check that the assignment belongs to this course
    bool assignmentFound = false;

    assignmentFound = std::any_of(
            assignments.begin(),
            assignments.end(),
            [assignmentId](const Assignment& assignment) {
             return assignment.getId() == assignmentId;
        }
    );
    
    if (!assignmentFound) {
        cout << "Invalid assignment ID for this course.\n";
        return;
    }

    vector<Submission> submissions =
        submissionRepository.findByAssignmentId(assignmentId);

    bool found = false;

    cout << "\n===== Marked Submissions =====\n";

    for (const auto& submission : submissions) {
        if (submission.getState() == SubmissionState::MARKED) {
            found = true;

            cout << "Submission ID: " << submission.getId()
                 << " | Student ID: " << submission.getStudentId()
                 << " | Mark: " << submission.getStoredMark()
                 << " | Feedback: " << submission.getStoredFeedback()
                 << endl;
        }
    }

    if (!found) {
        cout << "No submissions are awaiting moderation.\n";
        return;
    }

    SubmissionId submissionId;

    cout << "\nEnter Submission ID to moderate: ";
    cin >> submissionId;

    for (auto& submission : submissions) {
        if (submission.getId() == submissionId) {

            if (submission.getState() != SubmissionState::MARKED) {
                cout << "This submission is not awaiting moderation.\n";
                return;
            }

            submission.transition(SubmissionState::MODERATED);

            if (submissionRepository.update(submission)) {
                cout << "Submission successfully moderated.\n";
            } else {
                cout << "Failed to update submission.\n";
            }

            return;
        }
    }

    cout << "Submission ID not found.\n";
}
void releaseMarks(SQLiteAssignmentRepository& assignmentRepository,SQLiteSubmissionRepository& submissionRepository) {
    CourseId courseId;

    cout << "\nEnter Course ID: ";
    cin >> courseId;

    vector<Assignment> assignments =
        assignmentRepository.findByCourseId(courseId);

    if (assignments.empty()) {
        cout << "No assignments found for this course.\n";
        return;
    }

    cout << "\n===== Assignments =====\n";

    for (const auto& assignment : assignments) {
        cout << "Assignment ID: " << assignment.getId()
             << " | Title: " << assignment.getTitle() << endl;
    }

    AssignmentId assignmentId;

    cout << "\nEnter Assignment ID: ";
    cin >> assignmentId;

    // Verify that the assignment belongs to the selected course
    bool assignmentFound = false;

    assignmentFound = std::any_of(
        assignments.begin(),
        assignments.end(),
        [assignmentId](const Assignment& assignment) {
            return assignment.getId() == assignmentId;
        }
    );

    if (!assignmentFound) {
        cout << "Invalid assignment ID for this course.\n";
        return;
    }

    vector<Submission> submissions =
        submissionRepository.findByAssignmentId(assignmentId);

    bool found = false;

    cout << "\n===== Moderated Submissions =====\n";

    for (const auto& submission : submissions) {
        if (submission.getState() == SubmissionState::MODERATED) {
            found = true;

            cout << "Submission ID: " << submission.getId()
                 << " | Student ID: " << submission.getStudentId()
                 << " | Mark: " << submission.getStoredMark()
                 << endl;
        }
    }

    if (!found) {
        cout << "No moderated submissions available for release.\n";
        return;
    }

    SubmissionId submissionId;

    cout << "\nEnter Submission ID to release: ";
    cin >> submissionId;

    for (auto& submission : submissions) {
        if (submission.getId() == submissionId) {

            if (submission.getState() != SubmissionState::MODERATED) {
                cout << "This submission has not been moderated.\n";
                return;
            }

            submission.release();

            if (submissionRepository.update(submission)) {
                cout << "Marks successfully released.\n";
            } else {
                cout << "Failed to release marks.\n";
            }

            return;
        }
    }

    cout << "Submission ID not found.\n";
}

void viewMarkAuditRecords(const MarkAuditManager& auditManager) {

    const vector<MarkAuditEntry>& entries = auditManager.getEntries();

    if (entries.empty()) {
        cout << "\nNo mark audit records found.\n";
        return;
    }

    cout << "\n===== MARK AUDIT RECORDS =====\n";

    for (const auto& entry : entries) {

        Datetime date = entry.getChangedAt();

        cout << "\nAudit ID: " << entry.getId() << endl;
        cout << "Submission ID: " << entry.getSubmissionId() << endl;
        cout << "Changed by Lecturer ID: " << entry.getChangedBy() << endl;

        cout << "Date: "
             << date.getday() << "/"
             << date.getmonth() << "/"
             << date.getyear() << " "
             << date.gethours() << ":"
             << date.getminutes() << endl;

        cout << "Old Mark: " << entry.getOldMark() << endl;
        cout << "New Mark: " << entry.getNewMark() << endl;
        cout << "Reason: " << entry.getReason() << endl;

        cout << "-----------------------------\n";
    }
}
void hodMenu(SQLiteAssignmentRepository& assignmentRepository,SQLiteSubmissionRepository& submissionRepository,const MarkAuditManager& auditManage) {
    int choice;
    do {
        cout << "\n===== Head of Department Menu =====\n";
        cout << "1. View Marked Submissions\n";
        cout << "2. Moderate Marks\n";
        cout << "3. Release Marks\n";
        cout << "4. View Mark Audit Records\n";
        cout << "0. Logout\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1:{ 
                viewMarkedSubmissions(assignmentRepository, submissionRepository); 
                break;
            }
            case 2: {
                moderateMarks(assignmentRepository, submissionRepository); 
                break;
            }
            case 3: {
                releaseMarks(assignmentRepository, submissionRepository);
                break;
            }
            case 4: {
                viewMarkAuditRecords(auditManage);
                break;
            }
            case 0: break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

void manageDepartments(SQLiteDepartmentRepository& departmentRepository) {
    int choice;

    do {
        cout << "\n===== Department Management =====\n";
        cout << "1. Add Department\n";
        cout << "2. View Departments\n";
        cout << "0. Back\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            choice = -1;
        }

        switch (choice) {
            case 1: {
                int departmentId;
                int hodId;
                string name;

                cout << "Department ID: ";
                cin >> departmentId;

                cout << "Department name (one word for now): ";
                cin >> name;

                cout << "HoD user ID: ";
                cin >> hodId;

                Department department(
                    departmentId, name, hodId
                );

                if (departmentRepository.addDepartment(department)) {
                    cout << "Department added successfully.\n";
                } else {
                    cout << "Could not add department. "
                         << "Check the IDs, HoD role, and duplicate entries.\n";
                }

                break;
            }

            case 2: {
                auto departments =
                    departmentRepository.getAllDepartments();

                if (departments.empty()) {
                    cout << "No departments found.\n";
                } else {
                    cout << "\n===== Departments =====\n";

                    for (const auto& department : departments) {
                        cout << "Department ID: "
                             << department.getId() << '\n';

                        cout << "Name: "
                             << department.getName() << '\n';

                        cout << "HoD ID: "
                             << department.getHodId() << "\n\n";
                    }
                }

                break;
            }

            case 0:
                break;

            default:
                cout << "Invalid choice.\n";
        }

    } while (choice != 0);
}


void manageCourses(SQLiteCourseRepository& courseRepository) {
    int choice;

    do {
        cout << "\n===== Course Management =====\n";
        cout << "1. Add Course\n";
        cout << "2. View Courses\n";
        cout << "0. Back\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            choice = -1;
        }

        switch (choice) {
            case 1: {
                int courseId;
                int departmentId;
                string code, title;

                cout << "Course ID: ";
                cin >> courseId;

                cout << "Department ID: ";
                cin >> departmentId;

                cout << "Course code: ";
                cin >> code;

                cout << "Course title: ";
                cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n'
                );
                std::getline(cin, title);

                Course course(
                    courseId, departmentId, code, title
                );

                if (courseRepository.addCourse(course)) {
                    cout << "Course added successfully.\n";
                } else {
                    cout << "Could not add course. "
                         << "Check the department ID and course details.\n";
                }

                break;
            }

            case 2: {
                auto courses = courseRepository.getAllCourses();

                if (courses.empty()) {
                    cout << "No courses found.\n";
                } else {
                    cout << "\n===== Available Courses =====\n";

                    for (const auto& course : courses) {
                        cout << "Course ID: "
                             << course.getId() << '\n';

                        cout << "Department ID: "
                             << course.getDepartmentId() << '\n';

                        cout << "Course code: "
                             << course.getCode() << '\n';

                        cout << "Course title: "
                             << course.getTitle() << "\n\n";
                    }
                }

                break;
            }

            case 0:
                break;

            default:
                cout << "Invalid choice.\n";
        }

    } while (choice != 0);
}

void manageLecturerAssignments(
    SQLiteCourseRepository& courseRepository,
    SQLiteLecturerAssignmentRepository& assignmentRepository
) {
    int choice;

    do {
        cout << "\n===== Lecturer Assignment =====\n";
        cout << "1. Assign Lecturer to Course\n";
        cout << "2. View Lecturer Assignments\n";
        cout << "0. Back\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                int courseId, lecturerId;

                cout << "\nAvailable Courses:\n";

                auto courses = courseRepository.getAllCourses();

                if (courses.empty()) {
                    cout << "No courses available.\n";
                    break;
                }

                for (const auto& course : courses) {
                    cout << "ID: " << course.getId()
                         << " | " << course.getCode()
                         << " | " << course.getTitle() << '\n';
                }

                cout << "\nEnter Course ID: ";
                cin >> courseId;

                cout << "Enter Lecturer User ID: ";
                cin >> lecturerId;

                if (assignmentRepository.assignLecturer(courseId, lecturerId))
                    cout << "Lecturer assigned successfully.\n";
                else
                    cout << "Assignment failed. Check the course ID, lecturer ID, and whether the course already has a lecturer.\n";

                break;
            }

            case 2: {
                auto courses = courseRepository.getAllCourses();

                cout << "\n===== Lecturer Assignments =====\n";

                if (courses.empty()) {
                    cout << "No courses available.\n";
                    break;
                }

                for (const auto& course : courses) {
                    int lecturerId =
                        assignmentRepository.getLecturerForCourse(
                            course.getId()
                        );

                    cout << "Course: " << course.getCode()
                         << " - " << course.getTitle() << '\n';

                    if (lecturerId == -1)
                        cout << "Lecturer: Not assigned\n";
                    else
                        cout << "Lecturer User ID: " << lecturerId << '\n';

                    cout << "--------------------------\n";
                }

                break;
            }

            case 0:
                break;

            default:
                cout << "Invalid choice.\n";
        }

    } while (choice != 0);
}


void manageEnrollments(
    SQLiteCourseRepository& courseRepository,
    SQLiteEnrollmentRepository& enrollmentRepository
) {
    int choice;

    do {
        cout << "\n===== Student Course Enrollment =====\n";
        cout << "1. Enroll Student\n";
        cout << "2. View Enrollments\n";
        cout << "0. Back\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            choice = -1;
        }

        switch (choice) {
            case 1: {
                int studentId, courseId;

                auto courses = courseRepository.getAllCourses();

                if (courses.empty()) {
                    cout << "No courses available.\n";
                    break;
                }

                cout << "\n===== Available Courses =====\n";

                for (const auto& course : courses) {
                    cout << "ID: " << course.getId()
                         << " | " << course.getCode()
                         << " | " << course.getTitle() << '\n';
                }

                cout << "\nEnter Student User ID: ";
                cin >> studentId;

                cout << "Enter Course ID: ";
                cin >> courseId;

                if (enrollmentRepository.enrollStudent(studentId, courseId))
                    cout << "Student enrolled successfully.\n";
                else
                    cout << "Enrollment failed. Check the student ID, course ID, or whether the student is already enrolled.\n";

                break;
            }

            case 2: {
                auto enrollments =
                    enrollmentRepository.getAllEnrollments();

                cout << "\n===== Student Enrollments =====\n";

                if (enrollments.empty()) {
                    cout << "No enrollments found.\n";
                    break;
                }

                for (const auto& enrollment : enrollments) {
                    cout << "Enrollment ID: "
                         << enrollment.getId() << '\n';

                    cout << "Student ID: "
                         << enrollment.getStudentId() << '\n';

                    cout << "Course ID: "
                         << enrollment.getCourseId() << '\n';

                    cout << "--------------------------\n";
                }

                break;
            }

            case 0:
                break;

            default:
                cout << "Invalid choice.\n";
        }

    } while (choice != 0);
}


void administratorMenu(SQLiteUserRepository& repository, SQLiteDepartmentRepository& departmentRepository,SQLiteCourseRepository& courseRepository,
    SQLiteLecturerAssignmentRepository& lecturerAssignmentRepository, SQLiteEnrollmentRepository& enrollmentRepository ) {
    int choice;

    do {
        cout << "\n===== Administrator Menu =====\n";
        cout << "1. Register User\n";
        cout << "2. Manage Departments\n";
        cout << "3. Manage Courses\n";
        cout << "4. Assign Lecturers to Courses\n";
        cout << "5. Enrol Students in Courses\n";
        cout << "0. Logout\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: {
                int id;
                int roleChoice;
                string username, password, role;

                cout << "New user ID: ";
                if (!(cin >> id) || id <= 0) {
                    cin.clear();
                    cin.ignore(
                        std::numeric_limits<std::streamsize>::max(), '\n'
                    );
                    cout << "Invalid ID.\n";
                    break;
                }

                cout << "Choose role:\n";
                cout << "1. Student\n2. Lecturer\n";
                cout << "3. HoD\n4. Administrator\nChoice: ";

                if (!(cin >> roleChoice)) {
                    cin.clear();
                    cin.ignore(
                        std::numeric_limits<std::streamsize>::max(), '\n'
                    );
                    cout << "Invalid role.\n";
                    break;
                }

                switch (roleChoice) {
                    case 1: role = "Student"; break;
                    case 2: role = "Lecturer"; break;
                    case 3: role = "HoD"; break;
                    case 4: role = "Administrator"; break;
                    default:
                        cout << "Invalid role.\n";
                        continue;
                }

                cout << "New username: ";
                cin >> username;
                if (repository.userIdExists(id)) {
                cout << "That user ID already exists.\n";
                break;
             }

               if (repository.usernameExists(username)) {
              cout << "That username already exists.\n";
              break;
              }
                
                cout << "New password: ";
                cin >> password;

               if (password.empty()) {
               cout << "Password cannot be empty.\n";
               break;
              }
                

                if (repository.saveUser(id, username, password, role)) {
                    cout << "User registered successfully.\n";
                } else {
                    cout << "Registration failed. Check the ID and details.\n";
                }
                break;
            }

            case 2:{
                manageDepartments(departmentRepository);
                break;
            }
            case 3: {
                manageCourses(courseRepository); 
                break;
            }
            case 4: {
                manageLecturerAssignments(courseRepository,lecturerAssignmentRepository);
                break;
            }
            case 5: {
                manageEnrollments(courseRepository, enrollmentRepository);
                break;
            }
            case 0: break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

int main() {
    try {
        SQLiteConnection connection("lms.db");
        SQLiteUserRepository repository(connection);
        repository.createTable();
        SQLiteDepartmentRepository departmentRepository(connection);
        departmentRepository.initializeTable();
        SQLiteCourseRepository courseRepository(connection);
        courseRepository.initializeTable();
        SQLiteLecturerAssignmentRepository lecturerAssignmentRepository(connection);
        lecturerAssignmentRepository.initializeTable();
        SQLiteEnrollmentRepository enrollmentRepository(connection);
        enrollmentRepository.initializeTable();
        SQLiteAssignmentRepository assignmentRepository(connection);
        assignmentRepository.initializeTable();
        SQLiteExtensionRepository extensionRepository(connection);
        extensionRepository.initializeTable();  
        SQLiteSubmissionRepository submissionRepository(connection);
        submissionRepository.initializeTable();
        SQLiteMarkAuditRepository markAuditRepository(connection);
        markAuditRepository.initializeTable();
        MarkAuditManager auditManager(markAuditRepository);

        setupInitialAdministrator(repository); 

        int choice;

        do {
            cout << "\n===== Learning Management System =====\n";
            
            cout << "1. Login\n";
            cout << "0. Exit\nChoice: ";

            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n'
                );
                choice = -1;
            }

            if (choice == 1) {
                string username, password;

                cout << "Username: ";
                cin >> username;

                cout << "Password: ";
                cin >> password;

                auto role = repository.authenticateRole(username, password);

                if (!role.has_value()) {
                    cout << "Login failed. Check your credentials.\n";
                    continue;
                }
                auto loggedInUserId = repository.getUserIdByUsername(username);

                if (!loggedInUserId.has_value()) {
                 cout << "Could not retrieve user ID.\n";
                 continue;
                }
                cout << "Login successful. Role: " << *role << '\n';

                if (*role == "Student") {
                    studentMenu(*loggedInUserId,enrollmentRepository,courseRepository,assignmentRepository,extensionRepository,submissionRepository);
                } else if (*role == "Lecturer") {
                lecturerMenu(*loggedInUserId, courseRepository,lecturerAssignmentRepository,assignmentRepository,extensionRepository,enrollmentRepository, submissionRepository,
                auditManager); 
                } else if (*role == "HoD") {
                hodMenu(assignmentRepository, submissionRepository, auditManager);
                } else if (*role == "Administrator") {
                   administratorMenu(repository, departmentRepository, courseRepository, lecturerAssignmentRepository,
                    enrollmentRepository);
                }
            } else if (choice == 0) {
                cout << "Goodbye!\n";
            } else {
                cout << "Invalid choice.\n";
            }

        } while (choice != 0);

    } catch (const std::exception& error) {
        cout << "Database error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
