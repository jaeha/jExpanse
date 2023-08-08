#ifndef JTEXTEDIT_H
#define JTEXTEDIT_H

#include <QTextEdit>

class JTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit JTextEdit(QWidget *parent = nullptr);

signals:
    void toTextChanged(QString data);

public slots:
    void onTextChanged();
};

#endif // JTEXTEDIT_H
