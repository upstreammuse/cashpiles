TEMPLATE = app
QT += widgets
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..
HEADERS += \
   accountbalancerui.h \
   mainwindow.h \
   messageui.h
SOURCES += \
   accountbalancerui.cpp \
   mainwindow.cpp \
   messageui.cpp \
   main.cpp
FORMS += \
   accountbalancerui.ui \
   mainwindow.ui \
   messageui.ui
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
