QT += core
QT -= gui

CONFIG += c++11

DEFINES += OWN_LOGGING

TARGET = minipkgen
CONFIG += console
CONFIG -= app_bundle
DEFINES += QT_NO_CAST_TO_ASCII \
           QT_RESTRICTED_CAST_FROM_ASCII \
           QT_NO_CAST_FROM_BYTEARRAY \
           QT_MESSAGELOGCONTEXT

TEMPLATE = app

SOURCES += main.cpp \
    ../common/logger.cpp \
    ../common/diffgeneratorbase.cpp \
    jsondiffgenerator.cpp

HEADERS += \
    ../common/logger.h \
    options.h \
    ../common/fileentry.h \
    ../common/defines.h \
    ../common/diffgeneratorbase.h \
    ../common/ifilesprovider.h \
    jsondiffgenerator.h
