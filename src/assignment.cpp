
#include <iostream>
#include <stdexcept>
#include "assignment.h"

using namespace std;

Assignment::Assignment(AssignmentId id, CourseId courseId,
    const string& title, const Datetime& deadline, double maximumMark)
    : id(id), courseId(courseId), title(title),
      deadline(deadline), maximumMark(maximumMark) {

    if (maximumMark <= 0) {
        throw invalid_argument(
            "Maximum mark must be greater than zero."
        );
    }
}
// Getters

AssignmentId Assignment::getId() const {
    return id;
}

CourseId Assignment::getCourseId() const {
    return courseId;
}

string Assignment::getTitle() const {
    return title;
}

Datetime Assignment::getDeadline() const {
    return deadline;
}

double Assignment::getMaximumMark() const {
    return maximumMark;
}

// Setters

void Assignment::setTitle(const string& title) {
    this->title = title;
}

void Assignment::setMaximumMark(double maximumMark) {
    if (maximumMark <= 0) {
        throw invalid_argument(
            "Maximum mark must be greater than zero."
        );
    }

    this->maximumMark = maximumMark;
}

void Assignment::updateDeadline(const Datetime& newDeadline) {
    deadline = newDeadline;
}

// Display

void Assignment::display() const {
    cout << "\n========== ASSIGNMENT ==========\n";
    cout << "Assignment ID: " << id << endl;
    cout << "Course ID:     " << courseId << endl;
    cout << "Title:         " << title << endl;
    cout << "Deadline:      ";
    deadline.display();
    cout << endl;
    cout << "Maximum Mark:  " << maximumMark << endl;
    cout << "================================\n";
}

void Assignment::addSubmission(const Submission& submission) {
    if (submission.getAssignmentId() != id) {
        throw std::invalid_argument(
            "Submission belongs to a different assignment."
        );
    }

    submissions.push_back(submission);
}

void Assignment::addExtension(const Extension& extension) {
    if (extension.getAssignmentId() != id) {
        throw std::invalid_argument(
            "Extension belongs to a different assignment."
        );
    }

    for (const Extension& existing : extensions) {
        if (existing.getStudentId() == extension.getStudentId()) {
            throw std::invalid_argument(
                "This student already has an extension for this assignment."
            );
        }
    }

    extensions.push_back(extension);
}


Datetime Assignment::getEffectiveDeadline(UserId studentId) const {
    for (const Extension& extension : extensions) {
        if (extension.getStudentId() == studentId) {
            return extension.getRevisedDeadline();
        }
    }

    return deadline;
}

double Assignment::calculateLatePenalty(double mark, int daysLate) const {
    if (daysLate <= 0) {
        return mark;
    }

    if (daysLate > 3) {
        return 0.0;
    }

    double penalty = maximumMark * 0.10 * daysLate;
    double finalMark = mark - penalty;

    return (finalMark < 0) ? 0.0 : finalMark;
}