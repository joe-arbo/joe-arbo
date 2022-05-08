/*  workoutlistingui.cpp    Andrew Beauchemin     Virginia Tech       Last Modified: August 29, 2020 
 *  UI that lists all of the workouts and allows the user to select the file and chose a date to do it
 */

#include "workoutlistingui.h"
#include <QApplication>
#include <QDebug>

WorkoutListingUI::WorkoutListingUI(){
    this->setWindowModality(Qt::ApplicationModal);
    //reads workouts from file
    WorkoutList = Workout::readWorkouts("./workout_Templates.csv");

    QVBoxLayout* layout = new QVBoxLayout;

    //creates a new button from each workout and connects it to the changeLabel slot
    QPushButton* temp;
    for(int i(0); i < WorkoutList.size(); i++){
        QString tempstring = QString::fromStdString(WorkoutList.at(i).getName());
        temp = new QPushButton(tempstring);
        layout->addWidget(temp);
        connect(temp, SIGNAL(clicked()), this, SLOT(changeLabel()));
    }

    //addts text edit for displaying exercises
    this->exercises = new QTextEdit("Select Workout");
    this->exercises->setReadOnly(true);
    layout->addWidget(this->exercises);

    //adds date to start
    dateLabel = new QLabel("Day to Start");
    layout->addWidget(this->dateLabel);
    currDate = new QDateEdit();
    layout->addWidget(this->currDate);

    //adds days per week spinbox
    dayLabel = new QLabel("Days per Week");
    layout->addWidget(this->dayLabel);
    daysPerWeek = new QSpinBox();
    daysPerWeek->setRange(1,7);
    layout->addWidget(this->daysPerWeek);

    //adds total weeks to add spinbox
    totalLabel = new QLabel("Total Weeks to Add");
    layout->addWidget(this->totalLabel);
    totalWeeks = new QSpinBox();
    totalWeeks->setMinimum(1);
    layout->addWidget(this->totalWeeks);

    //adds start workout button
    startButton = new QPushButton("Add Workout to Calendar");
    layout->addWidget(this->startButton);
    connect(startButton, SIGNAL(clicked()), this, SLOT(on_Start_Clicked()));

    setLayout(layout);
    setVisible(true);
}

WorkoutListingUI::~WorkoutListingUI(){
}

void WorkoutListingUI::changeLabel(){
    //gets button from sender
    QPushButton *button = (QPushButton *)sender();

    //finds the reference to the name of the workout from the anem of the button
    int refNum = 0;
    for(int i(0); i < WorkoutList.size(); i++){
        if (WorkoutList.at(i).getName() == button->text().toStdString()){
            refNum = i;
            currWorkout = &WorkoutList[i];
            break;
        }
    }

    //clears the text box and appends it for each exercise in list
    exercises->clear();
    for (int i = 0;i < WorkoutList.at(refNum).getSize() ;i++ ) {
        exercises->append(QString::fromStdString(WorkoutList.at(refNum).getExerciseAt(i).getName()));
    }

    QScrollBar *vScrollBar = exercises->verticalScrollBar();
    vScrollBar->triggerAction(QScrollBar::SliderToMinimum);

    //sets selected workout
    selectedWorkout = refNum;
}

void WorkoutListingUI::on_Start_Clicked()
{
    if(currWorkout != nullptr)
    {
        std::string filename("plan.txt");
        std::fstream file;
        file.open(filename, std::ios_base::app | std::ios_base::in);

        if(file.is_open())
        {
            file << currWorkout->getName() << std::endl;
            file << currDate->date().day() << std::endl;
            file << currDate->date().month() << std::endl;
            file << currDate->date().year() << std::endl;
            file << daysPerWeek->value() << std::endl;
            file << totalWeeks->value() << std::endl;
        }
        file.close();

        //todays date
        QDate tempQ = currDate->date();
        date temp;

        for(int i=0; i<totalWeeks->value(); i++)
        {
            if(daysPerWeek->value() == 1)
            {
                QDate repeat = tempQ.addDays(7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
            }
            if(daysPerWeek->value() == 2)
            {
                QDate repeat = tempQ.addDays(3+7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
                repeat = tempQ.addDays(7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
            }
            if(daysPerWeek->value() == 3)
            {
                QDate repeat = tempQ.addDays(2+7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
                repeat = tempQ.addDays(4+7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
                repeat = tempQ.addDays(7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
            }
            if(daysPerWeek->value() == 4)
            {
                QDate repeat = tempQ.addDays(1+7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
                repeat = tempQ.addDays(3+7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
                repeat = tempQ.addDays(4+7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
                repeat = tempQ.addDays(7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
            }
            if(daysPerWeek->value() == 5)
            {
                QDate repeat = tempQ.addDays(1+7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
                repeat = tempQ.addDays(2+7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
                repeat = tempQ.addDays(3+7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
                repeat = tempQ.addDays(4+7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
                repeat = tempQ.addDays(7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
            }
            if(daysPerWeek->value() == 6)
            {
                QDate repeat = tempQ.addDays(1+7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
                repeat = tempQ.addDays(2+7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
                repeat = tempQ.addDays(3+7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
                repeat = tempQ.addDays(4+7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
                repeat = tempQ.addDays(5+7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
                repeat = tempQ.addDays(7*i);
                temp.day = repeat.day();
                temp.month = repeat.month();
                temp.year = repeat.year();
                Calendar::addToCalendar(temp,*currWorkout);
            }
            if(daysPerWeek->value() == 7)
            {
                for(int j=0; j<7; j++)
                {
                    QDate repeat = tempQ.addDays(j+7*i);
                    temp.day = repeat.day();
                    temp.month = repeat.month();
                    temp.year = repeat.year();
                    Calendar::addToCalendar(temp,*currWorkout);
                }
            }
        }
        exercises->setText("Plan saved to calendar");
        currWorkout = nullptr;
    }
    else
    {
        exercises->setText("No workout has been selected!");
    }
}

//test main
/*
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WorkoutListingUI b;
    b.show();
    return a.exec();
}
*/

