!include( ../!include_common.pri ) {
    error("Couldn't find the !include_common.pri file!")
}

###########################
###    PROJECT SETUP    ###
###########################

TEMPLATE = app

TARGET = REditor

DEFINES += RINSIDE_LIBRARY
win32:DEFINES += Win32

###########################
###      LINK LIBS      ###
###########################
# Qt
QT += core gui widgets

# R Inside
INCLUDEPATH += $$SRC/RInside
LIBS += -L"$$LIB/RInside"
LIBS += -lRInside

###########################
###    SETUP OUTPUT     ###
###########################


###########################
###        DEPLOY       ###
###########################

DLLDESTDIR = $$BIN

###########################
###       SOURCE        ###
###########################

HEADERS += *.h
SOURCES += *.cpp
FORMS   += *.ui
