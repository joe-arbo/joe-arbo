/*  calendar.h    Vewake Vallabhaneni     Virginia Tech       Last Modified: August 29, 2021 
 *  Implementation of the calender, integrates workout onto calendar
 */

#ifndef CALENDAR_H
#define CALENDAR_H

#include <vector>
#include <utility>
#include <string>
#include <QDate>
#include "workout.h"

typedef struct date{
    int day;
    int month;
    int year;
}date;

bool operator==(date lhs, date rhs);

class Calendar
{
public:
    Calendar();
    static bool addToCalendar(date day, Workout w);
    static bool removeFromCalendar(date day, Workout w);
    static std::vector<Workout> getDay(date day);//returns all workouts from that day
    static void clear();
    static QDate getNextWorkout();

private:
    static std::vector<std::pair<date, std::vector<Workout>>> workouts;
};

#endif // CALENDAR_H
