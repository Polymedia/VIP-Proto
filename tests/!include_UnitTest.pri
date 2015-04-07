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
}
# End Include checking
