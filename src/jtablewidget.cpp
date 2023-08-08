#include "jtablewidget.h"
#include <QHeaderView>

JTableWidget::JTableWidget(int columnCount, QTableWidget *parent) : QTableWidget(parent)
{
    setColumnCount(columnCount);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    verticalHeader()->setVisible(false);
    horizontalHeader()->setVisible(false);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void JTableWidget::setHeader(QStringList header)
{
 //   horizontalHeader()->setVisible(true);
    setHorizontalHeaderLabels(header);
}

void JTableWidget::addSubTitle(const QString &title, QColor color_bg, QColor color_fg)
{
    QTableWidgetItem *item = new QTableWidgetItem(title);
    item->setFlags(Qt::NoItemFlags);
    item->setTextAlignment(Qt::AlignCenter);
    item->setBackground(color_bg);
    item->setForeground(color_fg);

    insertRow(rowCount());
    setItem(rowCount()-1, 0, item);
    setSpan(rowCount()-1, 0, 1, columnCount());
}

void JTableWidget::addData(QStringList data, QString id, QString date)
{
    insertRow(rowCount());
    int row = rowCount()-1;
    int column = 0;

    if (data.count()>0)
        horizontalHeader()->setVisible(true);

    foreach(QString str, data) {
        QTableWidgetItem *item = new QTableWidgetItem(str);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        item->setData(JTABLE_ID, id);
        item->setData(JTABLE_DATE, date);
        setItem(row, column++, item);
    }
}

bool JTableWidget::isEmpty()
{
    return (rowCount() == 0) ? true : false;
}

void JTableWidget::clear()
{
    setRowCount(0);
    horizontalHeader()->setVisible(false);
}

