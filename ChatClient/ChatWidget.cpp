#include "ChatWidget.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

ChatWidget::ChatWidget( QWidget* parent ) : QWidget( parent ) {
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
}


void ChatWidget::sendText() {
    auto text = inputField_->text();
    if( text.isEmpty() ) {
        return;
    }
    textView_->append( inputField_->text() );
    inputField_->clear();
}