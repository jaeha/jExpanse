 #include "jcalendar.h"
#include <QListWidget>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QLabel>
#include <QModelIndex>
#include <QApplication>
#include <QAction>
#include <QMenu>

#define NODATA  -1
#define MAX_RAW 6
#define MAX_COL 7
#define MIME_FORMAT  "application/x-fc"
#define DATE_YYMM    "yyyy-MM"
#define COLOR_SELECTED      Qt::cyan
#define FONT_DAY    QFont("Helvetica [Cronyx]", 18)

JCalendar::JCalendar(QDate date, IdMMap data, QTableView *parent) :
    QTableView(parent)
{
 //   qDebug() << "JCalendar Starting..!";

    m_copyMode = NONE;
    m_selectedId = NODATA;
    m_selectedDate = date;
    m_model = new QStandardItemModel(MAX_RAW,MAX_COL);
    setModel(m_model);

    QPalette p = palette();
    p.setColor(QPalette::Active, QPalette::Highlight, Qt::white);
   // p.setColor(QPalette::Inactive, QPalette::Highlight, Qt::white);
    setPalette(p);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setSectionsClickable(false);
    verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->hide();

    drawData(date, data);
    createActions();
    show();
}

void JCalendar::createActions()
{
    m_newAct = new QAction(tr("&New"), this);
    m_newAct->setShortcut(QKeySequence::New);
    connect(m_newAct, SIGNAL(triggered()), this, SLOT(onNew()));

/*    m_editAct = new QAction(tr("&Edit"), this);
    m_newAct->setShortcut(QKeySequence::Edit);
    connect(m_newAct, SIGNAL(triggered()), this, SLOT(onNew()));
*/
    m_copyAct = new QAction(tr("&Copy"), this);
    m_copyAct->setShortcut(QKeySequence::Copy);
    connect(m_copyAct, SIGNAL(triggered()), this, SLOT(onCopy()));

    m_cutAct = new QAction(tr("&Cut"), this);
    m_cutAct->setShortcut(QKeySequence::Cut);
    connect(m_cutAct, SIGNAL(triggered()), this, SLOT(onCut()));

    m_pasteAct = new QAction(tr("&Paste"), this);
    m_pasteAct->setShortcut(QKeySequence::Paste);
    m_pasteAct->setEnabled(false);
    connect(m_pasteAct, SIGNAL(triggered()), this, SLOT(onPaste()));

    m_deleteAct = new QAction(tr("&Delete"), this);
    m_deleteAct->setShortcut(QKeySequence::Delete);
    connect(m_deleteAct, SIGNAL(triggered()), this, SLOT(onDelete()));
}

void JCalendar::drawData(QDate date, IdMMap data)
{
    QDate today = QDate::currentDate();
    QDate cdate = QDate(date.year(), date.month(), 1); /// this should be the first day of the month.
    int startday = cdate.dayOfWeek()-1;
    int lastday = cdate.daysInMonth();
    int rows = (lastday / MAX_COL) + (((lastday % MAX_COL) + startday) / MAX_COL)+1;

    m_model->setHorizontalHeaderLabels(QStringList()<<"Monday" <<"Tuesday"
                                     <<"Wednesday" <<"Thorsday" <<"Friday"
                                     <<"Saturday" <<"Sunday");
    m_model->setRowCount(0);
    m_model->setRowCount(rows);
   // model->setColumnCount(MAX_COL);

    int day = 0;
    for (int i=0; i<rows && day<=lastday; i++) {
        for (int j=0; j<MAX_COL && day<=lastday; j++) {
            if (i == 0 && j == startday)
                day = 1;
            if (day > 0) {
                JCalList *list = new JCalList(day, data.values(day), this);

                if (cdate.toString(DATE_YYMM) == today.toString(DATE_YYMM) && day == today.day())
                    list->setBackground(QBrush(Qt::gray));

                setIndexWidget(m_model->index(i,j), list);
                day++;

                connect(list, SIGNAL(toSelect(int,int,QPoint)), this, SLOT(onSelect(int,int,QPoint)));
                connect(list, SIGNAL(toEdit(int, int, QPoint)), this, SLOT(onEdit(int, int, QPoint)));
                connect(list, SIGNAL(toMove(int,int,int)), this, SLOT(onMove(int,int,int)));
                connect(list, SIGNAL(toCopy()), this, SLOT(onCopy()));
                connect(list, SIGNAL(toCut()), this, SLOT(onCut()));
                connect(list, SIGNAL(toPaste()), this, SLOT(onPaste()));
                connect(list, SIGNAL(toDelete()), this, SLOT(onDelete()));
                connect(list, SIGNAL(toNew()), this, SLOT(onNew()));
                connect(list, SIGNAL(toShowContextMenu(QPoint)), this, SLOT(onShowContextMenu(QPoint)));
                connect(this, SIGNAL(toUpdateList(int, int, QStringList)), list, SLOT(onUpdateList(int, int, QStringList)));
                connect(this, SIGNAL(toSelectList(int, CalendarItem)), list, SLOT(onSelectList(int, CalendarItem)));
            }
        }
    }
}

