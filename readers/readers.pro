TEMPLATE = lib
CONFIG += dll
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..
HEADERS += \
   nativereader.h
SOURCES += \
   nativereader.cpp
win32:CONFIG(release, debug|release) {
   LIBS += -L$$OUT_PWD/../kernel/release -lcp-kernel
}
else:win32:CONFIG(debug, debug|release) {
   LIBS += -L$$OUT_PWD/../kernel/debug -lcp-kernel
}
else:unix {
   LIBS += -L$$OUT_PWD/../kernel -lcp-kernel
}
TARGET = cp-readers
