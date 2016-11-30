#-------------------------------------------------
#
# Project created by QtCreator 2016-11-15T01:57:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Cigma16
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    highlighter.cpp \
    assembler017.cpp \
    emulator.cpp \
    assembler144.cpp \
    assembler.cpp

HEADERS  += mainwindow.h \
    highlighter.h \
    assembler017.h \
    emulator.h \
    assembler144.h \
    assembler.h

FORMS    += mainwindow.ui

RESOURCES +=

RC_FILE += \
    resource.rc

DISTFILES += \
    resource.rc

QMAKE_LFLAGS += -static-libgcc -static-libstdc++
