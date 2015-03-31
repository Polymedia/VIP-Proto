QT += core

TARGET = RInside
TEMPLATE = lib
DESTDIR = $$PWD/../../lib/RInside
DLLDESTDIR = $$PWD/../../bin

DEFINES += RINSIDE_LIBRARY
win32:DEFINES += Win32

SOURCES += *.cpp
HEADERS += *.h

R_HOME = $$system(R RHOME)

INCLUDEPATH += $$R_HOME/include
LIBS += -L$$R_HOME/lib -lR
