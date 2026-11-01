
#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H


#include <vector>
#include <string>
#include "submission.h"
#include "datetime.h"
#include "ids.h"
#include "extension.h"

using std::vector;
using std::string;

class Assignment {
private:
    AssignmentId id;
    CourseId courseId;
    string title;
    Datetime deadline;
    double maximumMark;
    vector<Submission> submissions;
    vector<Extension> extensions;

public:
    Assignment(AssignmentId id, CourseId courseId,const string& title,const Datetime& deadline,
     double maximumMark);

    AssignmentId getId() const;
    CourseId getCourseId() const;
    const string& getTitle() const;
    const Datetime& getDeadline() const;
    double getMaximumMark() const;

    void setTitle(const string& title);
    void setMaximumMark(double maximumMark);
    void updateDeadline(const Datetime& newDeadline);

    void display() const;
    void addSubmission(const Submission& submission);
    void addExtension(const Extension& extension);

    Datetime getEffectiveDeadline(UserId studentId) const;
    double calculateLatePenalty(double mark, int daysLate) const;
};

#endif
