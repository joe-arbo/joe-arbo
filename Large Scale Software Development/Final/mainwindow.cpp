/*  mainwindow.cpp    Joeseph Arbolino     Virginia Tech       Last Modified: November 29, 2021 
 *  Used for debugging timer, unused in final design as it is
 *  Qt creator, using UI form design.
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_stopwatchButton_clicked()
{
    StopWatch *q = new StopWatch();
    q->setWindowTitle("Stopwatch");
    q->show();
}


void MainWindow::on_timerButton_clicked()
{
    Timer *t = new Timer();
    t->setWindowTitle("Timer");
    t->setTime(ui->hourSpinBox->value(), ui->minSpinBox->value(), ui->secSpinBox->value());
    t->show();
}

