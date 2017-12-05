#pragma once

#include <QtWidgets/QWidget>

class QLineEdit;
class QTextEdit;

class ChatWidget : public QWidget {
    Q_OBJECT;
public:
    ChatWidget( QWidget* parent = Q_NULLPTR );

protected slots:
    void sendText();

protected:
    QTextEdit* textView_;
    QLineEdit* inputField_;
};
