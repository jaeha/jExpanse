#ifndef JTABLEWIDGET_H
#define JTABLEWIDGET_H

#include <QTableWidget>

#define JTABLE_ID   1001
#define JTABLE_DATE 1002

class JTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit JTableWidget(int columnCount, QTableWidget *parent = 0);
    void setHeader(QStringList header);
    void addSubTitle(const QString &title, QColor color_bg, QColor color_fg);
    void addData(QStringList data, QString id, QString date = "");
    bool isEmpty();
    void clear();
};

#endif // JTABLEWIDGET_H
