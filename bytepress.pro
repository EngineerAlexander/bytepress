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

# If qmake is run from within the project root where the .pro file is:
# DESTDIR = bin
# OBJECTS_DIR = build
# MOC_DIR = build
# UI_DIR = build
# RCC_DIR = build

# However, it's more robust for qmake to handle paths relative to where the Makefile is generated.
# Typically, qmake is run in a separate build directory for out-of-source builds.
# If you run qmake from the project root, the above DESTDIR = bin, etc. is fine.
# If you `mkdir build_dir && cd build_dir && qmake ../bytepress.pro`, then: 
# DESTDIR needs to be relative like ../bin
# OBJECTS_DIR and MOC_DIR can be ., or specific subfolders within the build_dir.

# Let's assume qmake will be run from project root, and it will create Makefile in project root.
# Then a sub-run of make might happen in 'build' or objects go to 'build'
# The simplest for qmake run in project root:
# DESTDIR = bin
# OBJECTS_DIR = build
# MOC_DIR = build

# Let's re-evaluate. If Makefile is in project root, OBJECTS_DIR = build/ is correct.
# DESTDIR = bin/ is also correct.

# Final simplified paths assuming Makefile is in project root (where .pro is)
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build

# Ensure .qmake.stash and other qmake temp files also go to build if possible,
# but that's usually handled by running qmake from the build directory.
# For now, this setup should place .o and moc files in build/, and the app in bin/. 