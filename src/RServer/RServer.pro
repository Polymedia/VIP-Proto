!include( ../!include_common.pri ) {
    error("Couldn't find the !include_common.pri file!")
}

###########################
###    PROJECT SETUP    ###
###########################

TEMPLATE = app

TARGET = RServer

###########################
###      LINK LIBS      ###
###########################
# Qt
QT += core network

# R Inside
INCLUDEPATH += $$SRC/RInside
LIBS += -L"$$LIB/RInside"
LIBS += -lRInside

#QtWebApp
LIBS += -L"$$LIB/QtWebApp"
INCLUDEPATH += $$DEP_INCLUDE/QtWebApp
LIBS += -L"$$DEP_LIB/QtWebApp"

CONFIG(release, debug|release) {
    LIBS += -lQtWebApp1
}

CONFIG(debug, debug|release) {
    LIBS += -lQtWebAppd1
}

DEFINES += QTWEBAPPLIB_IMPORT

###########################
###    SETUP OUTPUT     ###
###########################

DESTDIR = $$BIN/RServer
DLLDESTDIR = $$BIN/RServer

###########################
###        DEPLOY       ###
###########################

###########################
###       SOURCE        ###
###########################

HEADERS += *.h \
    controllers/JsonTableController.h \
    controllers/JsonDataLoader.h
SOURCES += *.cpp \
    controllers/JsonTableController.cpp \
    controllers/JsonDataLoader.cpp
