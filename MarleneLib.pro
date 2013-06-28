#-------------------------------------------------
#
# Project created by QtCreator 2013-06-26T14:39:10
#
#-------------------------------------------------

QT       -= gui

TARGET = MarleneLib
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    Util.cpp \
    SmartCardMarlene.cpp \
    SmartCard.cpp \
    sha256.c \
    SocketClient.cpp

HEADERS += \
    Util.h \
    SmartCardMarlene.h \
    SmartCard.h \
    sha256.h \
    SocketClient.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32 {
INCLUDEPATH += C:\BasicCardPro\Api\H
}
