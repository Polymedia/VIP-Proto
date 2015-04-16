!include( ../!include_common.pri ) {
    error("Couldn't find the !include_common.pri file!")
}

###########################
###    PROJECT SETUP    ###
###########################

TEMPLATE = app

DEBUG_TARGET = REditord
RELEASE_TARGET = REditor

###########################
###      LINK LIBS      ###
###########################
# Qt
QT += core gui widgets

CONFIG(debug, debug|release){
    LIBS += -lRInsided
}

CONFIG(release, debug|release){
    LIBS += -lRInside
}

LIBS += -L"$$LIB/RInside"
INCLUDEPATH += $$SRC

###########################
###    SETUP OUTPUT     ###
###########################

DESTDIR = $$BIN/$$RELEASE_TARGET

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
FORMS   += *.ui
