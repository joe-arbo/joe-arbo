/*  main.cpp    Team Adrenaline     Virginia Tech       Last Modified: August 29, 2020 
 *  This is the main funciton for the project, it just runs wtUI, which is the main UI
 */

#include "wtui.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    wtUI w;
    w.show();
    return a.exec();
}
