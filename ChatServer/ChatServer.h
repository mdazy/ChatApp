#pragma once

#include <QtNetwork/QTcpServer>

#include <vector>

class ChatServer : public QTcpServer {
Q_OBJECT
public:
    ChatServer( QObject* parent = Q_NULLPTR );

public slots:
    void inspectConnection();
    void dispatch();
    void disconnectClient();

protected:
    void sendToClients( const QString& msg, const QTcpSocket* from = Q_NULLPTR ) const;

protected:
    QMap<QTcpSocket*, QString> clientSockets_;
};