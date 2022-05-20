/*  Exercise.cpp    Anna Taylor     Virginia Tech       Last Modified: November 29, 2021 
 *  Data structure to allow for the storing of exercise variables. (Relies on equipment)
 *  Qt creator class.
 */

#include "Exercise.h"

Exercise::Exercise()
{
    name = "New Exercise";
    description = "No Description";
    calorieBurnRate = 0;
    category = NO_CATEGORY;
    muscles = NO_MUSCLE_GROUP;
}

Exercise::Exercise(std::string nam, std::string des, float cal,
                   Category cat, MuscleGroups mus,
                   std::vector<std::string> equ)
{
    name = nam;
    description = des;
    calorieBurnRate = cal;
    category = cat;
    muscles = mus;
    equipment = equ;
}

std::string Exercise::getName()
{
    return name;
}

void Exercise::setName(std::string nam)
{
    name = nam;
}

std::string Exercise::getDescription()
{
    return description;
}

void Exercise::setDescription(std::string des)
{
    description = des;
}

float Exercise::getCalorieBurn()
{
    return calorieBurnRate;
}

void Exercise::setCalorieBurn(float cal)
{
    calorieBurnRate = cal;
}

Category Exercise::getCategory()
{
    return category;
}

void Exercise::setCategory(Category cat)
{
    category = cat;
}

MuscleGroups Exercise::getMuscles()
{
    return muscles;
}

void Exercise::setMuscles(MuscleGroups mus)
{
    muscles = mus;
}

std::vector<std::string> Exercise::getEquipment()
{
    return equipment;
}

void Exercise::setEquipment(std::vector<std::string> equ)
{
    equipment = equ;
}

void Exercise::addEquipment(std::vector<std::string> equ)
{
    for (int i = 0; i < equ.size(); i++) {
        equipment.push_back(equ[i]);
    }
}

void Exercise::removeEquipment(std::vector<std::string> equ)
{

    for (int i = 0; i < equ.size(); i++) {
        for (int j = 0; j < equipment.size(); j++) {
            if (equipment[j] == equ[i]) {
                equipment.erase(equipment.begin()+j);
                j--;
            }
        }
    }
}

std::vector<Exercise> Exercise::readExercises(std::string fileName)
{
    std::string fileLine, fileWord;
    std::ifstream readFile(fileName);

    std::vector<Exercise> returnVector;

    //error reading file
    if(!readFile){
        perror("ERROR: Wrong File Name.");
        return returnVector;
    }


    //reads each line of file
    while(std::getline(readFile, fileLine)){
        std::cout << fileLine << std::endl;

        std::istringstream  linePartition(fileLine);

        std::string name_read;
        std::string description_read;
        std::string calorieBurnRate_read;
        std::string category_read;
        std::string muscles_read;
        std::vector<std::string> equipment_read;

        if (std::getline(linePartition, fileWord, ',')) {
            name_read = fileWord;
        }
        if (std::getline(linePartition, fileWord, ',')) {
            description_read = fileWord;
        }
        if (std::getline(linePartition, fileWord, ',')) {
            calorieBurnRate_read = fileWord;
        }
        if (std::getline(linePartition, fileWord, ',')) {
            category_read = fileWord;
        }
        if (std::getline(linePartition, fileWord, ',')) {
            muscles_read = fileWord;
        }
        while (std::getline(linePartition, fileWord, ',')) {
            equipment_read.push_back(fileWord);
        }

        Exercise anExercise = Exercise(name_read, description_read, std::stof(calorieBurnRate_read), static_cast<Category>(std::stoi(category_read)), static_cast<MuscleGroups>(std::stoi(muscles_read)), equipment_read);

        returnVector.push_back(anExercise);
    }

    return returnVector;
}

void Exercise::writeExercises(std::vector<Exercise>ExList, std::string fileName)
{
    qDebug() << "WRITE EX";

    std::ofstream writeFile(fileName);

    //error reading file
    if(!writeFile){
        perror("ERROR: Wrong File Name.");
    }
    else {
        qDebug() << "CORRECT FILE NAME";
        for (int i = 0; i < ExList.size(); i++) {
            writeFile << ExList[i].getName() << "," <<
                         ExList[i].getDescription() << "," <<
                         ExList[i].getCalorieBurn() << "," <<
                         ExList[i].getCategory() << "," <<
                         ExList[i].getMuscles();
            auto Equip = ExList[i].getEquipment();
            for (int j = 0; j < Equip.size(); j++) {
                writeFile << "," << Equip[j];
            }
            writeFile << std::endl;
        }
    }
}


////test main, comment out the "Exercise::" before readExercise()
//int main()
//{
//    auto temp = readExercises("C:/Users/itsdr/Documents/TeamAdrenaline/ECE-4574-Group-Project/exercises.csv");
//    std::cout << temp.size() << std::endl;
//    for (int i = 0; i < temp.size(); i++) {
//        std::cout << "Name: " << temp[i].getName() << ", Description: " << temp[i].getDescription() << ", Calorie Burn: " << temp[i].getCalorieBurn() << ", Category: " << temp[i].getCategory() << ", Muscle Group: " << temp[i].getMuscles();

//        auto equip = temp[i].getEquipment();
//        for (int j = 0; j < equip.size(); j++) {
//            std::cout << ", Equipment[" << j << "]: " << equip[j];
//        }
//        std::cout << std::endl;
//    }

//    return 0;
//}

