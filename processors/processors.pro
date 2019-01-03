TEMPLATE = lib
CONFIG += dll
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..
HEADERS += \
   accountbalancer.h \
   budgetbalancer.h \
   datevalidator.h \
   nativewriter.h \
   reserveamountallocator.h \
   reservepercentallocator.h \
   routineallocator.h
SOURCES += \
   accountbalancer.cpp \
   budgetbalancer.cpp \
   datevalidator.cpp \
   nativewriter.cpp \
   reserveamountallocator.cpp \
   reservepercentallocator.cpp \
   routineallocator.cpp
win32:CONFIG(release, debug|release) {
   LIBS += -L$$OUT_PWD/../kernel/release -lcp-kernel
}
else:win32:CONFIG(debug, debug|release) {
   LIBS += -L$$OUT_PWD/../kernel/debug -lcp-kernel
}
else:unix {
   LIBS += -L$$OUT_PWD/../kernel -lcp-kernel
}
TARGET = cp-processors
