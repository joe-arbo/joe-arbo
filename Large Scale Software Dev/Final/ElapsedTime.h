/*  ElapsedTime.h    Joseph Arbolino     Virginia Tech       Last Modified: August 29, 2021 
 *  Records the time elapsed for each of exercises and dispalys taht information to the user
 */


#ifndef ELAPSEDTIME_H
#define ELAPSEDTIME_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDateTime>
#include <QTime>
#include <QLabel>
#include <iostream>

class ElapsedTime : public QWidget
{
    Q_OBJECT
public:
    explicit ElapsedTime(QWidget * parent = 0)
        : QWidget(parent)
        , mRunning(false)
        , mStartTime()
        , mLabel(new QLabel("000:00:00:000"))
        , lapLabel(new QLabel(""))
        , zero('0')
    {
        QHBoxLayout * hBoxLayout = new QHBoxLayout(this);
        QPushButton * startButton = new QPushButton("Start");
        QPushButton * lapButton = new QPushButton("Lap");
        QPushButton * stopButton = new QPushButton("Stop");

        QFont f( "Arial", 14, QFont::Normal);
        QFont lapFont("Arial", 10, QFont::Bold);
        mLabel->setFont(f);
        lapLabel->setFont(lapFont);

        hBoxLayout->addWidget(mLabel);
        hBoxLayout->addWidget(lapLabel);

        connect(startButton, SIGNAL(clicked()), SLOT(start()));
        connect(lapButton, SIGNAL(clicked()), SLOT(lap()));
        connect(stopButton, SIGNAL(clicked()), SLOT(stop()));

        startTimer(0);
    }

    QString timeElapsed()
    {
        qint64 ms = mStartTime.msecsTo(QTime::currentTime());
        int h = ms / 1000 / 60 / 60;
        int m = (ms / 1000 / 60) - (h * 60);
        return QString("%1 hours, %2 minutes").
                        arg(h, 1, 10, zero).
                        arg(m, 2, 10, zero);
    }

public slots:
    void start(void)
    {
        mStartTime = QTime::currentTime();
        mRunning = true;
    }

    void lap(void)
    {
        QString lapTime = "Lap Time: " + mLabel->text();
        lapLabel->setText(lapTime);
    }

    void stop(void)
    {
        mRunning = false;
    }

protected:
    void timerEvent(QTimerEvent *)
    {
        if(mRunning)
        {
            qint64 ms = mStartTime.msecsTo(QTime::currentTime());
            int h = ms / 1000 / 60 / 60;
            int m = (ms / 1000 / 60) - (h * 60);
            int s = (ms / 1000) - (m * 60);
            ms = ms - (s * 1000) - (m * 60000) - (h * 3600000);
            QString diff = QString("%1:%2:%3:%4").
                                arg(h, 3, 10, zero).
                                arg(m, 2, 10, zero).
                                arg(s, 2, 10, zero).
                                arg(ms, 3, 10, zero);
            mLabel->setText(diff);
        }
    }

private:
    bool mRunning;
    QTime mStartTime;
    QLabel * mLabel;
    QLabel * lapLabel;
    const QChar zero;
};

#endif // ELAPSEDTIME_H
