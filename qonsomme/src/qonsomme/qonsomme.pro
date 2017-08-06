QT += core network
QT -= gui

CONFIG += c++11

TARGET = qonsomme
CONFIG += console
CONFIG -= app_bundle

DEFINES += QT_MESSAGELOGCONTEXT

TEMPLATE = app

INCLUDEPATH += ../../vendors/exiv2/include/

CONFIG(debug, debug|release)  {
    LIBS += -L"$$PWD/../../libs/debug"
} else {
    LIBS += -L"$$PWD/../../libs/release"
    DEFINES += WITH_LOGS
}

macx {
    LIBS += -liconv
    LIBS += -lexpat
    LIBS += -lxmpsdk
    LIBS += -lexiv2
    LIBS += -lz
}

SOURCES += main.cpp \
    requestscoordinator.cpp \
    metadataconsolidatingworker.cpp \
    exiv2consolidator.cpp \
    helpers.cpp \
    resultssendingworker.cpp \
    exiv2inithelper.cpp \
    logger.cpp \
    loggingworker.cpp

HEADERS += \
    requestscoordinator.h \
    metadataconsolidatingworker.h \
    consolidationresult.h \
    consolidaterequest.h \
    readerwriterqueue.h \
    exiv2consolidator.h \
    exiv2tagnames.h \
    defines.h \
    helpers.h \
    resultssendingworker.h \
    exiv2inithelper.h \
    logger.h \
    loggingworker.h
