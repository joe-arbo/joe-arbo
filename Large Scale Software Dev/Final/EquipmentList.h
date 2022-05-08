/*  EquipmentList.h    Anna Taylor     Virginia Tech       Last Modified: November 29, 2021 
 *  A singleton class that allows for the storage of all available equipment or unowned equipment. Used mainly in Exercise Data structure and related files
 *  Qt creator class.
 */
#ifndef EQUIPMENTLIST_H
#define EQUIPMENTLIST_H

#include <map>
#include <vector>
#include <string>

//TODO:
//SINGLETON
class EquipmentList
{
private:
    //One Time Pad vector
    std::map<std::string,  bool> equipment;

    //Pointer to Equipment List
    static EquipmentList* equipmentListInstance;

protected:
    //contructor is PRIVATE, cannot call publically
    EquipmentList();

public:
    //getInstance will create or return the already existing singleton
    static EquipmentList* getInstance();
    //returns the Full Equipment List (and access status)
    std::map<std::string,  bool> getEquipmentList();
    //sets the Equipment List
    void setEquipmentList(std::map<std::string,  bool> list);
    //adds to the Equipment List and the access status
    void addEquipment(std::string name, bool owned);
    //searches for equipment status (Owned or unowned)
    bool searchEquipmentStatus(std::string name);
    //changes an equipment status (Owned or unowned)
    void changeEquipmentStatus(std::string name);
    //get ALL names of equipment
    std::vector<std::string> returnEquipmentNames();
    //get OWNED(true) or UNOWNED(false) names of equipment
    std::vector<std::string> returnEquipmentNames(bool owned);

};

#endif // EQUIPMENTLIST_H
