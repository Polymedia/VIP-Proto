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
        QMAKE_POST_LINK += "\"$$[QT_INSTALL_BINS]/windeployqt.exe\" \"$$DESTDIR/$${RELEASE_TARGET}.dll\" --libdir \"$$BIN/REditor\" -no-translations --no-system-d3d-compiler --no-compiler-runtime && " \
                             "$$VCOPY \"$$VS_LIB\" \"$$BIN\\REditor\" *.dll"
    }

    # Tests
    CONFIG(INCLUDE_TESTS) {
        DLLDESTDIR += $$TESTS_BIN
        CONFIG(DEPLOY_TESTS) {
            !isEmpty(QMAKE_POST_LINK) {
                QMAKE_POST_LINK += &&
            }
            QMAKE_POST_LINK += "\"$$[QT_INSTALL_BINS]/windeployqt.exe\" \"$$DESTDIR/$${RELEASE_TARGET}.dll\" --libdir \"$$TESTS_BIN\" -no-translations --no-system-d3d-compiler --no-compiler-runtime && " \
                                 "$$VCOPY \"$$VS_LIB\" \"$$TESTS_BIN\" *.dll"
        }
    }
}


###########################
###       SOURCE        ###
###########################

HEADERS += *.h
SOURCES += *.cpp
