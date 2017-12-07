#pragma once

#include <QtWidgets/QWidget>

class QLineEdit;
class QTextEdit;
class QTcpSocket; 

class ChatWidget : public QWidget {
    Q_OBJECT;
public:
    ChatWidget( QWidget* parent = Q_NULLPTR );

protected slots:
    void sendText();
    void connectToServer();
    void disconnectFromServer();

protected:
    QTextEdit* textView_;
    QLineEdit* inputField_;
    QTcpSocket* socket_;
    bool connected_;
};