// SLOTS

void JCalendar::onSelectItem(int id, CalendarItem citem)
{
    setFocus();
    emit toSelectList(id, citem);
}

void JCalendar::onUpdate(int id, int day, QStringList data)
{
     emit toUpdateList(id, day, data);
}

void JCalendar::onSelect(int id, int day, QPoint pos)
{
    m_selectedDate = QDate(m_selectedDate.year(), m_selectedDate.month(), day);
    m_selectedId = id;
    m_selectedPos = pos;

    if (id == NODATA) {
        m_newAct->setEnabled(true);
        m_copyAct->setEnabled(false);
        m_cutAct->setEnabled(false);
        m_pasteAct->setEnabled(true);
        m_deleteAct->setEnabled(false);
    } else {
        m_newAct->setEnabled(true);
        m_copyAct->setEnabled(true);
        m_cutAct->setEnabled(true);
        m_pasteAct->setEnabled(true);
        m_deleteAct->setEnabled(true);
    }

    emit toSelect(m_selectedId, m_selectedDate, m_selectedPos);
}

void JCalendar::onEdit(int id, int day, QPoint pos)
{
    m_selectedId = id;
    m_selectedDate = QDate(m_selectedDate.year(), m_selectedDate.month(), day);

    emit toEdit(id, m_selectedDate, pos);
}

void JCalendar::onMove(int id, int oldDay, int newDay)
{
  //  qDebug() << "JCalendar::onMove(): " << id << oldDay << newDay;

    emit toMove(id, QDate(m_selectedDate.year(), m_selectedDate.month(), newDay));
}

void JCalendar::onSelectMonth(QDate date, IdMMap data)
{
    m_selectedDate = date;
    drawData(m_selectedDate, data);
}

void JCalendar::onCopy()
{
    if (m_selectedId == NODATA) {
        m_lastError = "Copy: No item has been selected.";
        return;
    }

    m_copyId = m_selectedId;
    m_copyMode = COPY;
    m_copyAct->setEnabled(false);
    m_cutAct->setEnabled(false);
    m_pasteAct->setEnabled(true);
}

void JCalendar::onCut()
{
 //   qDebug() << "JCalendar::onCut()" << m_selectedId << m_selectedDate;

    if (m_selectedId == NODATA) {
        m_lastError = "Copy: No item has been selected.";
        return;
    }

    m_copyId = m_selectedId;
    m_copyMode = CUT;
    m_copyAct->setEnabled(false);
    m_cutAct->setEnabled(false);
    m_pasteAct->setEnabled(true);

    emit toUpdateList(m_copyId, m_selectedDate.toString("d").toInt(), QStringList()<<"");
}

void JCalendar::onPaste()
{
  // qDebug() << "JCalendar::onPaste()" << m_copyId << m_selectedDate;

    if (m_copyId == NODATA) {
        m_lastError = "Paste: No item has been selected.";
        return;
    }

    emit toPaste(m_copyId, m_selectedDate, m_copyMode);

    if (m_copyMode == CUT)
        m_copyMode = NONE;  // this prevent past repeatedly after CUT.

    m_copyAct->setEnabled(true);
    m_cutAct->setEnabled(true);
    m_pasteAct->setEnabled(false);
}

void JCalendar::onDelete()
{
    if (!confirm(this, "Do you really want to delete this transaction?"))
        return;

    emit toUpdateList(m_selectedId, m_selectedDate.toString("d").toInt(), QStringList()<<"");
    emit toDelete(m_selectedId);
}

