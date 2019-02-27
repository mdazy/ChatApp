TEMPLATE = app
DESTDIR = ../bin
OBJECTS_DIR = ../obj/ChatClient
MOC_DIR = ../moc

CONFIG += debug

win32:CONFIG += x86_64
win32:CONFIG -= x86 release

QT += widgets network

HEADERS += ChatWidget.h

SOURCES += ChatWidget.cpp
SOURCES += main.cpp
