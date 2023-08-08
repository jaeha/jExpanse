#ifndef JINPUTDLG_H
#define JINPUTDLG_H

#include "common.h"
#include <QDialog>
#include <QComboBox>
#include <QLineEdit>

class JInputDlg : public QDialog
{
    Q_OBJECT

public:
    explicit JInputDlg(const QString &msg, FieldType type, QStringList fields, QWidget *parent=0);
    QString result();

private:
    QComboBox *m_combo;
    QLineEdit *m_line;

};

#endif // JINPUTDLG_H
