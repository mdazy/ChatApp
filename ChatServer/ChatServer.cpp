#include "ChatServer.h"

#include <QtCore/QMap>

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QTcpSocket>

#include <iostream>
using namespace std;


/*!
 * Convenience for displaying a socket address in standard streams.
 */
ostream& operator<<( ostream& out, const QAbstractSocket* s ) {
    out << s->peerAddress().toString().toStdString() << ":" << s->peerPort();
    return out;
}


/*!
 * Starts the server on port 12345.
 */
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


/*!
 * Registers a new client with a default empty nickname.
 */
void ChatServer::inspectConnection() {
    while( hasPendingConnections() ) {
        auto socket = nextPendingConnection();
        cerr << "new connection from " << socket << endl;
        connect( socket, SIGNAL( readyRead() ), this, SLOT( dispatch() ) );
        connect( socket, SIGNAL( disconnected() ), this, SLOT( disconnectClient() ) );
        clientSockets_.insert( socket, QString() );
    }
}


/*!
 * Sends a message to all clients except \a from if specified.
 */
void ChatServer::sendToClients( const QString& msg, const QTcpSocket* from ) const {
    for( auto s = clientSockets_.keyBegin(); s != clientSockets_.keyEnd(); ++s ) {
        if( *s == from ) {
            continue;
        }
        ( *s )->write( msg.toLocal8Bit() );
    }
}


/*!
 * Processes the message received from the sender.
 *
 * If it is the first message from that sender it is stored as the nickname for that client
 * and a connection notification is sent.
 *
 * If it does not start with the expected nick, the new message is stored as the new nick
 * and a renaming notification is sent.
 *
 * Otherwise the message is sent as-is.
 */
void ChatServer::dispatch() {
    auto from = static_cast<QTcpSocket*>( sender() );
    QString text = QString::fromLocal8Bit( from->readAll() );

    cerr << "received from " << from << ": " << text.toStdString() << endl;

    if( clientSockets_[ from ].isEmpty() ) {
        // first message: store nick and send connection notification
        cerr << "nick for new connection" << endl;
        clientSockets_[ from ] = text;
        text += " connected.";
    } else if( !text.startsWith( clientSockets_[ from ] + ": " ) ) {
        // new nick, update and notify
        cerr << "new nick for existing connection" << endl;
        QString oldNick = clientSockets_[ from ];
        clientSockets_[ from ] = text;
        text = oldNick + " has changed his nick to " + text + ".";
    } else {
        // regular text
    }

    sendToClients( text, from );
}


/*!
 * Notifies other clients that the sender has disconnected.
 */
void ChatServer::disconnectClient() {
    auto from = static_cast<QTcpSocket*>( sender() );
    sendToClients( clientSockets_[ from ] + " disconnected.", from );
    clientSockets_.remove( from );
}