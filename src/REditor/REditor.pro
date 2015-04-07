!include( ../!include_common.pri ) {
    error("Couldn't find the !include_common.pri file!")
}

###########################
###    PROJECT SETUP    ###
###########################

TEMPLATE = app

TARGET = REditor

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

DESTDIR = $$BIN

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
