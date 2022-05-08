/*  workoutplanningui.cpp    Andrew Beauchemin/Vewake Vallabhaneni     Virginia Tech       Last Modified: November 29, 2021 
 *  Adds and removes exercises from workouts, and provides the UI to allow for easy planning.
 *  Qt creator, using UI form design.
 */

#include "workoutplanningui.h"
#include <QDebug>
#include <QApplication>

QString fromCat(Category cat){
    switch(cat){
    case NO_CATEGORY:
        return "No Catagory";
    case STRETCH:
        return "Stretch";
    case NORMAL:
        return "Normal";
    }
    return "No Catagory";
}

QString fromMuscleGroup(MuscleGroups mc){
    switch(mc){
    case NO_MUSCLE_GROUP:
        return "No Muscle Group:";
    case BACK:
        return "Back";
    case BICEPS:
        return "Biceps";
    case CARDIO:
        return "Cardio";
    case CHEST:
        return "Chest";
    case CORE:
        return "Core";
    case FOREARMS:
        return "Forearms";
    case FULLBODY:
        return "Fullbody";
    case LEGS:
        return "Legs";
    case NECK:
        return "Neck";
    case SHOULDERS:
        return "Shoulders";
    case TRICEPS:
        return "Triceps";
    case WRIST:
        return "Wrists";
    }
    return "No Muscle Group:";
}

