#include "infotab.h"
#include "trans.h"
#include "jtablewidget.h"
#include "category.h"
#include "common.h"

InfoTab::InfoTab(QWidget *parent) : QTabWidget(parent)
{
    setGeometry(0,0,50,50);
}

void InfoTab::monthSummary(QDate date)
{
 //   qDebug() << "InfoTab::monthSummary()" << date;
    Trans trans;
    CategoryType categorytype;
    JTableWidget *out = new JTableWidget(2);
    out->setHeader(QStringList()<<"Category" <<"Amount");

    IdMapIterator i(categorytype.desclist());
    while (i.hasNext()) {
        double total = 0;
        i.next();
        out->addSubTitle(i.value(), COLOR_TITLE_BG, COLOR_TITLE_FG);
        foreach (QStringList sl, trans.categorySum(QString("%Y-%m"), date.toString(DATE_YM), i.value())) {
           // qDebug() << sl;
            QStringList list = QStringList() << sl.at(CSDESC);
            double amt = sl.at(CSTOTAL).toDouble();
            total += amt;
            list << DOLLAR_FORMAT(amt);

            out->addData(list, sl.at(CSID));
        }
        // add total of each catgory type
        out->addSubTitle(QString("Total: $%L1").arg(DOLLAR_FORMAT(total)), COLOR_TOTAL_BG, COLOR_TOTAL_FG);
    }

    addTab(out, tr("Month Summary"));

    //signal
    connect(out, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onMonthItemClicked(QTableWidgetItem*)));
}


// SLOT

void InfoTab::onUpdate(QDate date)
{
    clear();
    monthSummary(date);
}

void InfoTab::onMonthItemClicked(QTableWidgetItem* item)
{
    emit toInfoSelected(item->data(JTABLE_ID).toInt());
}


