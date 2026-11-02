#include <stdexcept>
#include <gtest/gtest.h>
#include "assignment.h"
#include "submission.h"
#include "extension.h"
#include "datetime.h"

// Helper function to create a date and time.
Datetime makeDate(
    int day, int month, int year,
    int hours, int minutes, int seconds = 0
) {
    Datetime date;
    date.setday(day);
    date.setmonth(month);
    date.setyear(year);
    date.sethours(hours);
    date.setminutes(minutes);
    date.setseconds(seconds);
    return date;
}

// Test 1: Submission before deadline.
TEST(LateSubmissionTest, AcceptsSubmissionBeforeDeadline) {
    Datetime deadline = makeDate(10, 11, 2026, 23, 59);
    Datetime submitted = makeDate(10, 11, 2026, 20, 0);

    Assignment assignment(1, 1, "Programming", deadline, 100);
    Submission submission(1, 1, 10);

    submission.submit("assignment.pdf", submitted, assignment);

    EXPECT_TRUE(submission.hasSubmitted());
    EXPECT_EQ(submission.getState(), SubmissionState::SUBMITTED);
}

// Test 2: Submission exactly at deadline.
TEST(LateSubmissionTest, AcceptsSubmissionAtDeadline) {
    Datetime deadline = makeDate(10, 11, 2026, 23, 59);

    Assignment assignment(1, 1, "Programming", deadline, 100);
    Submission submission(1, 1, 10);

    submission.submit("assignment.pdf", deadline, assignment);

    EXPECT_TRUE(submission.hasSubmitted());
    EXPECT_EQ(submission.getState(), SubmissionState::SUBMITTED);
}

// Test 3: Submission one day late.
TEST(LateSubmissionTest, AcceptsSubmissionOneDayLate) {
    Datetime deadline = makeDate(10, 11, 2026, 23, 59);
    Datetime submitted = makeDate(11, 11, 2026, 23, 59);

    Assignment assignment(1, 1, "Programming", deadline, 100);
    Submission submission(1, 1, 10);

    submission.submit("assignment.pdf", submitted, assignment);

    EXPECT_TRUE(submission.hasSubmitted());
    EXPECT_EQ(submission.getState(), SubmissionState::LATE);
}

// Test 4: Submission exactly three days late.
TEST(LateSubmissionTest, AcceptsSubmissionThreeDaysLate) {
    Datetime deadline = makeDate(10, 11, 2026, 23, 59);
    Datetime submitted = makeDate(13, 11, 2026, 23, 59);

    Assignment assignment(1, 1, "Programming", deadline, 100);
    Submission submission(1, 1, 10);

    submission.submit("assignment.pdf", submitted, assignment);

    EXPECT_TRUE(submission.hasSubmitted());
    EXPECT_EQ(submission.getState(), SubmissionState::LATE);
}

// Test 5: Submission more than three days late.
TEST(LateSubmissionTest, RejectsSubmissionMoreThanThreeDaysLate) {
    Datetime deadline = makeDate(10, 11, 2026, 23, 59);
    Datetime submitted = makeDate(14, 11, 2026, 0, 0);

    Assignment assignment(1, 1, "Programming", deadline, 100);
    Submission submission(1, 1, 10);

    submission.submit("assignment.pdf", submitted, assignment);

    EXPECT_FALSE(submission.hasSubmitted());
}

// Test 6: One-day penalty.
TEST(LateSubmissionTest, DeductsTenPointsForOneDayLate) {
    Assignment assignment(
        1, 1, "Programming",
        makeDate(10, 11, 2026, 23, 59), 100
    );

    EXPECT_DOUBLE_EQ(assignment.calculateLatePenalty(80, 1), 70);
}

// Test 7: Two-day penalty.
TEST(LateSubmissionTest, DeductsTwentyPointsForTwoDaysLate) {
    Assignment assignment(
        1, 1, "Programming",
        makeDate(10, 11, 2026, 23, 59), 100
    );

    EXPECT_DOUBLE_EQ(assignment.calculateLatePenalty(80, 2), 60);
}

// Test 8: Three-day penalty.
TEST(LateSubmissionTest, DeductsThirtyPointsForThreeDaysLate) {
    Assignment assignment(
        1, 1, "Programming",
        makeDate(10, 11, 2026, 23, 59), 100
    );

    EXPECT_DOUBLE_EQ(assignment.calculateLatePenalty(80, 3), 50);
}

// Test 9: Reject penalty calculation beyond three days.
TEST(LateSubmissionTest, RejectsPenaltyBeyondThreeDays) {
    Assignment assignment(
        1, 1, "Programming",
        makeDate(10, 11, 2026, 23, 59), 100
    );

    EXPECT_THROW(
        assignment.calculateLatePenalty(80, 4),
        std::invalid_argument
    );
}

// Test 10: Extension changes the effective deadline.
TEST(LateSubmissionTest, UsesStudentExtensionDeadline) {
    Datetime originalDeadline = makeDate(10, 11, 2026, 23, 59);
    Datetime extendedDeadline = makeDate(12, 11, 2026, 23, 59);
    Datetime submitted = makeDate(12, 11, 2026, 23, 59);

    Assignment assignment(
        1, 1, "Programming", originalDeadline, 100
    );

    Extension extension(
        1, 1, 10, extendedDeadline,
        "Approved extension", 20
    );

    assignment.addExtension(extension);

    Submission submission(1, 1, 10);
    submission.submit("assignment.pdf", submitted, assignment);

    EXPECT_TRUE(submission.hasSubmitted());
    EXPECT_EQ(submission.getState(), SubmissionState::SUBMITTED);
}

// Test 6: One second beyond 24 hours counts as two days late.
TEST(LateSubmissionTest, CountsStarted24HourPeriods) {
    Datetime deadline = makeDate(10, 11, 2026, 23, 59, 0);
    Datetime submitted = makeDate(11, 11, 2026, 23, 59, 1);

    EXPECT_EQ(deadline.startedDaysLate(submitted), 2);
}

// Test 7: One second beyond 72 hours is rejected.
TEST(LateSubmissionTest, RejectsSubmissionOneSecondBeyond72Hours) {
    Datetime deadline = makeDate(10, 11, 2026, 23, 59, 0);
    Datetime submitted = makeDate(13, 11, 2026, 23, 59, 1);

    Assignment assignment(1, 1, "Programming", deadline, 100);
    Submission submission(1, 1, 10);

    submission.submit("assignment.pdf", submitted, assignment);

    EXPECT_FALSE(submission.hasSubmitted());
}