#ifndef NO_PENALTY_STRATEGY_H
#define NO_PENALTY_STRATEGY_H

#include "ilate_penalty_strategy.h"

class NoPenaltyStrategy : public ILatePenaltyStrategy {
public:
    double calculatePenalty(
        double mark,
        int daysLate,
        double maximumMark
    ) const override;
};

#endif
