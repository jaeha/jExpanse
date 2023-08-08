#include "searchwg.h"
#include "common.h"
#include "trans.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCompleter>
#include <QLabel>

SearchWg::SearchWg(QWidget *parent) : QWidget(parent)
{
    setGeometry(0,0,50,50);

    m_searchEd = new QLineEdit;
    m_searchEd->setPlaceholderText("Search");

    m_fieldCb = new QComboBox;
    m_fieldCb->addItems(QStringList() <<"All" <<"Category" <<"Payee" <<"Account");

    QDate today = QDate::currentDate();
    m_dateFromEd = new QLineEdit;
    m_dateFromEd->setInputMask("0000-00-00");
  //  m_dateFromEd->setPlaceholderText(N2S(1) + "/" + N2S(today.month()) + "/" + N2S(today.year()));

    m_dateToEd = new QLineEdit;
    m_dateToEd->setInputMask("0000-00-00");
 //   m_dateToEd->setPlaceholderText(N2S(today.daysInMonth()) + "/" + N2S(today.month()) + "/" + N2S(today.year()));

    m_out = new JTableWidget(7);
    m_out->setHeader(QStringList() <<"Date" <<"Amount" <<"Desc" <<"Payee" <<"Category" <<"Account" <<"Note");

    QGridLayout *sgrid = new QGridLayout;
    sgrid->addWidget(new QLabel(tr("Search:")), 0, 0);
    sgrid->addWidget(m_searchEd, 0, 1, 1, 5);
    sgrid->addWidget(new QLabel(tr("Date:")), 1, 0);
    sgrid->addWidget(m_dateFromEd, 1, 1);
    sgrid->addWidget(new QLabel(tr("to:")), 1, 2);
    sgrid->addWidget(m_dateToEd, 1, 3);
    sgrid->addWidget(new QLabel(tr("in:")), 1, 4);
    sgrid->addWidget(m_fieldCb, 1, 5);
    sgrid->setMargin(0);
    sgrid->setSpacing(5);

    QVBoxLayout *mvbox = new QVBoxLayout;
    mvbox->addLayout(sgrid);
    mvbox->addWidget(m_out);

    m_searchEd->setFocus();
    setLayout(mvbox);

    connect(m_searchEd, SIGNAL(returnPressed()), this, SLOT(onSearch()));
    connect(m_dateFromEd, SIGNAL(returnPressed()), this, SLOT(onSearch()));
    connect(m_dateToEd, SIGNAL(returnPressed()), this, SLOT(onSearch()));
    connect(m_fieldCb, SIGNAL(currentTextChanged(QString)), this, SLOT(onComboChanged(QString)));
    connect(m_out, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onItemClicked(QTableWidgetItem*)));
}

// SLOTs

void SearchWg::onComboChanged(QString text)
{
    onSearch();
}

void SearchWg::onSearch()
{
  //  qDebug() << "SearchWg::onSearch()";

    Trans trans;
    QList<QStringList>data = trans.search(m_searchEd->text(), m_fieldCb->currentText(), \
                                          m_dateFromEd->text(), m_dateToEd->text());
    m_out->clear();
    if (!data.isEmpty()) {
        double total=0, amount=0;
        foreach (QStringList sl, data) {
            amount = sl.at(STAMOUNT).toDouble();
            if (!sl.at(SCCTYPE).compare("Income", Qt::CaseInsensitive))
                total += amount;
            else
                total -= amount;
            sl.replace(STAMOUNT, DOLLAR_FORMAT(amount));
            QString id = sl.at(STID);
            QString date = sl.at(STDATE);
            sl.removeFirst();
            sl.removeLast();
            m_out->addData(sl, id, date);
        }
   //     qDebug() << "total: " << total;
        m_out->addSubTitle(DOLLAR_FORMAT(total), COLOR_TOTAL_BG, COLOR_TOTAL_FG);
    } else
        m_out->addSubTitle("Not Found.", Qt::white, Qt::black);

    for (int i=3; i<6; i++)
        m_out->hideColumn(i);
}


void SearchWg::onItemClicked(QTableWidgetItem *item)
{
    emit toSearchSelected(item->data(JTABLE_ID).toInt(), item->data(JTABLE_DATE).toDate());
}
