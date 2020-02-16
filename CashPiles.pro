TEMPLATE = app
CONFIG += console
#QT += widgets
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
HEADERS += \
   src/cashpiles.h \
   src/currency.h \
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
   src/ledgerbudgetcloseentry.h \
   src/ledgerbudgetentry.h \
   src/ledgerbudgetgoalentry.h \
   src/ledgerbudgetincomeentry.h \
   src/ledgerbudgetreserveamountentry.h \
   src/ledgerbudgetreservepercententry.h \
   src/ledgerbudgetroutineentry.h \
   src/ledgerbudgetwithholdingentry.h \
   src/ledgercomment.h \
   src/ledgeritem.h \
   src/ledgerreserve.h \
   src/ledgertransaction.h \
   src/ledgertransactionentry.h \
   src/texttable.h
SOURCES += \
   src/cashpiles.cpp \
   src/currency.cpp \
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
   src/ledgerbudgetcloseentry.cpp \
   src/ledgerbudgetentry.cpp \
   src/ledgerbudgetgoalentry.cpp \
   src/ledgerbudgetincomeentry.cpp \
   src/ledgerbudgetreserveamountentry.cpp \
   src/ledgerbudgetreservepercententry.cpp \
   src/ledgerbudgetroutineentry.cpp \
   src/ledgerbudgetwithholdingentry.cpp \
   src/ledgercomment.cpp \
   src/ledgeritem.cpp \
   src/ledgerreserve.cpp \
   src/ledgertransaction.cpp \
   src/ledgertransactionentry.cpp \
   src/texttable.cpp
TARGET = cashpiles
