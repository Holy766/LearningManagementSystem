#ifndef DAILY_PENALTY_STRATEGY_H
#define DAILY_PENALTY_STRATEGY_H

#include "ilate_penalty_strategy.h"

class DailyPenaltyStrategy : public ILatePenaltyStrategy {
private:
    double dailyPenaltyRate;

public:
    explicit DailyPenaltyStrategy(double dailyPenaltyRate);

    double calculatePenalty(
        double mark,
        int daysLate,
        double maximumMark
    ) const override;
};

#endif
