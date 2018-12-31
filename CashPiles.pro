TEMPLATE = app
CONFIG += console
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
INCLUDEPATH += include
HEADERS += \
   include/accountbalancer.h \
   include/budgetbalancer.h \
   include/currency.h \
   include/csvreader.h \
   include/daterange.h \
   include/datevalidator.h \
   include/interval.h \
   include/itemprocessor.h \
   include/ledger.h \
   include/ledgeraccountcommand.h \
   include/ledgerbudget.h \
   include/ledgercomment.h \
   include/ledgeritem.h \
   include/ledgertransaction.h \
   include/ledgertransactionentry.h \
   include/nativereader.h \
   include/nativewriter.h \
   include/reserveamountallocator.h \
   include/reservepercentallocator.h \
   include/routineallocator.h \
   include/ynabregisterreader.h
SOURCES += \
   src/accountbalancer.cpp \
   src/budgetbalancer.cpp \
   src/currency.cpp \
   src/csvreader.cpp \
   src/daterange.cpp \
   src/datevalidator.cpp \
   src/interval.cpp \
   src/itemprocessor.cpp \
   src/ledger.cpp \
   src/ledgeraccountcommand.cpp \
   src/ledgerbudget.cpp \
   src/ledgercomment.cpp \
   src/ledgeritem.cpp \
   src/ledgertransaction.cpp \
   src/ledgertransactionentry.cpp \
   src/main.cpp \
   src/nativereader.cpp \
   src/nativewriter.cpp \
   src/reserveamountallocator.cpp \
   src/reservepercentallocator.cpp \
   src/routineallocator.cpp \
   src/ynabregisterreader.cpp
OTHER_FILES += \
   README.md
TARGET = CashPiles
