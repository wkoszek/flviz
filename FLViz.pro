# -------------------------------------------------
# Project created by QtCreator 2009-06-20T15:08:41
# -------------------------------------------------
QT += svg \
    xml
TARGET = FLViz
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    FLViz.cpp \
    subr/strsep.cc \
    subr/strlcat.cc \
    flvizgv.cpp
HEADERS += mainwindow.h \
    subr/subr.h \
    queue.h \
    FLViz.h \
    flvizgv.h
FORMS += mainwindow.ui

# DEFINES += D -Iwiz\include\graphviz\
LIBS += -lgraph \
    -lgvc
LIBS += -L"." -L"release\lib"
INCPATH += wiz\include\graphviz
QMAKE_CXXFLAGS += -Wno-write-strings
