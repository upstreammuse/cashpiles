TEMPLATE = lib
QT -= gui
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
HEADERS += \
   account.h \
   accountbalance.h \
   accountclosure.h \
   budgetperiod.h \
   category.h \
   currency.h \
   interval.h \
   transaction.h \
   transactionentry.h
SOURCES += \
   account.cpp \
   accountbalance.cpp \
   accountclosure.cpp \
   budgetperiod.cpp \
   category.cpp \
   currency.cpp \
   interval.cpp \
   transaction.cpp \
   transactionentry.cpp
TARGET = model