void JCalendar::onNew()
{
    emit toEdit(NODATA, m_selectedDate, m_selectedPos);
}

void JCalendar::onShowContextMenu(QPoint pos)
{
    QMenu menu(this);
    menu.addAction(m_newAct);
    menu.addAction(m_copyAct);
    menu.addAction(m_cutAct);
    menu.addAction(m_pasteAct);
    menu.addAction(m_deleteAct);

    menu.exec(pos);
}

// ########
// JCalList
// ########

JCalList::JCalList(int day, QList<QStringList> data, QTableView *parent) :
    QListWidget(parent)
{
    //qDebug() << "JCalList() : " << day << data;

    m_day = day;

    m_dispItem = (CalendarData)readSettings("CalendarDispItem").toInt();
    if (m_dispItem <= DESC) {
        m_dispItem = DESC;
        writeSettings("CalendarDispItem", DESC);
    }

    setFrameStyle(0);
    setStyleSheet("QListView::item:selected { background-color: white; color: black; }");
   // setStyleSheet("QListView::item:unselected { background-color: white; color: black; }");
    //setAlternatingRowColors(true);
   // setStyleSheet("alternate-background-color: white;background-color: white;");
  //  setDragDropMode(QAbstractItemView::InternalMove);
    setDropIndicatorShown(true);
    setAcceptDrops(true);
   // setDragEnabled(true);

    // color
    QPalette p = palette();
    p.setColor(QPalette::Active, QPalette::Highlight, Qt::blue);
    p.setColor(QPalette::Inactive, QPalette::Highlight, Qt::white);
    setPalette(p);

    // day
    QListWidgetItem *itemDay = new QListWidgetItem(QString("%1").arg(day), this);
    itemDay->setData(ITEM_ID, NODATA);
    itemDay->setTextAlignment(Qt::AlignRight);
    itemDay->setFont(FONT_DAY);
    itemDay->setFlags(itemDay->flags() &~ Qt::ItemIsSelectable &~ Qt::ItemIsUserCheckable);

    //data
    //foreach (QStringList slist, data) {
    for (int i=data.size(); i>0; i--) {
        QStringList slist = data.at(i-1);
        if (!slist.isEmpty()) {
            QListWidgetItem *itemData = new QListWidgetItem(itemTitle(slist), this);
            itemData->setData(ITEM_ID, slist.at(CID));
            itemData->setData(ITEM_CATEGORY, slist.at(CATEGORY_ID));
        }
    }
}

QString JCalList::itemTitle(QStringList list)
{
    return QString("%1 $%L2").arg(list.at(m_dispItem)).arg(list.at(AMOUNT).toDouble(),0,'f',2);
}

void JCalList::setBackground(const QBrush &bg) {
    item(0)->setBackground(bg);
}

void JCalList::mouseDoubleClickEvent(QMouseEvent *event)
{
 //   qDebug() << "JCalList::mouseDoubleClickEvent()";

    QPoint pos = event->pos();
    QListWidgetItem *item = itemAt(pos);
    int id = NODATA;

    if (item)
        id = item->data(ITEM_ID).toInt();

    emit toEdit(id, m_day, mapToGlobal(pos));

    QListWidget::mouseDoubleClickEvent(event);
}

void JCalList::mousePressEvent(QMouseEvent *event)
{
  //  qDebug() << "JCalList::mousePressEvent()";

    QPoint pos = event->pos();

    if (event->button() != Qt::LeftButton)
             m_movePos = pos;

    QListWidgetItem *item = itemAt(pos);
    int id = NODATA;
    if (item)
        id = item->data(ITEM_ID).toInt();

  //  qDebug() << "JCalList::mousePressEvent() id, day" << id << m_day;

    emit toSelect(id, m_day, mapToGlobal(pos));

    QListWidget::mousePressEvent(event);
}

void JCalList::mouseMoveEvent(QMouseEvent *event)
{
 //   qDebug() << "JCalList::mouseMoveEvent()";

    if (event->buttons() & Qt::LeftButton) {
        if ((event->pos() - m_movePos).manhattanLength() \
                >= QApplication::startDragDistance()) {
            performDrag();
        }
    }
    QListWidget::mouseMoveEvent(event);
}

