TEMPLATE = app
QT += widgets
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
HEADERS += \
   model/currency.h \
   model/daterange.h \
   model/interval.h \
   model/itemprocessor.h \
   model/ledger.h \
   model/ledgeraccountcommand.h \
   model/ledgerbudget.h \
   model/ledgercomment.h \
   model/ledgeritem.h \
   model/ledgertransaction.h \
   model/ledgertransactionentry.h \
   processors/accountbalancer.h \
   processors/budgetbalancer.h \
   processors/datevalidator.h \
   processors/nativewriter.h \
   processors/reserveamountallocator.h \
   processors/reservepercentallocator.h \
   processors/routineallocator.h \
   readers/csvreader.h \
   readers/nativereader.h \
   readers/ynabregisterreader.h \
   ui/accountbalancerui.h \
   ui/mainwindow.h \
   ui/messageui.h
SOURCES += \
   model/currency.cpp \
   model/daterange.cpp \
   model/itemprocessor.cpp \
   model/interval.cpp \
   model/ledger.cpp \
   model/ledgeraccountcommand.cpp \
   model/ledgerbudget.cpp \
   model/ledgercomment.cpp \
   model/ledgeritem.cpp \
   model/ledgertransaction.cpp \
   model/ledgertransactionentry.cpp \
   processors/accountbalancer.cpp \
   processors/budgetbalancer.cpp \
   processors/datevalidator.cpp \
   processors/nativewriter.cpp \
   processors/reserveamountallocator.cpp \
   processors/reservepercentallocator.cpp \
   processors/routineallocator.cpp \
   readers/csvreader.cpp \
   readers/nativereader.cpp \
   readers/ynabregisterreader.cpp \
   ui/accountbalancerui.cpp \
   ui/mainwindow.cpp \
   ui/messageui.cpp \
   main.cpp
FORMS += \
   ui/accountbalancerui.ui \
   ui/mainwindow.ui \
   ui/messageui.ui
OTHER_FILES += \
   README.md
TARGET = CashPiles
