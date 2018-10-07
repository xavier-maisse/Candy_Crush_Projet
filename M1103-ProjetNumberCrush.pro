TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    Correc_prof/gridmanagement.cpp \
    Correc_prof/nsutil.cpp \
    Correc_prof/game.cpp \
    Correc_prof/params.cpp
LIBS += -static
HEADERS += \
    Correc_prof/nsutil.h \
    Correc_prof/type.h \
    Correc_prof/params.h \
    Correc_prof/gridmanagement.h \
    Correc_prof/game.h
