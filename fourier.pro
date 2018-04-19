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
    src/common.h \
    src/decompose.h \
    src/generate.h \
    src/logger.h \
    src/wave.h \

SOURCES = \
    src/decompose.cpp \
    src/generate.cpp \
    src/logger.cpp \
    src/wave.cpp \
    src/main.cpp
