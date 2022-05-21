/*  exerciselistui.h    Anna Taylor    Virginia Tech       Last Modified: November 29, 2021 
 *  Creates the UI window that allows for exercizes to be created, edited and deleted. (Allows user to adjust paramters)
 *  Qt creator, using UI form design.
 */
#ifndef EXERCISELISTUI_H
#define EXERCISELISTUI_H

#include <QDialog>

#include "ui_exerciselistui.h"
#include "Exercise.h"
#include <iostream>

namespace Ui {
class ExerciseListUi;
}

class ExerciseListUi : public QDialog
{
    Q_OBJECT

public:
    explicit ExerciseListUi(QWidget *parent = nullptr);
    ~ExerciseListUi();

private slots:
    void on_ExerciseSelect_currentIndexChanged(int index);

    void on_SelectEquipment_clicked();

    void on_RemoveEquipment_clicked();

    void on_EditExercise_clicked();

    void on_buttonBox_accepted();

    void on_DeleteExercise_clicked();

    void on_AddNew_clicked();

private:
    Ui::ExerciseListUi *ui;
    std::vector<Exercise> ExList;
    int currIndex;
};

#endif // EXERCISELISTUI_H
