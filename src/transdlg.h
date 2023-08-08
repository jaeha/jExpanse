#ifndef TRANSDLG_H
#define TRANSDLG_H

#include "jtextedit.h"
#include "jattachmentwg.h"
#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QDate>

class TransDlg : public QDialog
{
    Q_OBJECT
public:
    explicit TransDlg(int tid, QDate tdate, QWidget *parent = nullptr);

private:
    void loadCombo(QComboBox *cb, QStringList list);

    QDoubleSpinBox *m_amountSb;
    QLineEdit *m_descEd;
    QComboBox *m_categoryCb, *m_payeeCb, *m_accountCb;
    JTextEdit *m_noteTe;
    JAttachmentWg *m_attachmentWg;
    int m_tid;
    QDate m_tdate;

signals:
    void toUpdate(int tid, QDate tdate);

public slots:
    void onSave();
    void onCategoryEdit();
    void onPayeeEdit();
    void onAccountEdit();
    void onRefeshCategory(int);
    void onRefeshPayee(int);
    void onRefeshAccount(int);
    void onFinished(int);
};

#endif // TRANSDLG_H
