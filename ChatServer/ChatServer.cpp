#include "ChatServer.h"

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QTcpSocket>

#include <iostream>
using namespace std;

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
        cerr << "new connection from " << socket->peerAddress().toString().toStdString() << ":" << socket->peerPort() << endl;
    }
}