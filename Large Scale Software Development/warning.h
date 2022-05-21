/*  warning.h    Vewake Vallabhaneni     Virginia Tech       Last Modified: November 29, 2021 
 *  Provides a warning pop up to bad user input (No workout planned, and attempted to start)
 *  Qt creator, using UI form design.
 */

#ifndef WARNING_H
#define WARNING_H

#include <QDialog>

namespace Ui {
class Warning;
}

class Warning : public QDialog
{
    Q_OBJECT

public:
    explicit Warning(QWidget *parent = nullptr);
    ~Warning();

private:
    Ui::Warning *ui;
};

#endif // WARNING_H
