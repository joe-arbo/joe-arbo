/*  EquipmentList.cpp    Anna Taylor     Virginia Tech       Last Modified: November 29, 2021 
 *  A singleton class that allows for the storage of all available equipment or unowned equipment. Used mainly in Exercise Data structure and related files
 *  Qt creator class.
 */
enum Equipment{
    ASSISTED,
    BAND,
    BARBELL,
    BODY_WEIGHT,
    BOSU_BALL,
    CABLE,
    DUMBELL,
    EZ_BARBELL,
    HAMMER,
    KETTLEBELL,
    LEVERAGE_MACHINE,
    MEDICINE_BALL,
    OLYMPIC_BARBELL,
    RESISTENCE_BAND,
    ROLL,
    ROPE,
    SLED_MACHINE,
    SMITH_MACHINE,
    STABILITY_BALL,
    SUSPENSION,
    TRAP_BAR,
    WEIGHTED,
    WHEEL_ROLLER,
    NO_EQUIPMENT
};
#include "EquipmentList.h"

EquipmentList* EquipmentList::equipmentListInstance = NULL;

//PRIVATE singleton contructor
EquipmentList::EquipmentList()
{
    //    Assumes that all of this is owned!

    //    DEFAULT EQUIPMENT:
    //    ASSISTED,
    //    BAND,
    //    BARBELL,
    //    BODY_WEIGHT,
    //    BOSU_BALL,
    //    CABLE,
    //    DUMBELL
    //    EZ_BARBELL,
    //    HAMMER,
    //    KETTLEBELL,
    //    LEVERAGE_MACHINE,
    //    MEDICINE_BALL,
    //    OLYMPIC_BARBELL,
    //    RESISTENCE_BAND,
    //    ROLL,
    //    ROPE,
    //    SLED_MACHINE,
    //    SMITH_MACHINE,
    //    STABILITY_BALL,
    //    SUSPENSION,
    //    TRAP_BAR,
    //    WEIGHTED,
    //    WHEEL_ROLLER,
    //    NO_EQUIPMENT

    equipment.insert(std::pair<std::string, bool>("Assisted", true));
    equipment.insert(std::pair<std::string, bool>("Band", true));
    equipment.insert(std::pair<std::string, bool>("Barbell", true));
    equipment.insert(std::pair<std::string, bool>("Body Weight", true));
    equipment.insert(std::pair<std::string, bool>("Bosu Ball", true));
    equipment.insert(std::pair<std::string, bool>("Cable", true));
    equipment.insert(std::pair<std::string, bool>("Dumbell", true));
    equipment.insert(std::pair<std::string, bool>("Ez Barbell", true));
    equipment.insert(std::pair<std::string, bool>("Hammer", true));
    equipment.insert(std::pair<std::string, bool>("Kettlebell", true));
    equipment.insert(std::pair<std::string, bool>("Leverage Machine", true));
    equipment.insert(std::pair<std::string, bool>("Medicine Ball", true));
    equipment.insert(std::pair<std::string, bool>("Olympic_Barbell", true));
    equipment.insert(std::pair<std::string, bool>("Resistence Band", true));
    equipment.insert(std::pair<std::string, bool>("Roll", true));
    equipment.insert(std::pair<std::string, bool>("Rope", true));
    equipment.insert(std::pair<std::string, bool>("Sled Machine", true));
    equipment.insert(std::pair<std::string, bool>("Smith Machine", true));
    equipment.insert(std::pair<std::string, bool>("Stability Ball", true));
    equipment.insert(std::pair<std::string, bool>("Suspension", true));
    equipment.insert(std::pair<std::string, bool>("Trap Bar", true));
    equipment.insert(std::pair<std::string, bool>("Weighted", true));
    equipment.insert(std::pair<std::string, bool>("Wheel Roller", true));
    equipment.insert(std::pair<std::string, bool>("No Equipment", true));
}

//Method to retrieve singleton object
EquipmentList* EquipmentList::getInstance()
{
    if (equipmentListInstance == NULL) { //if a singleton object doesnt already exist, create one
        equipmentListInstance = new EquipmentList();
    }

    return equipmentListInstance;
}

std::map<std::string, bool> EquipmentList::getEquipmentList()
{
    return equipment;
}

void EquipmentList::setEquipmentList(std::map<std::string, bool> list)
{
    equipment = list;
}

void EquipmentList::addEquipment(std::string name, bool owned)
{
    equipment.insert(std::pair<std::string, bool>(name, owned));
}

//will return false if name is not found and the new equipment name will be added to the list
bool EquipmentList::searchEquipmentStatus(std::string name)
{
    return equipment[name];
}

void EquipmentList::changeEquipmentStatus(std::string name)
{
    auto itr = equipment.find(name);
    if (itr != equipment.end()) {
        itr->second = !itr->second;
    }
}

std::vector<std::string> EquipmentList::returnEquipmentNames()
{
    std::vector<std::string> output = std::vector<std::string>();
    for( auto itr = equipment.begin(); itr != equipment.end(); ++itr){
        output.push_back(itr->first);
    }

    return output;
}

std::vector<std::string> EquipmentList::returnEquipmentNames(bool owned)
{
    std::vector<std::string> output = std::vector<std::string>();
    for( auto itr = equipment.begin(); itr != equipment.end(); ++itr){
        if (itr->second == owned) {
            output.push_back(itr->first);
        }
    }

    return output;
}

