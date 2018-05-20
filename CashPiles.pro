TEMPLATE = app
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
INCLUDEPATH += include
HEADERS += \
   include/csvreader.h \
   include/ledger.h \
   include/ledgeraccount.h \
   include/ledgeraccountcommand.h \
   include/ledgerbudgetallocation.h \
   include/ledgercommand.h \
   include/ledgercomment.h \
   include/ledgertransaction.h \
   include/ledgertransactionentry.h \
   include/nativeaccountcommand.h \
   include/nativebudgetcommand.h \
   include/nativecommand.h \
   include/nativecomment.h \
   include/nativemodel.h \
   include/nativereader.h \
   include/nativetransaction.h \
   include/nativetransactionentry.h \
   include/settingsdialog.h
SOURCES += \
   src/csvreader.cpp \
   src/ledger.cpp \
   src/ledgeraccount.cpp \
   src/ledgeraccountcommand.cpp \
   src/ledgerbudgetallocation.cpp \
   src/ledgercommand.cpp \
   src/ledgercomment.cpp \
   src/ledgertransaction.cpp \
   src/ledgertransactionentry.cpp \
   src/main.cpp \
   src/nativeaccountcommand.cpp \
   src/nativebudgetcommand.cpp \
   src/nativecommand.cpp \
   src/nativecomment.cpp \
   src/nativemodel.cpp \
   src/nativereader.cpp \
   src/nativetransaction.cpp \
   src/nativetransactionentry.cpp \
   src/settingsdialog.cpp
FORMS += \
   ui/settingsdialog.ui
OTHER_FILES += \
   README.md
TARGET = CashPiles
