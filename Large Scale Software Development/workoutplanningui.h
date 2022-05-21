/*  workoutplanningui.h    Andrew Beauchemin/Vewake Vallabhaneni     Virginia Tech       Last Modified: November 29, 2021 
 *  Adds and removes exercises from workouts, and provides the UI to allow for easy planning.
 *  Qt creator, using UI form design.
 */

#ifndef WORKOUTPLANNINGUI_H
#define WORKOUTPLANNINGUI_H

#include "workout.h"
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QLineEdit>
#include "workout.h"

class workoutPlainningUI: public QWidget {
    Q_OBJECT
private slots:
    void selectWorkoutExercise();
    void selectExercise();
    void addExercisefromWorkout();
    void removeExerciseFromWorkout();

    void saveWorkout();

private:
    std::vector<Exercise> exerciseList;
    std::vector<QPushButton*> workoutButtonList;
    std::vector<QPushButton*> exerciseButtonList;
    Workout workout;
    int refNum;

    QLineEdit* workoutName;

    int selectedWorkoutExerciseIndex = INT_MAX;
    int selectedExerciseListIndex = INT_MAX;

    QTextEdit* workoutExerciseSelected;
    QTextEdit* exerciseListExerciseSelected;

    QVBoxLayout* exerciseLayout;
    QVBoxLayout* workoutExerciseLayout;

    QPushButton* backButton;
    QPushButton* addButton;
    QPushButton* removeButtton;

public:
    workoutPlainningUI(int refNum);


};

#endif // WORKOUTPLANNINGUI_H
