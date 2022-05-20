/*  workoutrecap.cpp    Anna Taylor   Virginia Tech       Last Modified: November 10, 2021 
 *  This is a file for the workout planning recap application. Provides methods to allow for a workout summary with details.
 *  Qt creator, using UI form design.
 */
#include "workoutrecap.h"

WorkoutRecap::WorkoutRecap() {}

WorkoutRecap::WorkoutRecap(Workout inputWorkout, std::string date, std::vector<int> skipped, std::vector<std::pair<int, Exercise>> modified, std::vector<std::pair<int, Exercise>> added, std::vector<std::pair<int,std::string>> notes_in)
{
    workoutUsed = inputWorkout;
    auto times = workoutUsed.getCompletionTimes();
    for (int i = 0; i < times.size(); i++) {
        totalTime += times[i];
        caloriesBurned += workoutUsed.getExercises()[i].getCalorieBurn()*times[i];
        auto previouslyUsed = false;
        for (int j = 0; j < muscleGroupsUsed.size(); j++) {
            if (muscleGroupsUsed[j].first == workoutUsed.getExercises()[i].getMuscles()) {
                muscleGroupsUsed[j].second += times[i];
                previouslyUsed = true;
            }
        }
        if (previouslyUsed == false) {
            muscleGroupsUsed.push_back(std::pair<MuscleGroups, float>(workoutUsed.getExercises()[i].getMuscles(), times[i]));
        }
    }
    dateCompleted = date;
    exercisesSkipped = skipped;
    exercisesModified = modified;
    exercisesAdded = added;
    notes = notes_in;
}

std::vector<std::pair<float, float> > WorkoutRecap::getCalorieBurnOverTime()
{
    std::vector<std::pair<float, float>> result;

    for (int i = 0; i < workoutUsed.getCompletionTimes().size(); i++) {
        result.push_back(std::pair<float,float>(workoutUsed.getExercises()[i].getCalorieBurn()*workoutUsed.getCompletionTimes()[i] ,workoutUsed.getCompletionTimes()[i]));
    }

    return result;
}

float WorkoutRecap::getCalorieBurnTotal()
{
    return caloriesBurned;
}

std::vector<std::pair<MuscleGroups, float> > WorkoutRecap::getTimePerMuscle()
{
    return muscleGroupsUsed;
}

float WorkoutRecap::getTimeTotal()
{
    return totalTime;
}

std::vector<Exercise> WorkoutRecap::getOriginalPlan()
{
    return workoutUsed.getExercises();
}

std::vector<int> WorkoutRecap::getSkippedExercises()
{
    return exercisesSkipped;
}

std::vector<std::pair<int, Exercise> > WorkoutRecap::getAddedExercises()
{
    return exercisesAdded;
}
