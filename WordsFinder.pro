TEMPLATE = app
TARGET = WordsFinder

QT += quick quickcontrols2

CONFIG += c++17

SOURCES += \
    main.cpp \
    manager.cpp \
    readingthread.cpp

HEADERS += \
    manager.h \
    readingthread.h \
    infostruct.h

RESOURCES += qml.qrc

