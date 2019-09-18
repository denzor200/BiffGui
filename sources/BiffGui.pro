#-------------------------------------------------
#
# Project created by QtCreator 2019-08-12T21:04:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BiffGui
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        ActorName.cpp \
        convertersyncapi.cpp \
        converterwaiting.cpp \
        generating.cpp \
        libqxt/core/qxtglobal.cpp \
        libqxt/gui/qxtcheckcombobox.cpp \
        main.cpp \
        maintabledelegates.cpp \
        maintablemodel.cpp \
        mainwindow.cpp \
        settings.cpp \
        utils.cpp

HEADERS += \
        ActorName.h \
        convertersyncapi.h \
        converterwaiting.h \
        directoriesregistry.h \
        generating.h \
        libqxt/core/qxtglobal.h \
        libqxt/core/qxtnamespace.h \
        libqxt/gui/qxtcheckcombobox.h \
        libqxt/gui/qxtcheckcombobox_p.h \
        maintabledelegates.h \
        maintablemodel.h \
        mainwindow.h \
        settings.h \
        utils.h

FORMS += \
        converterwaiting.ui \
        generating.ui \
        mainwindow.ui \
        settings.ui

DEFINES += BUILD_QXT QXT_STATIC
INCLUDEPATH += libqxt\core

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
