#include "no_penalty_strategy.h"

double NoPenaltyStrategy::calculatePenalty(
    double mark,
    int daysLate,
    double maximumMark
) const {
    (void)daysLate;
    (void)maximumMark;

    return mark;
}
