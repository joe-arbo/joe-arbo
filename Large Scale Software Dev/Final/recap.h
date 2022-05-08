/*  recap.h    Joeseph Arbolino     Virginia Tech       Last Modified: November 29, 2021 
 *  provides a basic workout recap (minimal info)
 *  Qt creator, using UI form design.
 */
#ifndef RECAP_H
#define RECAP_H

#include <QWidget>

namespace Ui {
class Recap;
}

class Recap : public QWidget
{
    Q_OBJECT

public:
    explicit Recap(QWidget *parent = nullptr);
    ~Recap();
    void setTime(QString time);
    void setDescription(std::string Description);

private:
    Ui::Recap *ui;
};

#endif // RECAP_H
