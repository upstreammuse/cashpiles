TEMPLATE = lib
CONFIG += dll
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
HEADERS += \
   currency.h \
   daterange.h \
   interval.h \
   itemprocessor.h \
   ledger.h \
   ledgeraccountcommand.h \
   ledgerbudget.h \
   ledgercomment.h \
   ledgeritem.h \
   ledgertransaction.h \
   ledgertransactionentry.h
SOURCES += \
   currency.cpp \
   daterange.cpp \
   itemprocessor.cpp \
   interval.cpp \
   ledger.cpp \
   ledgeraccountcommand.cpp \
   ledgerbudget.cpp \
   ledgercomment.cpp \
   ledgeritem.cpp \
   ledgertransaction.cpp \
   ledgertransactionentry.cpp
TARGET = cp-kernel
