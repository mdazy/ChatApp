#include "ChatWidget.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QProcessEnvironment>
#include <QtCore/QSettings>
#include <QtCore/QTime>

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QTcpSocket>

#include <iostream>

using namespace std;


static const QString version = "1.1";


/*!
 * Sets up the UI and a non connected socket.
 */
ChatWidget::ChatWidget( QWidget* parent ) :
    QWidget( parent ),
    connected_( false )
{
    // retrieve settings from previous session
    settings_ = new QSettings( this );
    QString serverName = settings_->value( "serverName", QHostInfo::localHostName() ).toString();
    settings_->setValue( "serverName", serverName );
    QString nick = settings_->value( "nickName", QProcessEnvironment::systemEnvironment().value( "USERNAME" ) ).toString();
    settings_->setValue( "nickName", nick );

    auto l = new QVBoxLayout( this );

    auto hl = new QHBoxLayout();
    l->addLayout( hl );

    // server name input
    hl->addWidget( new QLabel( "Server" ) );
    serverName_ = new QLineEdit();
    serverName_->setObjectName( "server name" );
    serverName_->setText( serverName );
    hl->addWidget( serverName_ );
    connectButton_ = new QPushButton( "Connect" );
    connectButton_->setObjectName( "connect button" );
    hl->addWidget( connectButton_ );
    connect( connectButton_, SIGNAL( clicked() ), this, SLOT( tryConnectToServer() ) );

    hl = new QHBoxLayout();
    l->addLayout( hl );
    hl->addWidget( new QLabel( "Nickname" ) );
    nickName_ = new QLineEdit();
    nickName_->setText( nick );
    hl->addWidget( nickName_ );

    // read-only text view
    textView_ = new QTextEdit();
    textView_->setObjectName( "text view" );
    textView_->setReadOnly( true );
    textView_->setFocusPolicy( Qt::NoFocus );
    textView_->setAcceptRichText( false );
    l->addWidget( textView_ );

    // input field
    inputField_ = new QLineEdit();
    inputField_->setObjectName( "input field" );
    inputField_->setDisabled( true );
    l->addWidget( inputField_ );
    connect( inputField_, SIGNAL( returnPressed() ), this, SLOT( sendText() ) );

    // network connection
    socket_ = new QTcpSocket( this );
    connect( socket_, SIGNAL( connected() ), this, SLOT( connectToServer() ) );
    connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( disconnectFromServer() ) );
    connect( socket_, SIGNAL( disconnected() ), this, SLOT( disconnectFromServer() ) );
    connect( socket_, SIGNAL( readyRead() ), this, SLOT( receiveText() ) );
}


/*!
 */
ChatWidget::~ChatWidget() {
    settings_->sync();
}


/*!
 * Sends input text.
 * 
 * Sends the input text if it's not empty, prepending the nick name, then clears the
 * input field.
 */
void ChatWidget::sendText() {
    auto text = inputField_->text();
    inputField_->clear();
    if( text.isEmpty() ) {
        return;
    }
    if( text.startsWith( "/" ) ) {
        log( "/<b>Unknown command: " + text + ".</b>" );
        return;
    }

    log( text );

    if( nick() != prevNick_ ) {
        prevNick_ = nick();
        settings_->setValue( "nickName", prevNick_ );
        socket_->write( prevNick_.toLocal8Bit() );
        socket_->flush();
    }

    text = prevNick_ + ": " + text;
    if( connected_ ) {
        socket_->write( text.toLocal8Bit() );
    }
}


/*!
 * Displays text received from the server.
 */
void ChatWidget::receiveText() {
    auto text = QString::fromLocal8Bit( socket_->readAll() );
    if( text.isEmpty() ) {
        return;
    }
    log( text );
}


/*!
 * \return the nickname if it is defined, otherwise returns the OS username.
 */
QString ChatWidget::nick() const {
    QString name = nickName_->text();
    if( name.isEmpty() ) {
        name = QProcessEnvironment::systemEnvironment().value( "USERNAME" );
    }
    return name;
}


/*!
 * Logs the given text in the text view, prepending the current time.
 */
void ChatWidget::log( const QString& text ) const {
    QString time = QTime::currentTime().toString( Qt::DefaultLocaleShortDate ) + " - ";
    if( text.startsWith( "/" ) ) {
        textView_->append( time + text.right( text.length() - 1 ) );
    } else {
        textView_->append( "" );
        textView_->insertPlainText( time + text );
    }
}


/*!
 * Attempts to connect to the requested server.
 */
void ChatWidget::tryConnectToServer() {
    // disable connect button while trying
    serverName_->setDisabled( true );
    connectButton_->setDisabled( true );
    connectButton_->setText( "Connecting..." );

    // connect to port 12345
    socket_->connectToHost( serverName_->text(), 12345 );
    settings_->setValue( "serverName", serverName_->text() );
}


/*!
 * Update the UI once connected and sends the nickname as first message.
 */
void ChatWidget::connectToServer() {
    connected_ = true;
    log( "/<b><font color=green>*** Connected to " + socket_->peerAddress().toString() + ":" + QString::number( socket_->peerPort() ) + " ***</b></font>" );
    connectButton_->setText( "Connected" );
    inputField_->setEnabled( true );
    inputField_->setFocus();

    // send version to server
    socket_->write( version.toLocal8Bit() );
    socket_->flush();

    prevNick_ = nick();
    socket_->write( nick().toLocal8Bit() );
    socket_->flush();
}


/*!
 * Disconnects and resets the UI.
 */
void ChatWidget::disconnectFromServer() {
    connected_ = false;
    serverName_->setEnabled( true );
    connectButton_->setText( "Connect" );
    connectButton_->setEnabled( true );
    log( "/<b><font color=red>*** Disconnected from " + socket_->peerAddress().toString() + ":" + QString::number( socket_->peerPort() ) + " ***</font></b>" );
    inputField_->setDisabled( true );
}
