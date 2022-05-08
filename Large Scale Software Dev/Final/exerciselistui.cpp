/*  exerciselistui.cpp    Anna Taylor     Virginia Tech       Last Modified: November 29, 2021 
 *  Creates the UI window that allows for exercizes to be created, edited and deleted. (Allows user to adjust paramters)
 *  Qt creator, using UI form design.
 */
#include "exerciselistui.h"
#include "EquipmentList.h"
#include <QDebug>
#include <algorithm>

ExerciseListUi::ExerciseListUi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExerciseListUi)
{
    ui->setupUi(this);

    auto Equipment = (EquipmentList::getInstance())->returnEquipmentNames();

    for (int i = 0; i < Equipment.size(); i++) {
        //qDebug() << QString::fromStdString(Equipment[i]);
        ui->EquipmentNOT->addItem(QString::fromStdString(Equipment[i]));
    }

    ExList = Exercise::readExercises("./exercises.csv");

    ui->ExerciseSelect->clear();
    std::cout << ExList.size() << std::endl;
    for (int i = 0; i < ExList.size(); i++) {
        ui->ExerciseSelect->addItem(QString::fromStdString(ExList[i].getName()));
        //qDebug() << QString::fromStdString(ExList[i].getName());
    }

    ui->ExerciseSelect->setCurrentIndex(0);
}

ExerciseListUi::~ExerciseListUi()
{
    delete ui;
}

void ExerciseListUi::on_ExerciseSelect_currentIndexChanged(int index)
{
    currIndex = index;

    if (ExList.size() != 0 && index >= 0 && index < ExList.size()) {
        //qDebug() << index;
        ui->ExerciseText->setText(QString::fromStdString(ExList[index].getName()));
        ui->ExerciseDesc->setPlainText(QString::fromStdString(ExList[index].getDescription()));
        ui->ExerciseBurnRate->setValue(ExList[index].getCalorieBurn());
        ui->ExerciseCategory->setCurrentIndex(ExList[index].getCategory() + 1);
        ui->ExerciseMuscle->setCurrentIndex(ExList[index].getMuscles() + 1);

        //qDebug() << "Cat: " << ExList[index].getCategory() + 1;
        //qDebug() << "Mus: " << ExList[index].getMuscles() + 1;

        ui->Equipment->clear();
        ui->EquipmentNOT->clear();
        auto EquipList = (EquipmentList::getInstance())->returnEquipmentNames();

        for (int i = 0; i < EquipList.size(); i++) {
            //qDebug() << QString::fromStdString(Equipment[i]);
            ui->EquipmentNOT->addItem(QString::fromStdString(EquipList[i]));
        }

        auto Equip = ExList[index].getEquipment();
        for (int i = 0; i < Equip.size(); i++) {
            qDebug() << "Adding Item to Equip: " << QString::fromStdString(Equip[i]);
            auto equipHave = QString::fromStdString(Equip[i]);
            ui->Equipment->addItem(equipHave);

            //remove entries from EquipmentNOT
            auto numEquip = ui->EquipmentNOT->count();

            for (int i = 0; i < numEquip; i++) {
                auto equipName = ui->EquipmentNOT->item(i)->text();
                if (equipHave == equipName) {
                    //remove from not have list
                    qDebug() << "Removing Item from Equip: " << equipName;
                    ui->EquipmentNOT->takeItem(i);
                    break;
                }
            }
        }
    }
}


void ExerciseListUi::on_SelectEquipment_clicked()
{
    auto Item = ui->EquipmentNOT->currentItem();
    if (Item != nullptr) {
        ui->Equipment->addItem(ui->EquipmentNOT->takeItem(ui->EquipmentNOT->row(Item)));
    }
}


void ExerciseListUi::on_RemoveEquipment_clicked()
{
    auto Item = ui->Equipment->currentItem();
    if (Item != nullptr) {
        ui->EquipmentNOT->addItem(ui->Equipment->takeItem(ui->Equipment->row(Item)));
    }
}

void ExerciseListUi::on_EditExercise_clicked()
{
    if (ExList.size() > 0) {

        auto name = ui->ExerciseText->text().toStdString();
        name.erase(remove(name.begin(), name.end(), ','), name.end());
        auto name_temp = name;
        auto duplicate = 0;
        bool match = false;

        do {
            match = false;
            for (int i = 0; i < ExList.size(); i++) {
                if (currIndex != i && name == ExList[i].getName()) {
                    match = true;
                    duplicate++;
                    name = name_temp + "_" + std::to_string(duplicate);
                    break;
                }
            }
        } while (match == true);

        ExList[currIndex].setName(name);
        ui->ExerciseSelect->setItemText(currIndex, QString::fromStdString(name));
        //Remove Commas from description
        auto descTemp = (ui->ExerciseDesc->toPlainText()).toStdString();
        descTemp.erase(remove(descTemp.begin(), descTemp.end(), ','), descTemp.end());
        ExList[currIndex].setDescription(descTemp);
        ExList[currIndex].setCalorieBurn(ui->ExerciseBurnRate->value());
        ExList[currIndex].setCategory(Category(ui->ExerciseCategory->currentIndex() - 1));
        ExList[currIndex].setMuscles(MuscleGroups(ui->ExerciseMuscle->currentIndex() - 1));

        std::vector<std::string> Equip;
        auto numEquip = ui->Equipment->count();

        for (int i = 0; i < numEquip; i++) {
            auto equipName = ui->Equipment->item(i)->text().toStdString();
            Equip.push_back(equipName);
        }
        ExList[currIndex].setEquipment(Equip);

        this->on_ExerciseSelect_currentIndexChanged(currIndex);
    }
}



//#include <QApplication>

//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    ExerciseListUi w;
//    w.show();
//    return a.exec();
//}

void ExerciseListUi::on_buttonBox_accepted()
{
    qDebug() << "CLICKED SAVE";
    this->on_EditExercise_clicked();
    Exercise::writeExercises(ExList, "./exercises.csv");
}


void ExerciseListUi::on_DeleteExercise_clicked()
{
    if (ExList.size() > 0) {
        ExList.erase(ExList.begin() + currIndex);
        ui->ExerciseSelect->removeItem(currIndex);
    }
}


void ExerciseListUi::on_AddNew_clicked()
{

    auto Exer = Exercise();
    auto name = Exer.getName();
    auto duplicate = 0;
    bool match = false;

    do {
        match = false;
        for (int i = 0; i < ExList.size(); i++) {
            if (Exer.getName() == ExList[i].getName()) {
                match = true;
                duplicate++;
                Exer.setName(name + "_" + std::to_string(duplicate));
                break;
            }
        }
    } while (match == true);

    ExList.push_back(Exer);

    ui->ExerciseSelect->addItem(QString::fromStdString(Exer.getName()));
    ui->ExerciseSelect->setCurrentIndex(ui->ExerciseSelect->count() - 1);
}

