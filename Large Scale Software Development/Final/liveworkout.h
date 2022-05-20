/*  liveworkout.cpp    Joseph Arbolino     Virginia Tech       Last Modified: October 29, 2020 
 *  Helps track workout in real time
 */

#ifndef LIVEWORKOUT_H
#define LIVEWORKOUT_H

/**
 * Joseph Arbolino
 * 10/31/21
 * liveworkout.h
 */

#include <string>
#include <vector>
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QDate>

#include "workout.h"
#include "Exercise.h"
#include "ElapsedTime.h"
#include "recap.h"


typedef struct SavedWorkout{
    std::string name;
    int sets=0;
    int reps=0;
    int weight=0;
}savedWorkout ;

class LiveWorkout: public QWidget
{
    Q_OBJECT
public:
    LiveWorkout(int rn, Workout cw);

public slots:
    void nextExercise();
    void nextSet();
private:
    ElapsedTime *et;
    Workout currentWorkout;
    int refNum;
    QPushButton* nextExerciseButton;
    QPushButton* nextSetButton;
    QSpinBox* reps;
    QSpinBox* weight;
    QLabel* setLabel;
    QLabel* repLabel;
    QLabel* weightLabel;
    QLabel* exerciseName;
    Recap *r;
    int set=1;
    savedWorkout toSave;
    int totalWeight=0;
    int totalReps=0;
};

#endif // LIVEWORKOUT_H