void JCalList::contextMenuEvent(QContextMenuEvent *event)
{
    emit toShowContextMenu(event->globalPos());
}

void JCalList::performDrag()
{
   // qDebug() << "JCalList::performDrag()";

    QListWidgetItem *item = currentItem();
    if (!item)
        return;

  //  qDebug() << "item: " << item->text();

    if (item && item->data(ITEM_ID).toInt() != NODATA) {
        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << item->data(ITEM_ID).toInt()\
                   << item->data(ITEM_CATEGORY).toInt()\
                   << m_day;

        QMimeData *mimeData = new QMimeData;
        mimeData->setText(item->text());
        mimeData->setData(MIME_FORMAT, itemData);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(QPixmap(":/images/person.png"));
        if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
            delete item;
    }    
}

void JCalList::dragEnterEvent(QDragEnterEvent *event)
{
    JCalList *source = qobject_cast<JCalList *>(event->source());

    if (source && source != this) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void JCalList::dragMoveEvent(QDragMoveEvent *event)
{
    JCalList *source = qobject_cast<JCalList *>(event->source());

    if (source && source != this) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void JCalList::dropEvent(QDropEvent *event)
{
 //   qDebug() << "JCalList::dropEvent()";

    JCalList *source = qobject_cast<JCalList *>(event->source());

    if (source && source != this) {
        if (event->mimeData()->hasFormat(MIME_FORMAT)) {
            const QMimeData *mime = event->mimeData();
            QByteArray itemData = mime->data(MIME_FORMAT);
            QDataStream dataStream(&itemData, QIODevice::ReadOnly);

            event->acceptProposedAction();

            int id, category, oldDay;
            dataStream >> id >> category >> oldDay;
            int newDay = itemAt(0,0)->text().toInt();

         //   qDebug() << "received data: " << id << category << oldDay << newDay;

            QListWidgetItem *item = new QListWidgetItem(event->mimeData()->text());
            item->setData(ITEM_ID, id);
            item->setData(ITEM_CATEGORY, category);
            addItem(item);

            // update db
            emit toMove(id, oldDay, newDay);
        }
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void JCalList::keyPressEvent (QKeyEvent* event)
{
  //  qDebug() << "keyPressEvent() " << event;

    switch (event->key()) {
        case Qt::Key_N:
            emit toNew();
            break;
        case Qt::Key_C:
            emit toCopy();
            break;
        case Qt::Key_X:
            emit toCut();
            break;
        case Qt::Key_V:
            emit toPaste();
            break;
        case Qt::Key_Delete:
        case Qt::Key_Backspace:  // for the MAC
            emit toDelete();
            break;
    }
}


// SLOTs

void JCalList::onSelectList(int id, CalendarItem citem)
{
 //   qDebug() << "JCalList::onSelectList()" << id << ctime;

 //   message(DEBUG, "JCalList", QString("onSelectCategory: %1").arg(id));

    if (id <= 0)
        return;

    for (int i=1; i<count(); i++) { // row=0 is day.
        if (item(i)->data(citem).toInt() == id)
            item(i)->setBackground(QBrush(QColor(COLOR_SELECTED)));
        else
             item(i)->setBackground(QBrush(QColor(Qt::white)));
    }
}

void JCalList::onUpdateList(int id, int day, QStringList data)
{
//    qDebug() << "JCalList::onUpdateData()" << id << day << data;

    int foundIndex = 0;
    if (item(0)->text().toInt() == day) {  // first item is day
        for (int i=1; i<count(); i++) {
            if (item(i)->data(ITEM_ID).toInt() == id) {
                if (data.join("").isEmpty()) { // remove
                 //   qDebug() << "remove item: " << i;
                    takeItem(i);
                    return;
                }
                foundIndex = i;
         //       qDebug() << "found " << foundIndex;
                break;
            }
        }

        if (foundIndex == 0) {
            foundIndex = count();
            addItem(new QListWidgetItem());
        }
  //      qDebug() << "foundIndex = " << foundIndex << data;
        item(foundIndex)->setText(itemTitle(data));
        item(foundIndex)->setData(ITEM_ID, data.at(CID));
        item(foundIndex)->setData(ITEM_CATEGORY, data.at(CATEGORY_ID));
    }  
}

