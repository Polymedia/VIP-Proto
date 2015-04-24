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

CONFIG(debug, debug|release){
    TARGET = $$DEBUG_TARGET
}

CONFIG(release, debug|release){
    TARGET = $$RELEASE_TARGET
}

###########################
###       DEPLOY        ###
###########################

CONFIG(release, debug|release){
    # REditor
    DLLDESTDIR += $$BIN/REditor
    CONFIG(DEPLOY_APPS){
        !isEmpty(QMAKE_POST_LINK) {
            QMAKE_POST_LINK += &&
        }
        QMAKE_POST_LINK += "$$DEPLOYQT \"$$DESTDIR/$${RELEASE_TARGET}.dll\" \"$$BIN/REditor\" && " \
                           "$$VCOPY \"$$VS_LIB\" \"$$BIN\\REditor\" *.dll"
    }

    # REditor
    DLLDESTDIR += $$BIN/RServer
    CONFIG(DEPLOY_APPS){
        !isEmpty(QMAKE_POST_LINK) {
            QMAKE_POST_LINK += &&
        }
        QMAKE_POST_LINK += "$$DEPLOYQT \"$$DESTDIR/$${RELEASE_TARGET}.dll\" \"$$BIN/RServer\" && " \
                           "$$VCOPY \"$$VS_LIB\" \"$$BIN\\RServer\" *.dll"
    }

    # Tests
    CONFIG(INCLUDE_TESTS) {
        DLLDESTDIR += $$TESTS_BIN
        CONFIG(DEPLOY_TESTS) {
            !isEmpty(QMAKE_POST_LINK) {
                QMAKE_POST_LINK += &&
            }
            QMAKE_POST_LINK += "$$DEPLOYQT \"$$DESTDIR/$${RELEASE_TARGET}.dll\" \"$$TESTS_BIN\" && " \
                               "$$VCOPY \"$$VS_LIB\" \"$$TESTS_BIN\" *.dll"
        }
    }
}


###########################
###       SOURCE        ###
###########################

HEADERS += *.h
SOURCES += *.cpp
