/*  ElapsedTime.h    Joseph Arbolino     Virginia Tech       Last Modified: August 29, 2021 
 *  Debugging class that kept track of time elapsed
 */

#ifndef TIMER_H
#define TIMER_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDateTime>
#include <QTime>
#include <QTimer>
#include <QLabel>
#include <iostream>

class Timer : public QWidget
{
    Q_OBJECT
public:
    explicit Timer(QWidget * parent = 0)
        : QWidget(parent)
        , mRunning(true)
        , timer(new QTimer(parent))
        , mStartTime()
        , mLabel(new QLabel("00:00:00"))
        , timerButton(new QPushButton("Pause"))
        , hour(0)
        , min(0)
        , sec(0)
    {
        QHBoxLayout * hBoxLayout = new QHBoxLayout(this);
        QFont f( "Arial", 32, QFont::Bold);
        mLabel->setFont(f);

        hBoxLayout->addWidget(timerButton);
        hBoxLayout->addWidget(mLabel);

        mLabel->setText(mStartTime.toString("hh:mm:ss"));
        connect(timer, SIGNAL(timeout()), SLOT(countdown()));
        connect(timerButton, SIGNAL(clicked()), SLOT(toggle()));

        timer->start(1000);
    }

    void setTime(int h, int m, int s)
    {
        hour = h;
        min = m;
        sec = s;
        mStartTime.setHMS(hour, min, sec);
        mLabel->setText(mStartTime.toString("hh:mm:ss"));
    }

public slots:
    void toggle(void)
    {
        if (!mRunning)
        {
            mRunning = true;
            timerButton->setText("Pause");
        }
        else if (mRunning)
        {
            mRunning = false;
            timerButton->setText("Resume");
        }
    }

    void countdown(void)
    {
        if (mRunning)
        {
            mStartTime = mStartTime.addSecs(-1);
            mLabel->setText(mStartTime.toString("hh:mm:ss"));
            if (mLabel->text() == "00:00:00")
            {
                mRunning = false;
                //send signal here if needed
            }
        }
    }

private:
    bool mRunning;
    QTimer *timer;
    QTime mStartTime;
    QLabel * mLabel;
    QPushButton * timerButton;
    int hour;
    int min;
    int sec;
};

#endif // TIMER_H
