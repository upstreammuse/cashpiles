TEMPLATE = app
QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
HEADERS += \
   account.h \
   accountant.h \
   category.h \
   csvreader.h \
   mainwindow.h \
   transaction.h \
   transactionsplit.h \
   ynabbudget.h \
   ynabregister.h
SOURCES += \
   account.cpp \
   accountant.cpp \
   category.cpp \
   csvreader.cpp \
   main.cpp \
   mainwindow.cpp \
   transaction.cpp \
   transactionsplit.cpp \
   ynabbudget.cpp \
   ynabregister.cpp
FORMS += \
   mainwindow.ui
TARGET = CashPiles
