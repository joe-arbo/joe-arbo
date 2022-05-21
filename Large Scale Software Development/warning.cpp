/*  warning.cpp    Vewake Vallabhaneni     Virginia Tech       Last Modified: November 29, 2021 
 *  Provides a warning pop up to bad user input (No workout planned, and attempted to start)
 *  Qt creator, using UI form design.
 */
#include "warning.h"
#include "ui_warning.h"

Warning::Warning(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Warning)
{
    ui->setupUi(this);
}

Warning::~Warning()
{
    delete ui;
}
