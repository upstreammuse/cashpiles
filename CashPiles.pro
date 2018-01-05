TEMPLATE = app
QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
HEADERS += \
   csvreader.h \
   mainwindow.h \
   transaction.h \
   transactionsplit.h \
   ynabcsvmodel.h
SOURCES += \
   csvreader.cpp \
   main.cpp \
   mainwindow.cpp \
   transaction.cpp \
   transactionsplit.cpp \
   ynabcsvmodel.cpp
FORMS += \
   mainwindow.ui
TARGET = CashPiles
