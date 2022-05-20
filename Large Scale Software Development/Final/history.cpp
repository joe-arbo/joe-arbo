/*  history.cpp    Vewake Vallabhaneni     Virginia Tech       Last Modified: August 29, 2021 
 *  Creates a file called "history.txt" in which the workout completion history is saved to 
 */

#include "history.h"
#include "ui_history.h"

History::History(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::History)
{
    ui->setupUi(this);
    std::ifstream file;
    std::string filename("history.txt");
    file.open(filename);
    if(file.is_open())
    {
        std::string temp;
        getline(file, temp);
        while(file)
        {
            ui->plainTextEdit->appendPlainText(QString::fromStdString(temp));
            getline(file, temp);
        }
    }
    file.close();
}

History::~History()
{
    delete ui;
}
