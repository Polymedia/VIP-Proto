!include( $$PWD/../!include_UnitTest.pri ) {
    error("Couldn't find !include_UnitTest.pri file!")
}

###########################
###    PROJECT SETUP    ###
###########################

TEMPLATE = app

TARGET = tst_RInside

###########################
###     DEPENDENCIES    ###
###########################

INCLUDEPATH += $$SRC/RInside
LIBS += -L$$LIB/RInside -lRInside

###########################
###     SETUP OUTPUT    ###
###########################

###########################
###        SOURCE       ###
###########################

HEADERS += *.h
SOURCES += *.cpp

# ###############################
