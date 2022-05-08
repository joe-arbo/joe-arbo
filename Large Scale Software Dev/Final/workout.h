/*  workout.h    Andrew Beauchemin     Virginia Tech       Last Modified: August 29, 2021
 *  workout file that stores the name, exercise list, and completion times
 */

#ifndef WORKOUT_H
#define WORKOUT_H

#include <vector>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Exercise.h"

class Workout
{
private:
    std::vector<Exercise> exerciseList;
    std::vector<float> completionTimes;
    std::string workoutName;

public:
    //default and paratized constructor
    Workout();
    Workout(std::vector<Exercise> exerciseList, std::vector<float> completionTimes, std::string workoutName);

    std::string getName();
    void setName(std::string workoutName);


    //time retrieval funcitons
    float getBestTime();
    float getAvgTime();
    void pushBackTime(float time);

    //exercise retrieval functioins
    Exercise getExerciseAt(int ref);
    void setExerciseAt(int ref, Exercise exc);
    std::vector<Exercise> getExercises();
    void pushBackExercise(Exercise e);
    void removeExercise(int ref);
    std::vector<float> getCompletionTimes();
    int getSize();
    bool isEmpty();

    //static function to create workouts vector
    static std::vector<Workout> readWorkouts(std::string fileName);

};

#endif // WORKOUT_H
