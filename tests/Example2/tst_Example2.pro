!include( $$PWD/../!include_UnitTest.pri ) {
    error("Couldn't find !include_UnitTest.pri file!")
}

###########################
###    PROJECT SETUP    ###
###########################

TEMPLATE = app

TARGET = tst_Example2

###########################
###     DEPENDENCIES    ###
###########################

###########################
###     SETUP OUTPUT    ###
###########################

###########################
###        SOURCE       ###
###########################

HEADERS += *.h
SOURCES += *.cpp

# ###############################
