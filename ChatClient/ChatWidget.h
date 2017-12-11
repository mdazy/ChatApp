#pragma once

#include <QtWidgets/QWidget>

class QLineEdit;
class QPushButton;
class QSettings;
class QTextEdit;
class QTcpSocket; 

class ChatWidget : public QWidget {
    Q_OBJECT;
public:
    ChatWidget( QWidget* parent = Q_NULLPTR );
    virtual ~ChatWidget();

protected slots:
    void sendText();
    void receiveText();
    void tryConnectToServer();
    void connectToServer();
    void disconnectFromServer();

protected:
    QString nick() const;
    void log( const QString& text ) const;

protected:
    QLineEdit* serverName_;
    QPushButton* connectButton_;
    QLineEdit* nickName_;
    QTextEdit* textView_;
    QLineEdit* inputField_;
    QTcpSocket* socket_;
    bool connected_;
    QString prevNick_;
    QSettings* settings_;
};
