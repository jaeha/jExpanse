#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include "jdb.h"
#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>

class SettingsDlg : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDlg(QWidget *parent = nullptr);

private:
    QPushButton *m_basepathBt;
    QLineEdit *m_basepathEd;
    QTextEdit  *m_appStyleTx;
    int m_dispItem;

    void closeEvent(QCloseEvent *event);

signals:
    void toSelectDispItem(QString);

public slots:
    void onSave();
    void onClickedBasepathBt();
    void onClickedDispDesc();
    void onClickedDispPayee();
};

#endif // SETTINGSDLG_H
