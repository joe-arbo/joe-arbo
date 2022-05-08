/*  Exercise.h    Anna Taylor     Virginia Tech       Last Modified: November 29, 2021 
 *  Data structure to allow for the storing of exercise variables. (Relies on equipment)
 *  Qt creator class.
 */
#ifndef EXERCISE_H
#define EXERCISE_H

#include "EquipmentList.h"
#include <QDebug>

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <fstream>
#include <sstream>
#include <iostream>

enum MuscleGroups {
    NO_MUSCLE_GROUP = -1,
    BACK = 0,
    BICEPS = 1,
    CARDIO = 2,
    CHEST = 3,
    CORE = 4,
    FOREARMS = 5,
    FULLBODY = 6,
    LEGS = 7,
    NECK = 8,
    SHOULDERS = 9,
    TRICEPS = 10,
    WRIST = 11,
};

enum Category {
    NO_CATEGORY = -1,
    NORMAL = 0,
    STRETCH = 1,
};

class Exercise
{
public:
    Exercise();

    Exercise(std::string nam, std::string des, float cal,
             Category cat, MuscleGroups mus,
             std::vector<std::string> equ);

    std::string getName();

    void setName(std::string nam);

    std::string getDescription();

    void setDescription(std::string des);

    float getCalorieBurn();

    void setCalorieBurn(float cal);

    Category getCategory();

    void setCategory(Category cat);

    MuscleGroups getMuscles();

    void setMuscles(MuscleGroups mus);

    std::vector<std::string> getEquipment();

    void setEquipment(std::vector<std::string> equ);

    void addEquipment(std::vector<std::string> equ); //TODO

    void removeEquipment(std::vector<std::string> equ); //TODO

    //static function to create workouts vector
    static std::vector<Exercise> readExercises(std::string fileName);

    static void writeExercises(std::vector<Exercise> ExList, std::string fileName);

    //Mapping
//    static std::map<MuscleGroups, std::string> muscleMap;
//    static std::map<Category, std::string> categoryMap;

private:
    std::string name;

    std::string description;

    float calorieBurnRate;

    Category category;

    MuscleGroups muscles;

    std::vector<std::string> equipment;

    //Mapping
//    std::map<MuscleGroups, std::string> mapMuscles();
//    std::map<Category, std::string> mapCategories();
};

//std::map<Category, std::string> Exercise::categoryMap = Exercise::mapCategories();
//std::map<MuscleGroups, std::string> Exercise::muscleMap = Exercise::mapMuscles();

#endif // EXERCISE_H
