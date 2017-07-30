TEMPLATE = app

QMAKE_MAC_SDK = macosx10.11
QT += qml quick
CONFIG += c++11
DEFINES += QT_MESSAGELOGCONTEXT
DEFINES -= OWN_LOGGING

LIBS += -L"$$PWD/../libs/"
LIBS += -lz
LIBS += -lquazip

macx {
    INCLUDEPATH += "../3rdparties/quazip"
}

win32 {
    INCLUDEPATH += "../3rdparties/quazip"
    INCLUDEPATH += "../3rdparties/zlib-1.2.8/"
}

SOURCES += main.cpp \
    packageinstaller.cpp \
    packageparser.cpp \
    fshelpers.cpp \
    ../common/diffgeneratorbase.cpp \
    livelog.cpp \
    installationorchestrator.cpp \
    uihelper.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    packageinstaller.h \
    platform.h \
    packageparser.h \
    ../common/fileentry.h \
    ../common/defines.h \
    ../common/diffgeneratorbase.h \
    options.h \
    fshelpers.h \
    ../common/ifilesprovider.h \
    livelog.h \
    installationorchestrator.h \
    uihelper.h

DISTFILES += \
    StyledBusyIndicator.qml \
    DetailsWindow.qml \
    StyledButton.qml \
    Colors.js \
    StyledText.qml
