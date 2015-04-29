!include( ../!include_common.pri ) {
    error("Couldn't find the !include_common.pri file!")
}

###########################
###    PROJECT SETUP    ###
###########################

TEMPLATE = app

DEBUG_TARGET = RServerd
RELEASE_TARGET = RServer

###########################
###      LINK LIBS      ###
###########################
# Qt
QT += core network

CONFIG += console

# R Inside
INCLUDEPATH += $$SRC/RInside
LIBS += -L"$$LIB/RInside"

CONFIG(debug, debug|release){
    LIBS += -lRInsided
}

CONFIG(release, debug|release){
    LIBS += -lRInside
}

#QtWebApp
INCLUDEPATH += $$DEP_INCLUDE/QtWebApp
LIBS += -L"$$DEP_LIB/QtWebApp"

DEFINES += QTWEBAPPLIB_IMPORT
CONFIG(release, debug|release) {
    LIBS += -lQtWebApp1
}

CONFIG(debug, debug|release) {
    LIBS += -lQtWebAppd1
}

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
###        DEPLOY       ###
###########################

CONFIG(release, debug|release){
    CONFIG(DEPLOY_APPS){
        !isEmpty(QMAKE_POST_LINK) {
            QMAKE_POST_LINK += &&
        }
        QMAKE_POST_LINK += "$$DEPLOYQT \"$$DESTDIR/$${RELEASE_TARGET}.exe\" \"$$DESTDIR\" && " \
                           "$$VCOPY \"$$VS_LIB\" \"$$DESTDIR\" *.dll && " \
                           "$$VCOPY \"$$DEP_LIB\\QtWebApp\" \"$$DESTDIR\" QtWebApp1.dll"
    }
}


###########################
###       SOURCE        ###
###########################

HEADERS += *.h \
    controllers/JsonTableController.h \
    controllers/JsonDataLoader.h
SOURCES += *.cpp \
    controllers/JsonTableController.cpp \
    controllers/JsonDataLoader.cpp
