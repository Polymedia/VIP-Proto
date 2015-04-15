!include( ../!include_common.pri ) {
    error("Couldn't find the !include_common.pri file!")
}

###########################
###    PROJECT SETUP    ###
###########################

TEMPLATE = lib

DEBUG_TARGET = RInsided
RELEASE_TARGET = RInside

DEFINES += RINSIDE_LIBRARY
win32:DEFINES += Win32

###########################
###      LINK LIBS      ###
###########################
# Qt
QT += core

INCLUDEPATH += $$DEP_INCLUDE/R
LIBS += -L$$DEP_LIB/R -lR

###########################
###    SETUP OUTPUT     ###
###########################

DESTDIR = $$LIB/$$RELEASE_TARGET
DLLDESTDIR = $$BIN/REditor

CONFIG(debug, debug|release){
    TARGET = $$DEBUG_TARGET
}

CONFIG(release, debug|release){
    TARGET = $$RELEASE_TARGET
}

###########################
###       SOURCE        ###
###########################

HEADERS += *.h
SOURCES += *.cpp
