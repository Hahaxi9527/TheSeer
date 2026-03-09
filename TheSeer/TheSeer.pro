QT       += core gui
QT +=  network
QT+=  sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += qmake_qt

QMAKE_CXXFLAGS += -finput-charset=UTF-8
QMAKE_CXXFLAGS += -fexec-charset=UTF-8

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    lobby.cpp \
    login.cpp \
    main.cpp \
    manager.cpp \
    my_push_button.cpp \
    pets.cpp \
    start_game.cpp \
    theseer.cpp

HEADERS += \
    config.h \
    lobby.h \
    login.h \
    manager.h \
    my_push_button.h \
    pets.h \
    start_game.h \
    theseer.h

FORMS += \
    lobby.ui \
    login.ui \
    start_game.ui \
    theseer.ui

TRANSLATIONS += \
    TheSeer_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
