!include( !include_common.pri ) {
    error("Couldn't find the !include_common.pri file!")
}

TEMPLATE = subdirs

SUBDIRS = \
    RInside \
    REditor \
    RServer

REditor.depends += \
    RInside

REditor.depends += \
    RServer
    
CONFIG(INCLUDE_TESTS) {
    SUBDIRS += Tests

    Tests.file = ../tests/Tests.pro

    Tests.depends += \
        RInside \
        REditor \
        RServer
} 
