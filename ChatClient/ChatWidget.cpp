#include "ChatWidget.h"

#include <QtWidgets/QLineEdit>
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

    textView_ = new QTextEdit( this );
    textView_->setObjectName( "text view" );
    textView_->setReadOnly( true );
    textView_->setFocusPolicy( Qt::NoFocus );
    l->addWidget( textView_ );

    inputField_ = new QLineEdit( this );
    inputField_->setObjectName( "input field" );
    inputField_->setFocus();
    l->addWidget( inputField_ );

    connect( inputField_, SIGNAL( returnPressed() ), this, SLOT( sendText() ) );

    // connect to local server
    auto hostInfo = QHostInfo::fromName( QHostInfo::localHostName() );
    QHostAddress myAddress;
    for( const auto& a : hostInfo.addresses() ) {
        if( a.protocol() != QAbstractSocket::IPv4Protocol ) {
            continue;
        }
        if( ( a.toIPv4Address() & 0xff ) == 1 ) {
            continue;
        }
        myAddress = a;
        break;
    }

    socket_ = new QTcpSocket( this );
    connect( socket_, SIGNAL( connected() ), this, SLOT( connectToServer() ) );
    connect( socket_, SIGNAL( disconnected() ), this, SLOT( disconnectFromServer() ) );
    connect( socket_, SIGNAL( readyRead() ), this, SLOT( receiveText() ) );
    socket_->connectToHost( myAddress, 12345 );
}


void ChatWidget::sendText() {
    auto text = inputField_->text();
    if( text.isEmpty() ) {
        return;
    }
    if( connected_ ) {
        // send to server
        socket_->write( text.toLocal8Bit() );
    }
    textView_->append( text );
    inputField_->clear();
}


void ChatWidget::receiveText() {
    auto text = QString::fromLocal8Bit( socket_->readAll() );
    if( text.isEmpty() ) {
        return;
    }
    textView_->append( "Received: " + text );
}


void ChatWidget::connectToServer() {
    connected_ = true;
    textView_->append( "*** Connected to " + socket_->peerAddress().toString() + ":" + QString::number( socket_->peerPort() ) + " ***" );
}

void ChatWidget::disconnectFromServer() {
    connected_ = false;
    textView_->append( "*** Disconnected from " + socket_->peerAddress().toString() + ":" + QString::number( socket_->peerPort() ) + " ***" );
}
