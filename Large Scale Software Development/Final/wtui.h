/*  wtui.h    Vewake Vallabhaneni    Virginia Tech       Last Modified: November 29, 2021 
 *  This is the header file for themain window of the Workout Planning application. Acts as a central hub for all other widgets.
 *  Qt creator, using UI form design.
 */
#ifndef WTUI_H
#define WTUI_H

#include <QMainWindow>
#include <QDate>
#include "liveworkout.h"
#include "workoutplanningui.h"
#include "workoutlistingui.h"
#include "history.h"
#include "exerciselistui.h"
#include "warning.h"

QT_BEGIN_NAMESPACE
namespace Ui { class wtUI; }
QT_END_NAMESPACE

class wtUI : public QMainWindow
{
    Q_OBJECT

public:
    wtUI(QWidget *parent = nullptr);
    ~wtUI();

private slots:
    void on_calendarWidget_clicked(const QDate &date);

    void on_startWorkout_clicked();

    void on_newPlan_clicked();

    void on_history_clicked();

    void on_actionClear_Plan_triggered();

    void on_actionClear_History_triggered();

    void on_actionExercises_triggered();

    void workoutSelected(QAction* actions);

private:
    Ui::wtUI *ui;
    Workout *w = nullptr;
    LiveWorkout *lw = nullptr;
    WorkoutListingUI *wl = nullptr;
    History *h = nullptr;
    workoutPlainningUI *wp = nullptr;
};
#endif // WTUI_H
