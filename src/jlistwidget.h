#ifndef JLISTWIDGET_H
#define JLISTWIDGET_H

#include "common.h"
#include <QListWidget>

class JListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit JListWidget(QWidget *parent = nullptr);

    int currentKey();
    void update(IdMap data);
    void add(int key, QString data, bool isCurrentItem=true);

signals:
    void toDeleteDB(int id);
    void toListChanged(int id, QString data);

private slots:
    void onAdd(int key, QString data);
    void onUpdateCurrent(QString data);
    void onDeleteCurrent();
    void onItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
};

#endif // JLISTWIDGET_H
