TEMPLATE = app
CONFIG += console
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..
HEADERS += \
   csvreader.h \
   ynabregisterreader.h
SOURCES += \
   csvreader.cpp \
   main.cpp \
   ynabregisterreader.cpp
win32:CONFIG(release, debug|release) {
   LIBS += -L$$OUT_PWD/../kernel/release -lcp-kernel
   LIBS += -L$$OUT_PWD/../processors/release -lcp-processors
   LIBS += -L$$OUT_PWD/../readers/release -lcp-readers
}
else:win32:CONFIG(debug, debug|release) {
   LIBS += -L$$OUT_PWD/../kernel/debug -lcp-kernel
   LIBS += -L$$OUT_PWD/../processors/debug -lcp-processors
   LIBS += -L$$OUT_PWD/../readers/debug -lcp-readers
}
else:unix {
   LIBS += -L$$OUT_PWD/../kernel -lcp-kernel -lcp-processors -lcp-readers
}
TARGET = ynabconverter
