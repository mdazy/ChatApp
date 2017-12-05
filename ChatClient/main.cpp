#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include "ChatWidget.h"

int main( int argc, char** argv ) {
    QApplication q( argc, argv );

    auto mainW = new QMainWindow();
    mainW->setObjectName( "main window" );
    mainW->setCentralWidget( new ChatWidget( mainW ) );
    mainW->centralWidget()->setObjectName( "chat widget" );
    mainW->show();

    return qApp->exec();
}
