#include "jtextedit.h"

JTextEdit::JTextEdit(QWidget *parent) : QTextEdit(parent)
{
    connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
}

void JTextEdit::onTextChanged()
{
    emit toTextChanged(this->toPlainText());
}

