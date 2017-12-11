#include <QtCore/QSettings>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include "ChatWidget.h"

int main( int argc, char** argv ) {
    QApplication q( argc, argv );

    QCoreApplication::setApplicationName( "ChatClient" );
    QCoreApplication::setOrganizationName( "Bob" );
    QSettings::setDefaultFormat( QSettings::IniFormat );

    auto mainW = new QMainWindow();
    mainW->setObjectName( "main window" );
    mainW->setCentralWidget( new ChatWidget( mainW ) );
    mainW->centralWidget()->setObjectName( "chat widget" );
    mainW->setAttribute( Qt::WA_DeleteOnClose );
    mainW->show();

    return qApp->exec();
}
