TEMPLATE = app
CONFIG += console
#QT += widgets
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

HEADERS += \
   src/accountbalancer.h \
#   src/budgetbalancer.h \
   src/budgetallocator.h \
   src/currency.h \
   src/daterange.h \
   src/datevalidator.h \
   src/filereader.h \
   src/filewriter.h \
#   src/goalallocator.h \
   src/interval.h \
   src/itemprocessor.h \
   src/ledger.h \
   src/ledgeraccount.h \
   src/ledgerbudget.h \
   src/ledgerbudgetentry.h \
   src/ledgercomment.h \
   src/ledgeritem.h \
   src/ledgertransaction.h \
   src/ledgertransactionentry.h \
#   src/reserveamountallocator.h \

SOURCES += \
   src/accountbalancer.cpp \
#   src/budgetbalancer.cpp \
   src/budgetallocator.cpp \
   src/cashpilesdriver.cpp \
   src/currency.cpp \
   src/daterange.cpp \
   src/datevalidator.cpp \
   src/filereader.cpp \
   src/filewriter.cpp \
#   src/goalallocator.cpp \
   src/interval.cpp \
   src/itemprocessor.cpp \
   src/ledger.cpp \
   src/ledgeraccount.cpp \
   src/ledgerbudget.cpp \
   src/ledgerbudgetentry.cpp \
   src/ledgercomment.cpp \
   src/ledgeritem.cpp \
   src/ledgertransaction.cpp \
   src/ledgertransactionentry.cpp \
#   src/reserveamountallocator.cpp \

TARGET = cashpiles
