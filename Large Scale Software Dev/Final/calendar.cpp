/*  calendar.cpp    Vewake Vallabhaneni     Virginia Tech       Last Modified: August 29, 2021 
 *  Implementation of the calender, integrates workout onto calendar
 */

#include "calendar.h"

std::vector<std::pair<date, std::vector<Workout>>> Calendar::workouts;

bool operator==(date lhs, date rhs){
    if((lhs.day == rhs.day) && (lhs.month == rhs.month) && (lhs.year == rhs.year))
        return true;
    return false;
}

Calendar::Calendar()
{

}

bool Calendar::addToCalendar(date day, Workout w)
{
    //if day in calendar
    for(int i=0; i<(int)Calendar::workouts.size(); i++)
    {
        if(Calendar::workouts[i].first == day){
            Calendar::workouts[i].second.push_back(w);
            return true;
        }
    }
    std::pair<date, std::vector<Workout>> temp;
    temp.first = day;
    std::vector<Workout> v;
    v.push_back(w);
    temp.second = v;
    Calendar::workouts.push_back(temp);
    return true;
}

bool Calendar::removeFromCalendar(date day, Workout w)
{
    bool removed = false;
    //if day in calendar
    for(int i=0; i<(int)Calendar::workouts.size(); i++)
    {
        if(Calendar::workouts[i].first == day){
            //remove workout
            for(int j=0; j<(int)Calendar::workouts[i].second.size(); j++){
                if(Calendar::workouts[i].second[j].getName() == w.getName()){
                    Calendar::workouts[i].second.erase(Calendar::workouts[i].second.begin()+j);
                    removed = true;
                }
            }
            if(!removed){
                return false;
            }
            else{
                if(Calendar::workouts[i].second.empty()){
                    Calendar::workouts.erase(Calendar::workouts.begin()+i);
                }
                return true;
            }
        }
    }
    return false;
}

std::vector<Workout> Calendar::getDay(date day)
{
    //if day in calendar
    for(int i=0; i<(int)Calendar::workouts.size(); i++)
    {
        if(Calendar::workouts[i].first == day){
            return Calendar::workouts[i].second;
        }
    }
    std::vector<Workout> temp;
    return temp;
}

void Calendar::clear()
{
    Calendar::workouts.clear();
}

QDate Calendar::getNextWorkout()
{
    if(workouts.empty())
        return QDate(1990,1,1);
    QDate tempQ = QDate(workouts[workouts.size()-1].first.year, workouts[workouts.size()-1].first.month, workouts[workouts.size()-1].first.day);
    for(int i=0; i<workouts.size(); i++)
    {
        if(QDate(workouts[i].first.year, workouts[i].first.month, workouts[i].first.day) >= QDate::currentDate())
        {
            if(tempQ > QDate(workouts[i].first.year, workouts[i].first.month, workouts[i].first.day))
            {
                tempQ = QDate(workouts[i].first.year, workouts[i].first.month, workouts[i].first.day);
            }
        }
    }
    if(tempQ >= QDate::currentDate())
    {
        return tempQ;
    }
    else
    {
        return QDate(1990,1,1);
    }
}
