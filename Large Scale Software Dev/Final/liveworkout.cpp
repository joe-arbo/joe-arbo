/*  liveworkout.cpp    Joseph Arbolino     Virginia Tech       Last Modified: October 29, 2020 
 *  Helps track workout in real time
 */

#include "liveworkout.h"
#include <QDebug>
#include <QApplication>

LiveWorkout::LiveWorkout(int rn, Workout cw)
{
    this->setWindowModality(Qt::ApplicationModal);
    //append workout name to history
    std::string filename("history.txt");
    std::fstream file;

    file.open(filename, std::ios_base::app | std::ios_base::in);
    if(file.is_open())
    {
        file << "Workout: " << cw.getName() << std::endl << std::endl;
    }
    file.close();

    refNum = rn;
    currentWorkout = cw;
    et = new ElapsedTime();
    nextExerciseButton = new QPushButton("Next Exercise");
    connect(nextExerciseButton, SIGNAL(clicked()), this, SLOT(nextExercise()));

    QVBoxLayout* layout = new QVBoxLayout;

    QFont f( "Arial", 18, QFont::Bold);
    QString workoutMessage = QString::fromStdString(currentWorkout.getName());
    QLabel* workoutName = new QLabel(workoutMessage);
    workoutName->setFont(f);
    layout->addWidget(workoutName);

    //Adds elapsedTime timer
    et->start();
    layout->addWidget(et);

    //adds currentExercise
    exerciseName = new QLabel(QString::fromStdString(currentWorkout.getExerciseAt(refNum).getName()));
    QFont f2( "Arial", 12, QFont::Normal);
    exerciseName->setFont(f2);
    layout->addWidget(exerciseName);

    //set label
    setLabel = new QLabel(QString("Set ")+QString::number(set));
    layout->addWidget(setLabel);

    //reps
    repLabel = new QLabel("Reps");
    layout->addWidget(repLabel);
    reps = new QSpinBox();
    reps->setRange(0, 200);
    layout->addWidget(reps);

    //weight
    weightLabel = new QLabel("Weight");
    layout->addWidget(weightLabel);
    weight = new QSpinBox();
    weight->setRange(0,2000);
    layout->addWidget(weight);

    //next set button
    nextSetButton = new QPushButton("Next Set");
    connect(nextSetButton, SIGNAL(clicked()), this, SLOT(nextSet()));
    layout->addWidget(nextSetButton);

    //add nextExercise Button
    layout->addWidget(nextExerciseButton);

    setLayout(layout);
    setWindowTitle("Live Workout");
    resize(250,200);
    show();
}

void LiveWorkout::nextExercise()
{
    totalReps+=reps->value();
    totalWeight+=weight->value();
    //write exercise to history
    std::string filename("history.txt");
    std::fstream file;

    int avgReps = totalReps/set;
    int avgWeight = totalWeight/set;
    file.open(filename, std::ios_base::app | std::ios_base::in);
    if(file.is_open())
    {
        file << currentWorkout.getExerciseAt(refNum).getName() << std::endl;
        file << "Sets " << set << std::endl;
        file << "Reps " << avgReps << std::endl;
        file << "Weight " << avgWeight << std::endl << std::endl;
    }
    file.close();
    totalReps = 0;
    totalWeight = 0;

    ++refNum;
    if(refNum < currentWorkout.getSize())
    {
        set = 1;
        setLabel->setText(QString("Set ")+QString::number(set));
        exerciseName->setText(QString::fromStdString(currentWorkout.getExerciseAt(refNum).getName()));
    }
    if(refNum >= currentWorkout.getSize())
    {
        //write time and date workout completed
        std::string filename("history.txt");
        std::fstream file;

        file.open(filename, std::ios_base::app | std::ios_base::in);
        if(file.is_open())
        {
            file << "Completed at " << QTime::currentTime().toString("h:mm ap").toStdString() << " on " << QDate::currentDate().toString("MM.dd.yyyy").toStdString() << std::endl << std::endl;
        }
        file.close();

        r = new Recap;
        r->setWindowModality(Qt::ApplicationModal);
        r->setTime(et->timeElapsed());
        r->setDescription(currentWorkout.getName());
        r->show();
        this->hide();
    }
}

void LiveWorkout::nextSet()
{
    totalReps+=reps->value();
    totalWeight+=weight->value();
    set++;
    setLabel->setText(QString("Set ")+QString::number(set));
}

/*
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Workout w;
    LiveWorkout lw(1, w);
    lw.show();
    return a.exec();
}
*/
