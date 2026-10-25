
#include "datetime.h"

// Setters

void Datetime::setday(int d) {
    day = d;
}

void Datetime::setmonth(int m) {
    month = m;
}

void Datetime::setyear(int y) {
    year = y;
}

void Datetime::sethours(int h) {
    hours = h;
}

void Datetime::setminutes(int m) {
    minutes = m;
}

void Datetime::setseconds(int s) {
    seconds = s;
}

// Getters

int Datetime::getday() {
    return day;
}

int Datetime::getmonth() {
    return month;
}

int Datetime::getyear() {
    return year;
}

int Datetime::gethours() {
    return hours;
}

int Datetime::getminutes() {
    return minutes;
}

int Datetime::getseconds() {
    return seconds;
}


#include <iostream>
#include "datetime.h"

using namespace std;

// Display date and time neatly
void Datetime::display() const {
    cout << day << "/" << month << "/" << year
         << " ";

    if (hours < 10)
        cout << "0";
    cout << hours << ":";

    if (minutes < 10)
        cout << "0";
    cout << minutes << ":";

    if (seconds < 10)
        cout << "0";
    cout << seconds;
}

bool Datetime::isBefore(const Datetime& other) const {
    if (year != other.year)
        return year < other.year;

    if (month != other.month)
        return month < other.month;

    if (day != other.day)
        return day < other.day;

    if (hours != other.hours)
        return hours < other.hours;

    if (minutes != other.minutes)
        return minutes < other.minutes;

    return seconds < other.seconds;
}

int Datetime::daysDifference(const Datetime& other) const {
    auto dayNumber = [](int y, int m, int d) {
        y -= (m <= 2);
        int era = (y >= 0 ? y : y - 399) / 400;
        unsigned int yearOfEra = y - era * 400;
        unsigned int monthOfYear = m + (m > 2 ? -3 : 9);
        unsigned int dayOfYear =
            (153 * monthOfYear + 2) / 5 + d - 1;
        unsigned int dayOfEra =
            yearOfEra * 365 + yearOfEra / 4
            - yearOfEra / 100 + dayOfYear;

        return era * 146097 + static_cast<int>(dayOfEra);
    };

    return dayNumber(other.year, other.month, other.day)
         - dayNumber(year, month, day);
}