QT       += core gui widgets

CONFIG   += c++17
TARGET   = bytepress
TEMPLATE = app

# Output Directories
DESTDIR = ../bin  # Executable goes into bin, relative to build dir where qmake runs from
OBJECTS_DIR = ../build
MOC_DIR = ../build
# UI_DIR = ../build # If you add .ui files later
# RCC_DIR = ../build # If you add .qrc files later

# Define the source directory relative to the .pro file location
SRCDIR = src
STRATEGIESDIR = $${SRCDIR}/strategies

SOURCES += \
    $${SRCDIR}/main.cpp \
    $${SRCDIR}/mainwindow.cpp \
    $${SRCDIR}/compressionservice.cpp \
    $${STRATEGIESDIR}/pngstrategy.cpp \
    $${STRATEGIESDIR}/jpegstrategy.cpp \
    $${STRATEGIESDIR}/gifstrategy.cpp \
    $${STRATEGIESDIR}/avifstrategy.cpp \
    $${STRATEGIESDIR}/heifstrategy.cpp

HEADERS += \
    $${SRCDIR}/mainwindow.h \
    $${SRCDIR}/compressionservice.h \
    $${SRCDIR}/compressionstrategy.h \
    $${STRATEGIESDIR}/pngstrategy.h \
    $${STRATEGIESDIR}/jpegstrategy.h \
    $${STRATEGIESDIR}/gifstrategy.h \
    $${STRATEGIESDIR}/avifstrategy.h \
    $${STRATEGIESDIR}/heifstrategy.h

# For out-of-source builds, ensure headers are findable
INCLUDEPATH += $${PWD}/src $${PWD}/src/strategies

# Final simplified paths
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build