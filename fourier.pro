TEMPLATE = app
PROJECT = fourier
TARGET = $$PROJECT

CONFIG -= qt
CONFIG += warn_on

QMAKE_CXXFLAGS *= -Wall -Wextra -Werror -pedantic-errors
QMAKE_CXXFLAGS += -std=c++14

OBJECTS_DIR = $$PWD/build/obj
DESTDIR = $$PWD/build

INCLUDEPATH += src

HEADERS = \
    src/commons.h \
    src/decompose.h \
    src/dft.h \
    src/filter.h \
    src/generate.h \
    src/logger.h \
    src/wave.h \

SOURCES = \
    src/commons.cpp \
    src/decompose.cpp \
    src/dft.cpp \
    src/filter.cpp \
    src/generate.cpp \
    src/logger.cpp \
    src/wave.cpp \
    src/main.cpp
