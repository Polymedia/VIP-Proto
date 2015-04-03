!include( !include_common.pri ) {
    error("Couldn't find the !include_common.pri file!")
}

TEMPLATE = subdirs

SUBDIRS = \
    RInside \
    REditor

REditor.depends += \
    RInside
