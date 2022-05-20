/*  recap.h    Joeseph Arbolino     Virginia Tech       Last Modified: November 29, 2021 
 *  provides a basic workout recap (minimal info)
 *  Qt creator, using UI form design.
 */
#include "recap.h"
#include "ui_recap.h"
#include <QTime>

Recap::Recap(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Recap)
{
    ui->setupUi(this);
}

Recap::~Recap()
{
    delete ui;
}

void Recap::setTime(QString time)
{
    ui->totalTime->setPlainText("Workout Time\n" + time);
}

void Recap::setDescription(std::string description)
{
    std::string temp = description + " completed at ";
    QTime tempTime = QTime::currentTime();
    QString time = tempTime.toString("h:mm a");
    ui->description->setPlainText(QString::fromStdString(temp)+time);
}
