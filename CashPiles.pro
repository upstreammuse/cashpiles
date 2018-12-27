TEMPLATE = app
CONFIG += console
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
INCLUDEPATH += include
HEADERS += \
   include/accountbalancer.h \
   include/budgetbalancer.h \
   include/currency.h \
   include/csvreader.h \
   include/datevalidator.h \
   include/itemprocessor.h \
   include/ledger.h \
   include/ledgeraccountcommand.h \
   include/ledgerallocation.h \
   include/ledgercomment.h \
   include/ledgeritem.h \
   include/ledgertransaction.h \
   include/ledgertransactionentry.h \
   include/nativereader.h \
   include/nativewriter.h \
   include/ynabbudgetreader.h \
   include/ynabconsolidator.h \
   include/ynabregisterreader.h
SOURCES += \
   src/accountbalancer.cpp \
   src/budgetbalancer.cpp \
   src/currency.cpp \
   src/csvreader.cpp \
   src/datevalidator.cpp \
   src/itemprocessor.cpp \
   src/ledger.cpp \
   src/ledgeraccountcommand.cpp \
   src/ledgerallocation.cpp \
   src/ledgercomment.cpp \
   src/ledgeritem.cpp \
   src/ledgertransaction.cpp \
   src/ledgertransactionentry.cpp \
   src/main.cpp \
   src/nativereader.cpp \
   src/nativewriter.cpp \
   src/ynabbudgetreader.cpp \
   src/ynabconsolidator.cpp \
   src/ynabregisterreader.cpp
OTHER_FILES += \
   CashPiles_File_Format.txt \
   cp-sample.txt \
   README.md
TARGET = CashPiles
