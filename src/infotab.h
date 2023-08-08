#ifndef INFOTAB_H
#define INFOTAB_H

#include <QTabWidget>
#include <QTableWidgetItem>
#include <QDate>

class InfoTab : public QTabWidget
{
    Q_OBJECT
public:
    explicit InfoTab(QWidget *parent = 0);

private:
    void monthSummary(QDate date);

signals:
    void toInfoSelected(int);

public slots:
    void onUpdate(QDate);
    void onMonthItemClicked(QTableWidgetItem*);

};

#endif // INFOTAB_H
