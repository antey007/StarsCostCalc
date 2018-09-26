#-------------------------------------------------
#
# Project created by QtCreator 2018-07-26T14:50:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CostCalculator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    decryptor.cpp \
    importdlg.cpp \
    designdlg.cpp \
    costdynamicsdlg.cpp \
    filterdlg.cpp \
    salvageanalyzerdlg.cpp \
    eventsdlg.cpp

HEADERS  += mainwindow.h \
    defs.h \
    decryptor.h \
    FileHeaderBlock.h \
    Block.h \
    PlayerBlock.h \
    DesignBlock.h \
    importdlg.h \
    designdlg.h \
    costdynamicsdlg.h \
    filterdlg.h \
    salvageanalyzerdlg.h \
    ObjectBlock.h \
    eventsdlg.h

OTHER_FILES +=
