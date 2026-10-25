#ifndef ILATE_PENALTY_STRATEGY_H
#define ILATE_PENALTY_STRATEGY_H

class ILatePenaltyStrategy {
public:
    virtual ~ILatePenaltyStrategy() = default;

    virtual double calculatePenalty(
        double mark,
        int daysLate,
        double maximumMark
    ) const = 0;
};

#endif
