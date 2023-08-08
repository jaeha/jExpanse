#ifndef SEARCHWG_H
#define SEARCHWG_H

#include "jtablewidget.h"
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDateTimeEdit>

class SearchWg : public QWidget
{
    Q_OBJECT
public:
    explicit SearchWg(QWidget *parent = 0);

private:
    QLineEdit *m_searchEd;
    QComboBox *m_fieldCb;
   // QDateTimeEdit *m_dateFromEd, *m_dateToEd;
    QLineEdit *m_dateFromEd, *m_dateToEd;
    JTableWidget *m_out;

signals:
    void toSearchSelected(int id, QDate date);

public slots:
    void onSearch();
    void onComboChanged(QString);
    void onItemClicked(QTableWidgetItem *item);
};

#endif // SEARCHWG_H
