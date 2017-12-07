#include <QtWidgets/QApplication>

#include "ChatServer.h"

int main( int argc, char** argv ) {
    QApplication q( argc, argv );
    new ChatServer( &q );
    return q.exec();
}