workoutPlainningUI::workoutPlainningUI(int refNum){
    //creates exercise list
    exerciseList = Exercise::readExercises("./exercises.csv");

    //if refNum is int max then the user wants to create a new workout, rather than use existsing
    this->refNum = refNum;
    if(refNum != INT_MAX)
    {
        workout = Workout::readWorkouts("./workout_Templates.csv").at(refNum);
        workoutName = new QLineEdit(QString::fromStdString(workout.getName()));
    }
    else
    {
        workout = Workout();
        workoutName = new QLineEdit();
    }

    //layout construction for left collum

    //overarching layout for left collum
    QVBoxLayout* workoutLayout = new QVBoxLayout();

    //editale field for workout name
    workoutLayout->addWidget(workoutName);

    //scroller functionalities
    QScrollArea* workoutScroller =  new QScrollArea();
    workoutScroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    workoutScroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    workoutScroller->setWidgetResizable(true);

    //layout and widget for workout exercise scroller
    workoutExerciseLayout = new QVBoxLayout();
    QWidget* leftwidget = new QWidget();
    leftwidget->setLayout(workoutExerciseLayout);

    //creates a button for each of the exercises in the workout, and links that button
    //also pushes it back in button list
    QPushButton* temp;
    for(int i(0); i < workout.getSize(); i++){
        QString tempstring = QString::fromStdString(workout.getExerciseAt(i).getName());
        temp = new QPushButton(tempstring);
        workoutExerciseLayout->addWidget(temp);
        connect(temp, SIGNAL(clicked()), this, SLOT(selectWorkoutExercise()));
        workoutButtonList.push_back(temp);
    }

   //adds scroller to scroll through left widget, and adds the scroller to the layout
    workoutScroller->setWidget(leftwidget);
    workoutLayout->addWidget(workoutScroller);

    //addts text edit for displaying exercises
    this->workoutExerciseSelected = new QTextEdit("Select Exercise");
    this->workoutExerciseSelected->setReadOnly(true);
    this->workoutExerciseSelected->setMaximumHeight(100);
    workoutLayout->addWidget(this->workoutExerciseSelected);

    //adds start workout button
    backButton = new QPushButton("Save Workout");
    workoutLayout->addWidget(this->backButton);
    connect(backButton, SIGNAL(clicked()), this, SLOT(saveWorkout()));

    //retrieves exercise data from exercise data list
    for (int i(0); i < workout.getExercises().size(); i++){
        std::string temp = workout.getExercises().at(i).getName();
        for (int e(0); e < exerciseList.size(); e++){
            if(temp == exerciseList.at(e).getName()){
                workout.setExerciseAt(i, exerciseList.at(e));
            }
        }
    }

    //middle collum layout

    //creates the <<< and the >>> buttons
    QVBoxLayout* addRemoveLayout = new QVBoxLayout();
    addButton = new QPushButton("<<<");
    removeButtton = new QPushButton(">>>");
    addRemoveLayout->addWidget(addButton);
    addRemoveLayout->addWidget(removeButtton);

    //connects the buttons to their correct function slot
    connect(addButton, SIGNAL(clicked()), this, SLOT(addExercisefromWorkout()));
    connect(removeButtton, SIGNAL(clicked()), this, SLOT(removeExerciseFromWorkout()));


    //right collum layout

    //overarching layout for right collum
    QVBoxLayout* rightLayout = new QVBoxLayout();

    QLabel* exercisesLabel = new QLabel("Exercises");
    rightLayout->addWidget(exercisesLabel);

    //scroller functionalities
    QScrollArea* exerciseScroller =  new QScrollArea();
    exerciseScroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    exerciseScroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    exerciseScroller->setWidgetResizable(true);
    exerciseLayout = new QVBoxLayout();

    QWidget* rightwidget = new QWidget();
    rightwidget->setLayout(exerciseLayout);

    //adds each exercise in the exercise list to the exercise button vector and connects it
    for(int i = 0; i < exerciseList.size(); i++){
        QString tempstring = QString::fromStdString(exerciseList.at(i).getName());
        temp = new QPushButton(tempstring);
        exerciseLayout->addWidget(temp);
        connect(temp, SIGNAL(clicked()), this, SLOT(selectExercise()));
        exerciseButtonList.push_back(temp);
    }

    exerciseScroller->setWidget(rightwidget);
    rightLayout->addWidget(exerciseScroller);

    //addts text edit for displaying exercises
    this->exerciseListExerciseSelected = new QTextEdit("Select Exercise");
    this->exerciseListExerciseSelected->setReadOnly(true);
    this->exerciseListExerciseSelected->setMaximumHeight(100);
    rightLayout->addWidget(exerciseListExerciseSelected);


    //removes all of the exercises alread in the workout from the exercise button list
    for(int z = 0; z < workoutButtonList.size(); z++){
        QString temp = workoutButtonList.at(z)->text();
        for(int i = 0; i < exerciseButtonList.size(); i++){
            if(temp == exerciseButtonList.at(i)->text()){
                exerciseButtonList.at(i)->deleteLater();
                exerciseButtonList.erase(exerciseButtonList.begin()+i);
            }
        }
    }

    //creates main layout with other three layouts
    QHBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->addItem(workoutLayout);
    mainLayout->addItem(addRemoveLayout);
    mainLayout->addItem(rightLayout);

    setLayout(mainLayout);
}

void workoutPlainningUI::selectWorkoutExercise(){
    //gets sender button name
    QPushButton *button = (QPushButton *)sender();
    std::string tempName = button->text().toStdString();

    //finds the exercise from the exercise list
    Exercise tempExercise;
    for(int i = 0; i < workout.getExercises().size(); i++){
        if (workout.getExercises().at(i).getName() == tempName){
            tempExercise = workout.getExercises().at(i);
            break;
        }
    }

    //updates the statistics of the exercise in the text box
    workoutExerciseSelected->setText("Selected: " + QString::fromStdString(tempExercise.getName())
                                     + "\nDescription: " + QString::fromStdString(tempExercise.getDescription())
                                     + "\nCalories Burned: " + QString::number(tempExercise.getCalorieBurn())
                                     + "\nCatagory: " + fromCat(tempExercise.getCategory())
                                     + "\nMuscle Group: " + fromMuscleGroup(tempExercise.getMuscles()));

    //finds the index of the selected button in the button vector
    for(int i = 0; i < workoutButtonList.size(); i++){
        if (workoutButtonList.at(i)->text().toStdString() == tempName){
            selectedWorkoutExerciseIndex = i;
            break;
        }
    }
}

