/*  workoutrecap.h    Anna Taylor   Virginia Tech       Last Modified: November 10, 2021 
 *  This is the header file for the workout planning recap application. Provides methods to allow for a workout summary with details.
 *  Qt creator, using UI form design.
 */

#ifndef WORKOUTRECAP_H
#define WORKOUTRECAP_H

#include <string>
#include <vector>

#include "workout.h"
#include "Exercise.h"

class WorkoutRecap
{
public:
    WorkoutRecap();
    WorkoutRecap(Workout inputWorkout, std::string date, std::vector<int> skipped, std::vector<std::pair<int, Exercise>> modified, std::vector<std::pair<int, Exercise>> added, std::vector<std::pair<int,std::string>> notes_in);
    //Pair(Calories,TimeBurnedOver)
    std::vector<std::pair<float, float>> getCalorieBurnOverTime();
    float getCalorieBurnTotal();
    std::vector<std::pair<MuscleGroups, float>> getTimePerMuscle();
    float getTimeTotal();
    std::vector<Exercise> getOriginalPlan();
    std::vector<int> getSkippedExercises();
    std::vector<std::pair<int, Exercise>> getAddedExercises();

private:
    Workout workoutUsed;
    float totalTime;
    std::string dateCompleted;
    float caloriesBurned;
    std::vector<int> exercisesSkipped;
    //denotes which exercise was modified (or removed), and the modified exercise (default exercise if removed)
    std::vector<std::pair<int, Exercise>> exercisesModified;
    //denotes after which exercise a new exercise was added (-1 if at start), and the exercise
    std::vector<std::pair<int, Exercise>> exercisesAdded;
    //denotes muscle groups used, and how long each was worked
    std::vector<std::pair<MuscleGroups, float>> muscleGroupsUsed;
    //denotes which exercise a note was made at (-1 if general notes)
    std::vector<std::pair<int,std::string>> notes;
};

#endif // WORKOUTRECAP_H
