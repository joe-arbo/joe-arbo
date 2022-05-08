/*  workout.cpp    Andrew Beauchemin     Virginia Tech       Last Modified: August 29, 2021
 *  workout file that stores the name, exercise list, and completion times
 */

#include "workout.h"

Workout::Workout()
{
    workoutName = "Default Workout";
    exerciseList = std::vector<Exercise>();
    completionTimes = std::vector<float>();
}

Workout::Workout(std::vector<Exercise> exerciseList, std::vector<float> completionTimes, std::string workoutName){
    this->exerciseList = exerciseList;
    this->completionTimes = completionTimes;
    this->workoutName = workoutName;
}

std::string Workout::getName(){
    return this->workoutName;
}

void Workout::setName(std::string workoutName){
    this->workoutName = workoutName;
}

//returns best time using max_element
float Workout::getBestTime(){
    if(completionTimes.empty())
        return 0;
    else
        return *std::max_element(completionTimes.begin(), completionTimes.end());
}

//returns avg time using sum over size
float Workout::getAvgTime(){
    if(completionTimes.empty())
        return 0;
    else
        return (float) std::accumulate(completionTimes.begin(), completionTimes.end(), 0) / completionTimes.size();
}

//pushes back time
void Workout::pushBackTime(float time){
    completionTimes.push_back(time);
}

Exercise Workout::getExerciseAt(int ref){
    if(ref > exerciseList.size())
        return Exercise();
    else
        return exerciseList.at(ref);
}

void Workout::setExerciseAt(int ref, Exercise exc){
    if(ref > exerciseList.size())
        return;
    else
        exerciseList.at(ref) = exc;
}

std::vector<Exercise> Workout::getExercises()
{
    return exerciseList;
}

void Workout::pushBackExercise(Exercise e){
    exerciseList.push_back(e);
}

void Workout::removeExercise(int ref){
    if(ref >= exerciseList.size())
        return;
    else
        exerciseList.erase(exerciseList.begin() + ref);
    return;
}

std::vector<float> Workout::getCompletionTimes()
{
    return completionTimes;
}

int Workout::getSize(){
    return (int) exerciseList.size();
}

bool Workout::isEmpty(){
    return exerciseList.empty();
}

std::vector<Workout> Workout::readWorkouts(std::string fileName){
    std::string fileLine;
    std::ifstream readFile(fileName);

    std::vector<Workout> returnVector;
    //error reading file
    if(!readFile){
        perror("ERROR: Wrong File Name.");
        return returnVector;
    }


    //reads each line of file
    while(std::getline(readFile, fileLine)){
        std::vector<Exercise> exerciseVector;
        exerciseVector.clear();
        std::istringstream  linePartition(fileLine);
        std::string partitionPortion;

        //first line is workout name
        std::string workoutName;
        std::getline(linePartition, workoutName, ',');


        //all remaining lines are exercises
        while(std::getline(linePartition, partitionPortion, ',')){
            std::istringstream exercisePartition(partitionPortion);
            std::string exerciseTemp;
            Exercise tempExercise;

            //extracts name from exercise
            std::getline(exercisePartition, exerciseTemp, ':');
            tempExercise.setName(exerciseTemp);

            //extracts body part from exercise
            std::getline(exercisePartition, exerciseTemp, ':');

            if(tempExercise.getName() != "")
            {
                exerciseVector.push_back(tempExercise);
            }

        }
        //pushes back a constructed workout routine from exercise list
        returnVector.push_back(Workout(exerciseVector, std::vector<float>(), workoutName));
    }

    readFile.close();
    return returnVector;
}

//test main
/*
int main(int argc, char *argv[])
{
    Workout::readWorkouts("C:/Users/Andrew/Documents/ECE-4574-Group-Project/workout_Templates.csv");
    return 0;
}
*/
