
#include "daily_penalty_strategy.h"
#include <stdexcept>

DailyPenaltyStrategy::DailyPenaltyStrategy(double dailyPenaltyRate)
    : dailyPenaltyRate(dailyPenaltyRate) {
    if (dailyPenaltyRate < 0.0 || dailyPenaltyRate > 1.0) {
        throw std::invalid_argument(
            "Daily penalty rate must be between 0 and 1."
        );
    }
}

double DailyPenaltyStrategy::calculatePenalty(
    double mark,
    int daysLate,
    double maximumMark
) const {
    if (daysLate <= 0) {
        return mark;
    }

    double penalty = maximumMark * dailyPenaltyRate * daysLate;
    double finalMark = mark - penalty;

    return finalMark < 0.0 ? 0.0 : finalMark;
}
