TEMPLATE = app
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
HEADERS += \
   src/datatable.h \
   src/tabtable.h \
   src/tabtablerow.h \
   src/datatablefactory.h \
   src/datatablerow.h \
    src/txntable.h \
    src/txn.h \
    src/account.h \
    src/averager.h
SOURCES += \
   src/datatable.cpp \
   src/tabtable.cpp \
   src/tabtablerow.cpp \
   src/datatablefactory.cpp \
   src/datatablerow.cpp \
    src/txntable.cpp \
    src/txn.cpp \
    src/account.cpp \
    src/main.cpp \
    src/averager.cpp
OTHER_FILES += \
    README.md \
    data.txt
TARGET = CashPiles
