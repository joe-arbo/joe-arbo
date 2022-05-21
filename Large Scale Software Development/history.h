/*  history.h    Vewake Vallabhaneni     Virginia Tech       Last Modified: August 29, 2021 
 *  Creates a file called "history.txt" in which the workout completion history is saved to 
 */

#ifndef HISTORY_H
#define HISTORY_H

#include <QWidget>
#include <iostream>
#include <fstream>

namespace Ui {
class History;
}

class History : public QWidget
{
    Q_OBJECT

public:
    explicit History(QWidget *parent = nullptr);
    ~History();

private:
    Ui::History *ui;
};

#endif // HISTORY_H
