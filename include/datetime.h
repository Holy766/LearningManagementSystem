
#ifndef DATETIME_H
#define DATETIME_H

class Datetime {
private:
    int day;
    int month;
    int year;
    int hours;
    int minutes;
    int seconds;

public:
    Datetime(): day(0), month(0), year(0),hours(0), minutes(0), seconds(0) {
} 
    void setday(int);
    void setmonth(int);
    void setyear(int);
    void sethours(int);
    void setminutes(int);
    void setseconds(int);

    int getday();
    int getmonth();
    int getyear();
    int gethours();
    int getminutes();
    int getseconds();

    void display() const;
    bool isBefore(const Datetime& other) const;
    int daysDifference(const Datetime& other) const;
};

#endif
