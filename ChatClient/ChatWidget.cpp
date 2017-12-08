#include "ChatWidget.h"

#include <QtCore/QProcessEnvironment>

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QTcpSocket>

ChatWidget::ChatWidget( QWidget* parent ) :
    QWidget( parent ),
    connected_( false )
{
    auto l = new QVBoxLayout( this );

    auto hl = new QHBoxLayout();
    l->addLayout( hl );

    // server name input
    hl->addWidget( new QLabel( "Server" ) );
    serverName_ = new QLineEdit();
    serverName_->setObjectName( "server name" );
    serverName_->setText( QHostInfo::localHostName() );
    hl->addWidget( serverName_ );
    connectButton_ = new QPushButton( "Connect" );
    connectButton_->setObjectName( "connect button" );
    hl->addWidget( connectButton_ );
    connect( connectButton_, SIGNAL( clicked() ), this, SLOT( tryConnectToServer() ) );

    hl = new QHBoxLayout();
    l->addLayout( hl );
    hl->addWidget( new QLabel( "Nickname" ) );
    nickName_ = new QLineEdit();
    nickName_->setText( QProcessEnvironment::systemEnvironment().value( "USERNAME" ) );
    hl->addWidget( nickName_ );

    // read-only text view
    textView_ = new QTextEdit();
    textView_->setObjectName( "text view" );
    textView_->setReadOnly( true );
    textView_->setFocusPolicy( Qt::NoFocus );
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


void ChatWidget::sendText() {
    auto text = inputField_->text();
    if( text.isEmpty() ) {
        return;
    }
    textView_->append( text );

    QString nick = nickName_->text();
    if( nick.isEmpty() ) {
        nick = QProcessEnvironment::systemEnvironment().value( "USERNAME" );
    }
    text = nick + ": " + text;
    if( connected_ ) {
        // send to server
        socket_->write( text.toLocal8Bit() );
    }

    inputField_->clear();
}


void ChatWidget::receiveText() {
    auto text = QString::fromLocal8Bit( socket_->readAll() );
    if( text.isEmpty() ) {
        return;
    }
    textView_->append( "Received: " + text );
}


void ChatWidget::tryConnectToServer() {
    // disable connect button while trying
    serverName_->setDisabled( true );
    connectButton_->setDisabled( true );
    connectButton_->setText( "Connecting..." );

    // connect to port 12345
    socket_->connectToHost( serverName_->text(), 12345 );
}


void ChatWidget::connectToServer() {
    connected_ = true;
    textView_->append( "*** Connected to " + socket_->peerAddress().toString() + ":" + QString::number( socket_->peerPort() ) + " ***" );
    connectButton_->setText( "Connected" );
    inputField_->setEnabled( true );
    inputField_->setFocus();
}


void ChatWidget::disconnectFromServer() {
    connected_ = false;
    serverName_->setEnabled( true );
    connectButton_->setText( "Connect" );
    connectButton_->setEnabled( true );
    textView_->append( "*** Disconnected from " + socket_->peerAddress().toString() + ":" + QString::number( socket_->peerPort() ) + " ***" );
    inputField_->setDisabled( true );
}
