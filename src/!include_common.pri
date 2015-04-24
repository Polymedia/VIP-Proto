##
## 1) Included in every application/project inside VIP.pro
## 2) Included in every module of application-project inside VIP.pro
##
##################################################################################

# Include checking
!contains(INCLUDED_PRI, $$PWD/!include_common.pri) {
    INCLUDED_PRI += $$PWD/!include_common.pri

    !include(!include_custom.pri) {
        message("Custom .pri file not found.")
    }

    # Paths
    ROOT = "$$PWD/.."

    BIN = $$ROOT/bin

    DEP = $$ROOT/dep
    DEP_INCLUDE = $$DEP/include
    DEP_LIB = $$DEP/lib
    VS_LIB = $$DEP_LIB/vsredist

    LIB = $$ROOT/lib

    SRC = $$ROOT/src

    # Tools
    VCOPY = "$$DEP/vcopy.cmd"    
    DEPLOYQT="\"$$DEP/vwindeployqt.cmd\" \"$$[QT_INSTALL_BINS]\""

    !win32-msvc2003:!win32-msvc2005:!win32-msvc2008:!win32-msvc2010:!win32-msvc2013 {
        error("Only win32 MSVC compiler supported!")
    }

    # Tests configuration
    CONFIG(INCLUDE_TESTS) {
        # We need to setup bin paths here because
        # we have to deploy project *.dll from project source *.pro
        # Tests directories
        TESTS_BIN = $$BIN/Tests
        TESTS_SRC = $$ROOT/tests
    }
}
# End Include checking
