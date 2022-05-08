/*  workoutlisting.cpp    Vewake Vallabhaneni     Virginia Tech       Last Modified: August 29, 2021
 *  UI file to accomidate the workoutlistingui, uses a combination of ui file and explicit delcration
 *  of QT parts in cpp and h files
 */

#include "workoutlisting.h"
#include "ui_workoutlisting.h"

WorkoutListing::WorkoutListing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WorkoutListing)
{
    ui->setupUi(this);
}

WorkoutListing::~WorkoutListing()
{
    delete ui;
}