void workoutPlainningUI::selectExercise(){
    QPushButton *button = (QPushButton *)sender();
    std::string tempName = button->text().toStdString();

    //finds teh exercise from the exercise list
    Exercise tempExercise;
    for(int i = 0; i < exerciseList.size(); i++){
        if (exerciseList.at(i).getName() == tempName){
            tempExercise = exerciseList.at(i);
            break;
        }
    }

    //updates the statistics in the text box
    exerciseListExerciseSelected->setText("Selected: " + QString::fromStdString(tempExercise.getName())
                                        + "\nDescription: " + QString::fromStdString(tempExercise.getDescription())
                                        + "\nCalories Burned: " + QString::number(tempExercise.getCalorieBurn())
                                        + "\nCatagory: " + fromCat(tempExercise.getCategory())
                                        + "\nMuscle Group: " + fromMuscleGroup(tempExercise.getMuscles()));

    //finds teh index of the buttton int eh button vector
    for(int i = 0; i < exerciseButtonList.size(); i++){
        if (exerciseButtonList.at(i)->text().toStdString() == tempName){
            selectedExerciseListIndex = i;
            break;
        }
    }
}

void workoutPlainningUI::addExercisefromWorkout(){
    //returns if none selected
    if(selectedExerciseListIndex == INT_MAX)
        return;
    QPushButton* button = exerciseButtonList.at(selectedExerciseListIndex);

    //erases previous place and conncts and pushes back to new place
    exerciseButtonList.erase(exerciseButtonList.begin()+selectedExerciseListIndex);
    workoutButtonList.push_back(button);
    button->disconnect();
    connect(button, SIGNAL(clicked()), this, SLOT(selectWorkoutExercise()));

    //adds the button widget to the new place
    workoutExerciseLayout->addWidget(button);
    exerciseLayout->removeWidget(button);

    //resets selected text and refNum
    exerciseListExerciseSelected->setText("Exercise Removed");
    selectedExerciseListIndex = INT_MAX;
}

void workoutPlainningUI::removeExerciseFromWorkout(){
    //returns if none selected
    if(selectedWorkoutExerciseIndex == INT_MAX)
        return;
    QPushButton* button = workoutButtonList.at(selectedWorkoutExerciseIndex);

    //erases previous place and conncts and pushes back to new place
    workoutButtonList.erase(workoutButtonList.begin()+selectedWorkoutExerciseIndex);
    exerciseButtonList.push_back(button);
    button->disconnect();
    connect(button, SIGNAL(clicked()), this, SLOT(selectExercise()));

    //reste selected text and reference number
    exerciseLayout->addWidget(button);
    workoutExerciseLayout->removeWidget(button);
    workoutExerciseSelected->setText("Exercise Removed");
    selectedWorkoutExerciseIndex = INT_MAX;
}

void workoutPlainningUI::saveWorkout(){
    //gets workouts from file
    std::vector<Workout> workoutList = Workout::readWorkouts("./workout_Templates.csv");

    //if not new workout then erase old workout
    if(refNum != INT_MAX){
        workoutList.erase(workoutList.begin()+refNum);
    }

    //outputs all of the workouts in the current vector
    std::ofstream workoutFile("./workout_Templates.csv");
    for(int z = 0; z < workoutList.size(); z++){
        Workout temp = workoutList.at(z);
        workoutFile << temp.getName() << ",";
        for(int i = 0; i < temp.getSize(); i++){
            workoutFile << temp.getExerciseAt(i).getName() << ',';
        }
        workoutFile << std::endl;
    }

    //outputs the new custom workout created by user
    workoutFile << workoutName->text().toStdString() << ",";
    for(int i = 0; i < workoutButtonList.size(); i++){
        workoutFile << workoutButtonList.at(i)->text().toStdString() << ",";
    }

    workoutFile.close();
    this->deleteLater();
}

/*
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    workoutPlainningUI b(0);
    b.show();
    return a.exec();
}
*/
