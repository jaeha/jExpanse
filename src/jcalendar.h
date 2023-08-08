#ifndef JCALENDAR_H
#define JCALENDAR_H

#include <QtGui>
#include <QObject>
#include <QWidget>
#include <QTableView>
#include <QDate>
#include <QStandardItemModel>
#include <QListWidget>
#include <QListWidgetItem>
#include "common.h"

enum CalendarItem {ITEM_ID=1001, ITEM_CATEGORY};
enum CalendarData {CID=0, CATEGORY_ID, DESC, PAYEE, AMOUNT};
enum CalendarCopyMode {NONE=0, COPY, CUT};

class JCalendar : public QTableView
{
    Q_OBJECT
public:
    explicit JCalendar(QDate date, IdMMap data, QTableView *parent = 0);
    void setCategory(int id);
    QString lastError() {return m_lastError;};

private:   
    void drawData(QDate date, IdMMap data);
    void createActions();

    QDate m_selectedDate;
    int m_selectedId;
    QPoint m_selectedPos;
    int m_selectedCategory;
    int m_copyId;
    QStandardItemModel *m_model;
    QString m_lastError;
    CalendarCopyMode m_copyMode;
    QAction *m_newAct, *m_copyAct, *m_cutAct, *m_pasteAct, *m_deleteAct;

signals:
    void toSelect(int, QDate, QPoint);
    void toEdit(int, QDate, QPoint);
    void toMove(int, QDate);
    void toPaste(int, QDate, CalendarCopyMode);
    void toDelete(int);
    void toSetData(int, QStringList);
    void toDebug(int, QString);
    // to JCalList
    void toSelectList(int, CalendarItem);
    void toUpdateList(int, int, QStringList);

private slots:
    void onUpdate(int, int, QStringList);
    void onSelect(int, int, QPoint);
    void onSelectMonth(QDate, IdMMap);
    void onSelectItem(int, CalendarItem);
    void onShowContextMenu(QPoint);
    void onEdit(int, int, QPoint);
    void onMove(int, int, int);
    void onCopy();
    void onCut();
    void onPaste();
    void onDelete();
    void onNew();

};


// ########
// JCalList
// ########

class JCalList : public QListWidget
{
    Q_OBJECT

public:
    explicit JCalList(int day, QList<QStringList> list, QTableView *parent =  0);
    void setBackground(const QBrush &bg);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void keyPressEvent (QKeyEvent* event);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    void performDrag();      
    QString itemTitle(QStringList list);

    QPoint m_movePos;
    int m_day;
    QList<QStringList> m_list;
    CalendarData m_dispItem;

signals:
    void toNew();
    void toSelect(int, int, QPoint);
    void toEdit(int, int, QPoint);
    void toMove(int, int, int);
    void toCopy();
    void toCut();
    void toPaste();
    void toDelete();
    void toShowContextMenu(QPoint);

public Q_SLOTS:
    void onSelectList(int, CalendarItem);
    void onUpdateList(int, int, QStringList);

};


#endif // JCALENDAR_H
