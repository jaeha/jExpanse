#include "jlistwidget.h"
#include <QListWidgetItem>

#define LIST_KEY    1000
#define LIST_DESC   "desc"

JListWidget::JListWidget(QWidget *parent) : QListWidget(parent)
{
    setSortingEnabled(true);
    connect(this, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), \
            this, SLOT(onItemChanged(QListWidgetItem*,QListWidgetItem*)));
}

int JListWidget::currentKey()
{
    return currentItem()->data(LIST_KEY).toInt();
}

void JListWidget::update(IdMap data)
{
    IdMapIterator i(data);
    while (i.hasNext()) {
        i.next();
        QListWidgetItem *item = new QListWidgetItem(i.value());
        item->setData(LIST_KEY, i.key());
        addItem(item);
    }
}

void JListWidget::add(int key, QString data, bool isCurrentItem)
{
    QListWidgetItem *item = new QListWidgetItem(data);
    item->setData(LIST_KEY, key);
    addItem(item);
    if (isCurrentItem)
        setCurrentItem(item);
}


// SLOTS
void JListWidget::onAdd(int key, QString data)
{
   // qDebug() << "JListWidget::onAdd()";
    add(key, data, true);
}

void JListWidget::onUpdateCurrent(QString data)
{
    currentItem()->setText(data);
}

void JListWidget::onDeleteCurrent()
{
    int id = currentItem()->data(LIST_KEY).toInt(); // this should be call before remove the current row;
    if (confirm(this, QString("Do you really want to delete %1?").arg(currentItem()->text()))) {
        takeItem(currentRow());
        emit toDeleteDB(id);
    }
}

void JListWidget::onItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    emit toListChanged(current->data(LIST_KEY).toInt(), current->data(Qt::DisplayRole).toString());
}
