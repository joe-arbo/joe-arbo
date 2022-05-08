/*  workoutlisting.h    Vewake Vallabhaneni     Virginia Tech       Last Modified: August 29, 202
 *  UI file to accomidate the workoutlistingui, uses a combination of ui file and explicit delcration
 *  of QT parts in cpp and h files
 */

#ifndef WORKOUTLISTING_H
#define WORKOUTLISTING_H

#include <QWidget>

namespace Ui {
class WorkoutListing;
}

class WorkoutListing : public QWidget
{
    Q_OBJECT

public:
    explicit WorkoutListing(QWidget *parent = nullptr);
    ~WorkoutListing();

private:
    Ui::WorkoutListing *ui;
};

#endif // WORKOUTLISTING_H
