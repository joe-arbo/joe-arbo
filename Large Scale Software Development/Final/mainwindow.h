/*  mainwindow.h    Joeseph Arbolino     Virginia Tech       Last Modified: November 29, 2021 
 *  Used for debugging timer, unused in final design as it is
 *  Qt creator, using UI form design.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "StopWatch.h"
#include "Timer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_stopwatchButton_clicked();

    void on_timerButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
