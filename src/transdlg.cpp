#include "common.h"
#include "transdlg.h"
#include "trans.h"
#include "payee.h"
#include "account.h"
#include "category.h"
#include "jdbtabledlg.h"
#include "jattachmentwg.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QCompleter>
#include <QDoubleSpinBox>

TransDlg::TransDlg(int tid, QDate tdate, QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Transaction");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    m_tid = tid;
    m_tdate = tdate;

    Trans *trans = new Trans;
    Payee *payee = new Payee;
    Category *category = new Category;
    Account *account = new Account;
    CategoryType categorytype;
    AccountType accounttype;

    setModal(true);
    setGeometry(DIALOG_SIZE);
    restoreGeometry(readSettings("TD_Geometry").toByteArray());

  //  qDebug() << "TransDlg::TransDlg(), tid=" << m_tid;

    if (m_tid != NODATA)
        if (!trans->fetch(m_tid)) {
            message(J_ERROR,"Trans Dialog",QString("Unable to find the id=%1").arg(m_tid));
            return;
        }

    //date
    QLabel *dateLb = new QLabel(m_tdate.toString(DATE_DMY));
    dateLb->setStyleSheet(INFO_STYLE);

    //amount
    m_amountSb = new QDoubleSpinBox;
    m_amountSb->setRange(0, 1000000000000000);
    m_amountSb->setPrefix("$ ");
    m_amountSb->setValue(trans->amount());
    m_amountSb->setStyleSheet("QDoubleSpinBox {font-size:18pt 'Times';}");
    m_amountSb->setAlignment(Qt::AlignCenter);
    m_amountSb->setMinimumWidth(100);

    //desc
    m_descEd = new QLineEdit;
    m_descEd->setText(trans->desc());

    //category
    m_categoryCb = new QComboBox;
    loadCombo(m_categoryCb, category->desclist());
    QPushButton *categoryBt = new QPushButton;
    categoryBt->setIcon(QIcon(":add-item"));
    categoryBt->setFlat(true);
    m_categoryCb->setCurrentText(category->id2desc(trans->category_id()));
    m_categoryCb->lineEdit()->setPlaceholderText("Search");

    //payee
    m_payeeCb = new QComboBox;
    loadCombo(m_payeeCb, payee->desclist());
    QPushButton *payeeBt = new QPushButton;
    payeeBt->setIcon(QIcon(":add-item"));
    payeeBt->setFlat(true);
    m_payeeCb->setCurrentText(payee->id2desc(trans->payee_id()));
    m_payeeCb->lineEdit()->setPlaceholderText("Search");

    //account
    m_accountCb = new QComboBox;
    loadCombo(m_accountCb, account->desclist());
    QPushButton *accountBt = new QPushButton;
    accountBt->setIcon(QIcon(":add-item"));
    accountBt->setFlat(true);
    m_accountCb->setCurrentText(account->id2desc(trans->account_id()));
    m_accountCb->lineEdit()->setPlaceholderText("Search");

    //attachment
    m_attachmentWg = new JAttachmentWg(tdate, trans->attachment());

    //note
    m_noteTe = new JTextEdit;
    m_noteTe->setText(trans->note());

    //save & close
    QPushButton *saveBt = new QPushButton(tr("Save"));
    QPushButton *closeBt = new QPushButton(tr("Cancel"));

    QHBoxLayout *saveLayout = new QHBoxLayout;
    saveLayout->addStretch();
    saveLayout->addWidget(saveBt);
    saveLayout->addWidget(closeBt);

    // grid
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(new QLabel(tr("Description: ")), 1, 0, 1, 1, Qt::AlignRight);
    grid->addWidget(m_descEd, 1, 1);//, 1, 2, Qt::AlignLeft);
    grid->addWidget(new QLabel(tr("Category: ")), 2, 0, 1, 1, Qt::AlignRight);
    grid->addWidget(m_categoryCb, 2, 1);
    grid->addWidget(categoryBt, 2, 2);
    grid->addWidget(new QLabel(tr("Payee: ")), 3, 0, 1, 1, Qt::AlignRight);
    grid->addWidget(m_payeeCb, 3, 1);
    grid->addWidget(payeeBt, 3, 2);
    grid->addWidget(new QLabel(tr("Account: ")), 4, 0, 1, 1, Qt::AlignRight);
    grid->addWidget(m_accountCb, 4, 1);
    grid->addWidget(accountBt, 4, 2);
    grid->addWidget(new QLabel(tr("Attachment: ")), 5, 0, 1, 1, Qt::AlignRight);
    grid->addWidget(m_attachmentWg, 5, 1, 1, 2);
    grid->addWidget(new QLabel(tr("Note: ")), 6, 0, 1, 1, Qt::AlignRight);
    grid->addWidget(m_noteTe, 6, 1, 1, 2);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(dateLb);
    vbox->addWidget(m_amountSb);
    vbox->addLayout(grid);
    vbox->addLayout(saveLayout);

    setLayout(vbox);

    connect(categoryBt, SIGNAL(clicked()), this, SLOT(onCategoryEdit()));
    connect(payeeBt, SIGNAL(clicked()), this, SLOT(onPayeeEdit()));
    connect(accountBt, SIGNAL(clicked()), this, SLOT(onAccountEdit()));
    connect(saveBt, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(closeBt, SIGNAL(clicked()), this, SLOT(close()));
    connect(this, SIGNAL(finished(int)), this, SLOT(onFinished(int)));
}

void TransDlg::loadCombo(QComboBox *cb, QStringList list)
{
    QString old;
    if (cb->count()>0) {
        old = cb->currentText();
        cb->clear();
    }

    cb->addItems(list);
    QCompleter *comp = new QCompleter(list);
    comp->setCaseSensitivity(Qt::CaseInsensitive);
    cb->setEditable(true);
    cb->setCompleter(comp);

    if (!old.isEmpty())
        cb->setCurrentText(old);
};

// SLOT

void TransDlg::onSave()
{
//    qDebug() <<"TransDlg::onSave()";

    Trans *trans = new Trans;
    Payee *payee = new Payee;
    Category *category = new Category;
    Account *account = new Account;

    double amount = m_amountSb->value();
    int category_id = category->desc2id(m_categoryCb->currentText());
    int payee_id = payee->desc2id(m_payeeCb->currentText());
    int account_id = account->desc2id(m_accountCb->currentText());
    QString attachment = m_attachmentWg->attachedFile();
    bool ok = true;

   // qDebug() <<"TransDlg::onSave()" <<m_tid <<D2S(amount) <<category_id <<payee_id <<account_id;

    if (amount == 0) {
        message(J_ERROR, "Transaction", "Amount cannot be Zero!");
        ok = false;
    }
    else if (m_descEd->text().isEmpty()) {
        message(J_ERROR, "Transaction", "Description cannot be Empty!");
        ok = false;
    }
    else if (category_id==0) {
        message(J_ERROR, "Transaction", "Category was not selected!");
        m_categoryCb->clearEditText();
        ok = false;
    }
    else if (payee_id==0) {
        message(J_ERROR, "Transaction", "Payee was not selected!");
        m_payeeCb->clearEditText();
        ok = false;
    }
    else if (account_id==0) {
        message(J_ERROR, "Transaction", "Account was not selected!");
        m_accountCb->clearEditText();
        ok = false;
    }

    if (ok) {
        m_tid = trans->updateData(m_tid, m_tdate, amount, m_descEd->text(), \
                category_id, payee_id, account_id, attachment, "", m_noteTe->toPlainText());
        emit toUpdate(m_tid, m_tdate);
        close();
    }
}

void TransDlg::onCategoryEdit()
{
    Category category;
    JDBTableDlg *dlg = new JDBTableDlg(category.dbtable(), "trans", category.fields());
    dlg->setWindowTitle("Categories");
    connect(dlg, SIGNAL(finished(int)), this, SLOT(onRefeshCategory(int)));
    dlg->show();
}

void TransDlg::onPayeeEdit()
{
    Payee payee;
    JDBTableDlg *dlg = new JDBTableDlg(payee.dbtable(), "trans", payee.fields());
    dlg->setWindowTitle("Payees");
    connect(dlg, SIGNAL(finished(int)), this, SLOT(onRefeshPayee(int)));
    dlg->show();
}

void TransDlg::onAccountEdit()
{
    Account account;
    JDBTableDlg *dlg = new JDBTableDlg(account.dbtable(), "trans", account.fields());
    dlg->setWindowTitle("Accounts");
    connect(dlg, SIGNAL(finished(int)), this, SLOT(onRefeshAccount(int)));
    dlg->show();
}

void TransDlg::onRefeshCategory(int result)
{
    Category c;
    loadCombo(m_categoryCb, c.desclist());
}

void TransDlg::onRefeshPayee(int result)
{
    Payee p;
    loadCombo(m_payeeCb, p.desclist());
}

void TransDlg::onRefeshAccount(int result)
{
    Account a;
    loadCombo(m_accountCb, a.desclist());
}

void TransDlg::onFinished(int result)
{
 //   qDebug() << "TransDlg::onFinished() " << m_tid << m_tdate << result;

    writeSettings("TD_Geometry", saveGeometry());;
}
