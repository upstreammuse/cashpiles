TEMPLATE = app
QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
HEADERS += \
   mainwindow.h \
   transaction.h \
   ynabcsvmodel.h \
   ynabcsvreader.h \
    transactionsplit.h
SOURCES += \
   main.cpp \
   mainwindow.cpp \
   transaction.cpp \
   ynabcsvmodel.cpp \
   ynabcsvreader.cpp \
    transactionsplit.cpp
FORMS += \
   mainwindow.ui
TARGET = CashPiles
