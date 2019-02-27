TEMPLATE = app
DESTDIR = ../bin
OBJECTS_DIR = ../obj/ChatServer
MOC_DIR = ../moc

CONFIG += debug

win32:CONFIG += x86_64
win32:CONFIG -= x86 release

QT += widgets network

HEADERS += ChatServer.h

SOURCES += ChatServer.cpp
SOURCES += main.cpp
