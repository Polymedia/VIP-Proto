# Include checking
!contains(INCLUDED_PRI, $$PWD/!include_UnitTest.pri) {
	INCLUDED_PRI += $$PWD/!include_UnitTest.pri
	
	###########################
	###    PROJECT SETUP    ###
	###########################
        !include( $$PWD/../src/!include_common.pri ) {
                error("Couldn't find !include_common.pri file!")
	}

	###########################
	###     DEPENDENCIES    ###
	###########################
	# Qt
	QT += core

	CONFIG   += console
	CONFIG   -= app_bundle

	# Qt TestLib
	QT += testlib

	# GTest & GMock
	INCLUDEPATH += $$DEP_INCLUDE

	LIBS += -L"$$DEP_LIB/gtest" \
                -L"$$DEP_LIB/gmock"


        CONFIG(debug, debug|release) {
            LIBS += -lgtestd
            LIBS += -lgmockd
	}

        CONFIG(release, debug|release) {
            LIBS += -lgtest
            LIBS += -lgmock
	}

        # Rude Mock
        INCLUDEPATH += "$$DEP_INCLUDE/RudeMocks"

        LIBS += -L"$$DEP_LIB/RudeMocks"

        CONFIG(debug, debug|release) {
            LIBS += -lRudeMocks-d
        }

        CONFIG(release, debug|release){
            LIBS += -lRudeMocks
        }        

	# TestUtilites
        INCLUDEPATH += $$TESTS_SRC

	###########################
	###     SETUP OUTPUT    ###
	###########################
	DESTDIR += $$TESTS_BIN

        ###########################
        ###       DEPLOY        ###
        ###########################

        CONFIG(release, debug|release){
            CONFIG(DEPLOY_TESTS) {
                !isEmpty(QMAKE_POST_LINK) {
                    QMAKE_POST_LINK += &&
                }
                QMAKE_POST_LINK += "\"$$[QT_INSTALL_BINS]/windeployqt.exe\" \"$$DESTDIR/$${TARGET}.exe\" --libdir \"$$TESTS_BIN\" -no-translations --no-system-d3d-compiler --no-compiler-runtime && " \
                                   "$$VCOPY \"$$VS_LIB\" \"$$TESTS_BIN\" *.dll"
            }
        }
}
# End Include checking
