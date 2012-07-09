TEMPLATE = app
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QMAKE_CXXFLAGS += -Wall -Werror
HEADERS += \
    datesequencer.h \
    timeline.h \
    scheduledeventdialog.h \
    calendar.h \
    calendarday.h \
    mainwindow.h
SOURCES += \
   main.cpp \
    datesequencer.cpp \
    timeline.cpp \
    scheduledeventdialog.cpp \
    calendar.cpp \
    calendarday.cpp \
    mainwindow.cpp
FORMS += \
    scheduledeventdialog.ui \
    mainwindow.ui
OTHER_FILES += \
   README.md
TARGET = CashPiles
