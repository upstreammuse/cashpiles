TEMPLATE = lib
QT -= gui
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
DEPENDPATH += ../../include/model
INCLUDEPATH += ../../include
HEADERS += \
   ../../include/model/account.h \
   ../../include/model/accountbalance.h \
   ../../include/model/accountclosure.h \
   ../../include/model/budgetperiod.h \
   ../../include/model/category.h \
   ../../include/model/currency.h \
   ../../include/model/interval.h \
   ../../include/model/model.h \
   ../../include/model/transaction.h \
   ../../include/model/transactionentry.h
SOURCES += \
   account.cpp \
   accountbalance.cpp \
   accountclosure.cpp \
   budgetperiod.cpp \
   category.cpp \
   currency.cpp \
   interval.cpp \
   model.cpp \
   transaction.cpp \
   transactionentry.cpp
TARGET = model
