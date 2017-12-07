#include "ChatServer.h"

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QTcpSocket>

#include <iostream>
using namespace std;


ostream& operator<<( ostream& out, const QAbstractSocket* s ) {
    out << s->peerAddress().toString().toStdString() << ":" << s->peerPort();
    return out;
}


ChatServer::ChatServer( QObject* parent ) : QTcpServer( parent ) {
    // pick a single IPv4 address for the local host
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
    // start on port 12345
    if( !listen( myAddress, 12345 ) ) {
        // TODO: how to diagnose failure
        cerr << "NOT LISTENING" << endl;
    } else {
        connect( this, SIGNAL( newConnection() ), this, SLOT( inspectConnection() ) );
    }
}

void ChatServer::inspectConnection() {
    while( hasPendingConnections() ) {
        auto socket = nextPendingConnection();
        cerr << "new connection from " << socket << endl;
        connect( socket, SIGNAL( readyRead() ), this, SLOT( dispatch() ) );
        connect( socket, SIGNAL( disconnected() ), this, SLOT( disconnectClient() ) );
        clientSockets_.push_back( socket );
    }
}


void ChatServer::dispatch() {
    auto from = static_cast<QTcpSocket*>( sender() );
    QString text = QString::fromLocal8Bit( from->readAll() );
    cerr << "received " << text.toStdString() << " from " << from << endl;
    for( const auto s : clientSockets_ ) {
        if( s == from ) {
            continue;
        }
        cerr << "   dispatching to " << s << endl;
        s->write( text.toLocal8Bit() );
    }
}


void ChatServer::disconnectClient() {
    auto from = static_cast<QTcpSocket*>( sender() );
    cerr << from << " disconnected" << endl;
    clientSockets_.removeOne( from );
}