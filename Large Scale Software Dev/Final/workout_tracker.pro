QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    EquipmentList.cpp \
    Exercise.cpp \
    calendar.cpp \
    exerciselistui.cpp \
    history.cpp \
    liveworkout.cpp \
    main.cpp \
    mainwindow.cpp \
    recap.cpp \
    warning.cpp \
    workout.cpp \
    workoutlisting.cpp \
    workoutlistingui.cpp \
    workoutplanningui.cpp \
    workoutrecap.cpp \
    wtui.cpp

HEADERS += \
    ElapsedTime.h \
    EquipmentList.h \
    Exercise.h \
    StopWatch.h \
    Timer.h \
    calendar.h \
    exerciselistui.h \
    history.h \
    json.hpp \
    liveworkout.h \
    mainwindow.h \
    recap.h \
    warning.h \
    workout.h \
    workoutlisting.h \
    workoutlistingui.h \
    workoutplanningui.h \
    workoutrecap.h \
    wtui.h

FORMS += \
    exerciselistui.ui \
    history.ui \
    mainwindow.ui \
    recap.ui \
    warning.ui \
    workoutlisting.ui \
    wtui.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    exercises.csv \
    workout_Templates.csv \
    workout_tracker.pro.user \
    workout_tracker.pro.user.08cb3d5
