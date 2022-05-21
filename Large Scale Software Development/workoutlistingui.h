/*  workoutlistingui.h    Andrew Beauchemin     Virginia Tech       Last Modified: August 29, 2020 
 *  UI that lists all of the workouts and allows the user to select the file and chose a date to do it
 */

#ifndef WORKOUTLISTINGUI_H
#define WORKOUTLISTINGUI_H

#include "workout.h"
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QScrollBar>
#include "calendar.h"
#include <QSpinBox>
#include <QLabel>
#include <QDateEdit>

class WorkoutListingUI : public QWidget {
    Q_OBJECT
public:
    WorkoutListingUI();
    ~WorkoutListingUI();

private:
    std::vector<Workout> WorkoutList;
    QTextEdit* exercises;
    int selectedWorkout = 0;
    QLabel* dateLabel;
    QDateEdit* currDate;
    QLabel* dayLabel;
    QSpinBox* daysPerWeek;
    QLabel* totalLabel;
    QSpinBox* totalWeeks;
    QPushButton* startButton;
    Workout* currWorkout = nullptr;

private slots:
    void changeLabel();
    void on_Start_Clicked();

};

#endif // WORKOUTLISTINGUI_H
