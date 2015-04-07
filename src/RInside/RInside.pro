!include( ../!include_common.pri ) {
    error("Couldn't find the !include_common.pri file!")
}

###########################
###    PROJECT SETUP    ###
###########################

TEMPLATE = lib

TARGET = RInside

DEFINES += RINSIDE_LIBRARY
win32:DEFINES += Win32

###########################
###      LINK LIBS      ###
###########################
# Qt
QT += core

# R
R_HOME = $$system(R RHOME)

INCLUDEPATH += $$R_HOME/include
LIBS += -L$$DEP_LIB/R -lR

###########################
###    SETUP OUTPUT     ###
###########################

DESTDIR = $$LIB/RInside

###########################
###        DEPLOY       ###
###########################

DLLDESTDIR = $$BIN/REditor

###########################
###       SOURCE        ###
###########################

HEADERS += *.h
SOURCES += *.cpp
