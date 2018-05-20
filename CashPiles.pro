TEMPLATE = app
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
HEADERS += \
   datatable.h \
   tabtable.h \
   tabtablerow.h \
   datatablefactory.h \
   datatablerow.h \
    txntable.h \
    txn.h \
    account.h \
    averager.h
SOURCES += \
   datatable.cpp \
   tabtable.cpp \
   tabtablerow.cpp \
   datatablefactory.cpp \
   datatablerow.cpp \
    txntable.cpp \
    txn.cpp \
    account.cpp \
    main.cpp \
    averager.cpp
OTHER_FILES += \
    README.md \
    data.txt
TARGET = CashPiles
