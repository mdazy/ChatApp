#include "ChatServer.h"

#include <QtNetwork/QHostInfo>
#include <QtNetwork/QHostAddress>

#include <iostream>
using namespace std;

ChatServer::ChatServer( QObject* parent ) : QTcpServer( parent ) {
    auto hostInfo = QHostInfo::fromName( QHostInfo::localHostName() );
    for( const auto& a : hostInfo.addresses() ) {
        if( a.protocol() != QAbstractSocket::IPv4Protocol ) {
            continue;
        }
        if( ( a.toIPv4Address() & 0xff ) == 1 ) {
            continue;
        }
        cerr << a.toString().toStdString() << endl;
    }
}