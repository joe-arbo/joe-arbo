/*  wtui.cpp    Vewake Vallabhaneni    Virginia Tech       Last Modified: November 29, 2021 
 *  This is the main window for the Workout Planning application. Acts as a central hub for all other widgets.
 *  Qt creator, using UI form design.
 */

#include "wtui.h"
#include "ui_wtui.h"
#include "calendar.h"


wtUI::wtUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::wtUI)
{
    ui->setupUi(this);
    //read existing plan
    std::string name;
    int daysPerWeek, totalWeeks;
    int day, month, year;
    std::ifstream file;
    std::string filename("plan.txt");
    file.open(filename);
    if(file.is_open())
    {
        while(!file.eof())
        {
            file >> name;
            file >> day;
            file >> month;
            file >> year;
            file >> daysPerWeek;
            file >> totalWeeks;
            QDate tempQ = QDate(year, month, day);
            date temp;
            //set current workout to correct
            std::vector<Workout> WorkoutList = Workout::readWorkouts("./workout_Templates.csv");
            Workout* currWorkout;
            bool workoutFound = false;

            for(int i=0; i<WorkoutList.size(); i++)
            {
                if(name == WorkoutList[i].getName())
                {
                    currWorkout = &WorkoutList[i];
                    workoutFound = true;
                }
            }
            if(workoutFound)
            {
                for(int i=0; i<totalWeeks; i++)
                {
                    if(daysPerWeek == 1)
                    {
                        QDate repeat = tempQ.addDays(7*i);
                        temp.day = repeat.day();
                        temp.month = repeat.month();
                        temp.year = repeat.year();
                        Calendar::addToCalendar(temp,*currWorkout);
                    }
                    if(daysPerWeek == 2)
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
                    if(daysPerWeek == 3)
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
                    if(daysPerWeek == 4)
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
                    if(daysPerWeek == 5)
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
                    if(daysPerWeek == 6)
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
                    if(daysPerWeek == 7)
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
            }
        }
        file.close();
    }
    date temp;
    temp.day = Calendar::getNextWorkout().day();
    temp.month = Calendar::getNextWorkout().month();
    temp.year = Calendar::getNextWorkout().year();
    if(Calendar::getDay(temp).size() > 0)
    {
        ui->nextWorkout->setText("Next Workout On "+Calendar::getNextWorkout().toString("dd.MM.yyyy"));
    }

    //coding for dynamically creating list of workouts
    std::vector<Workout> workouts = Workout::readWorkouts("./workout_Templates.csv");
    for(int i = 0; i < workouts.size(); i++){
        QAction* action = new QAction(QString::fromStdString(workouts.at(i).getName()));
        action->setData(i);
        ui->menuEdit_Workouts->addAction(action);
    }

    //adds a button to create a new workout
    QAction* action = new QAction("+Add Workout+");
    action->setData(INT_MAX);
    ui->menuEdit_Workouts->addAction(action);

    connect(ui->menuEdit_Workouts, SIGNAL(triggered(QAction*)), this, SLOT(workoutSelected(QAction*)));
}

wtUI::~wtUI()
{
    delete ui;
}


void wtUI::on_calendarWidget_clicked(const QDate &date)
{
    struct date temp;
    temp.day = Calendar::getNextWorkout().day();
    temp.month = Calendar::getNextWorkout().month();
    temp.year = Calendar::getNextWorkout().year();
    if(Calendar::getDay(temp).size() > 0)
    {
        ui->nextWorkout->setText("Next Workout On "+Calendar::getNextWorkout().toString("dd.MM.yyyy"));
    }
}


void wtUI::on_startWorkout_clicked()
{
    if(lw!=nullptr)
        delete lw;
    QDate day = ui->calendarWidget->selectedDate();
    date temp;
    temp.day = day.day();
    temp.month = day.month();
    temp.year = day.year();
    std::vector<Workout> workouts = Calendar::getDay(temp);
    if(!workouts.empty())
    {
        lw = new LiveWorkout(0, workouts[0]);
        lw->setWindowModality(Qt::ApplicationModal);
        Calendar::removeFromCalendar(temp, workouts[0]);
    }
    else
    {
        lw = nullptr;
        Warning noWorkout;
        noWorkout.exec();
    }
    temp.day = Calendar::getNextWorkout().day();
    temp.month = Calendar::getNextWorkout().month();
    temp.year = Calendar::getNextWorkout().year();
    if(Calendar::getDay(temp).size() > 0)
    {
        ui->nextWorkout->setText("Next Workout On "+Calendar::getNextWorkout().toString("dd.MM.yyyy"));
    }
}


void wtUI::on_newPlan_clicked()
{
    if(wl!=nullptr)
        delete wl;
    wl = new WorkoutListingUI();
    wl->setWindowModality(Qt::ApplicationModal);
    wl->show();
}


void wtUI::on_history_clicked()
{
    if(h!=nullptr)
        delete h;
    h = new History();
    h->setWindowModality(Qt::ApplicationModal);
    h->show();
}


void wtUI::on_actionClear_Plan_triggered()
{
    Calendar::clear();
    std::string filename("plan.txt");
    std::ofstream file;
    file.open(filename);
    file.close();

    ui->nextWorkout->setText("Next Workout On ");
}


void wtUI::on_actionClear_History_triggered()
{
    std::string filename("history.txt");
    std::ofstream file;
    file.open(filename);
    file.close();
}


void wtUI::on_actionExercises_triggered()
{
    ExerciseListUi ExerciseList;
    ExerciseList.exec();
}


void wtUI::workoutSelected(QAction* action)
{
    if(wp!=nullptr)
        delete wp;
    wp = new workoutPlainningUI(action->data().toInt());
    wp->setWindowModality(Qt::ApplicationModal);
    wp->show();
}

