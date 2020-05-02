TEMPLATE = app
CONFIG -= qt
CONFIG += console
HEADERS += \
   src/cashpiles.h \
   src/csvreader.h \
   src/currency.h \
   src/date.h \
   src/daterange.h \
   src/filereader.h \
   src/filewriter.h \
   src/identifier.h \
   src/interval.h \
   src/ipaccountbalancer.h \
   src/ipbudgetallocator.h \
   src/ipdatevalidator.h \
   src/iptransactioncategorizer.h \
   src/iptransferbalancer.h \
   src/itemprocessor.h \
   src/ledger.h \
   src/ledgeraccount.h \
   src/ledgerblank.h \
   src/ledgerbudget.h \
   src/ledgerbudgetcancelentry.h \
   src/ledgerbudgetcloseentry.h \
   src/ledgerbudgetentry.h \
   src/ledgerbudgetgoalentry.h \
   src/ledgerbudgetgoalsentry.h \
   src/ledgerbudgetincomeentry.h \
   src/ledgerbudgetreserveamountentry.h \
   src/ledgerbudgetreservepercententry.h \
   src/ledgerbudgetroutineentry.h \
   src/ledgerbudgetwithholdingentry.h \
   src/ledgercomment.h \
   src/ledgeritem.h \
   src/ledgertransaction.h \
   src/ledgertransactionentry.h \
   src/texttable.h \
   src/ynabregisterreader.h
SOURCES += \
   src/cashpiles.cpp \
   src/csvreader.cpp \
   src/currency.cpp \
   src/date.cpp \
   src/daterange.cpp \
   src/filereader.cpp \
   src/filewriter.cpp \
   src/identifier.cpp \
   src/interval.cpp \
   src/ipaccountbalancer.cpp \
   src/ipbudgetallocator.cpp \
   src/ipdatevalidator.cpp \
   src/iptransactioncategorizer.cpp \
   src/iptransferbalancer.cpp \
   src/itemprocessor.cpp \
   src/ledger.cpp \
   src/ledgeraccount.cpp \
   src/ledgerblank.cpp \
   src/ledgerbudget.cpp \
   src/ledgerbudgetcancelentry.cpp \
   src/ledgerbudgetcloseentry.cpp \
   src/ledgerbudgetentry.cpp \
   src/ledgerbudgetgoalentry.cpp \
   src/ledgerbudgetgoalsentry.cpp \
   src/ledgerbudgetincomeentry.cpp \
   src/ledgerbudgetreserveamountentry.cpp \
   src/ledgerbudgetreservepercententry.cpp \
   src/ledgerbudgetroutineentry.cpp \
   src/ledgerbudgetwithholdingentry.cpp \
   src/ledgercomment.cpp \
   src/ledgeritem.cpp \
   src/ledgertransaction.cpp \
   src/ledgertransactionentry.cpp \
   src/texttable.cpp \
   src/ynabregisterreader.cpp
TARGET = cashpiles
