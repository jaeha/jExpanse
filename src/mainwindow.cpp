#include "mainwindow.h"
#include "common.h"
#include "transdlg.h"
#include "trans.h"
#include "settingsdlg.h"
#include "infotab.h"
#include "searchwg.h"
#include <QVBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle(QString("%1 v%2").arg(APP_TITLE).arg(APP_VER));

    m_date = QDate::currentDate();
    gui();
    updateCalendarDate(m_date);

   // QWidget::showMaximized();
    QList<int> size = {2000, 20};
    m_leftsp->setSizes(size);

    //settings
    restoreGeometry(readSettings("MW_Geometry").toByteArray());
    m_rightsp->restoreState(readSettings("MW_RightSplit").toByteArray());
    m_leftsp->restoreState(readSettings("MW_LeftSplit").toByteArray());
}

void MainWindow::gui()
{
    Trans trans;
    InfoTab *infoTab = new InfoTab;
    SearchWg *searchWg = new SearchWg(this);
    JCalendar *calendar = new JCalendar(m_date,trans.monthData(m_date));

    QLabel *dateLb = new QLabel(m_date.toString(DATE_MONTHYEAR));
    dateLb->setStyleSheet("font: 24pt");

    QPushButton *previousBt = new QPushButton();
    previousBt->setIcon(QIcon(":previous-month"));
    previousBt->setFlat(true);
    QPushButton *nextBt = new QPushButton();
    nextBt->setIcon(QIcon(":next-month"));
    nextBt->setFlat(true);

    QPushButton *todayBt = new QPushButton(tr("Today"));
    todayBt->setFlat(true);
    todayBt->setStyleSheet("QPushButton{font-size: 11pt}");
    //todayBt->setFixedSize(QSize(100,50));


    QPushButton *settingsBt = new QPushButton();
    settingsBt->setIcon(QIcon(":settings"));
    settingsBt->setFlat(true);

    QHBoxLayout *dlayout= new QHBoxLayout;
    dlayout->setSpacing(1);
    dlayout->addWidget(dateLb);
    dlayout->addStretch();
    dlayout->addWidget(previousBt);
    dlayout->addWidget(todayBt);
    dlayout->addWidget(nextBt);
    dlayout->addSpacing(10);
    dlayout->addWidget(settingsBt);

    QVBoxLayout *clayout = new QVBoxLayout;
    clayout->addLayout(dlayout);
    clayout->addWidget(calendar);
    QWidget *calendarWg = new QWidget();
    calendarWg->setLayout(clayout);

    m_rightsp = new QSplitter(Qt::Vertical);
    m_rightsp->addWidget(infoTab);
    m_rightsp->addWidget(searchWg);
  //  m_rightsp->setHandleWidth(3);
    m_rightsp->setCollapsible(0, false);
    m_rightsp->setCollapsible(1, false);

    m_leftsp = new QSplitter(Qt::Horizontal);
   // m_leftsp->setHandleWidth(3);
    m_leftsp->addWidget(calendarWg);
    m_leftsp->addWidget(m_rightsp);
    m_leftsp->setCollapsible(0, false);
    m_leftsp->setCollapsible(1, false);

    setCentralWidget(m_leftsp);

    // select month
    connect(previousBt, SIGNAL(clicked()), this, SLOT(onPreviousMonthCalendar()));
    connect(todayBt, SIGNAL(clicked()), this, SLOT(onThisMonthCalendar()));
    connect(nextBt, SIGNAL(clicked()), this, SLOT(onNextMonthCalendar()));
    connect(this, SIGNAL(toUpdateTitle(QString)), dateLb, SLOT(setText(QString)));
    //settings
    connect(settingsBt, SIGNAL(clicked()), this, SLOT(onEditSettings()));
    // Calendar --> this
    connect(calendar, SIGNAL(toEdit(int, QDate, QPoint)), this, SLOT(onEditCalendar(int, QDate, QPoint)));
    connect(calendar, SIGNAL(toMove(int, QDate)), this, SLOT(onMoveCalendar(int,QDate)));
    connect(calendar, SIGNAL(toPaste(int, QDate, CalendarCopyMode)), this, SLOT(onPasteCalendar(int, QDate, CalendarCopyMode)));
    connect(calendar, SIGNAL(toDelete(int)), this, SLOT(onDeleteCalendar(int)));
    // this --> Calendar
    connect(this, SIGNAL(toUpdateCalendar(int, int, QStringList)), calendar, SLOT(onUpdate(int, int, QStringList)));
    connect(this, SIGNAL(toSelectMonthCalendar(QDate, IdMMap)), calendar, SLOT(onSelectMonth(QDate, IdMMap)));
    // search
    connect(searchWg, SIGNAL(toSearchSelected(int, QDate)), this, SLOT(onSearchSelected(int, QDate)));
    connect(this, SIGNAL(toSelectItemCalendar(int, CalendarItem)), calendar, SLOT(onSelectItem(int, CalendarItem)));
    // info
    connect(this, SIGNAL(toUpdateInfo(QDate)), infoTab, SLOT(onUpdate(QDate)));
    connect(infoTab, SIGNAL(toInfoSelected(int)), this, SLOT(onInfoSelected(int)));
}

void MainWindow::updateCalendarDate(QDate date)
{
    Trans trans;
    m_date = date;
    emit toSelectMonthCalendar(m_date, trans.monthData(m_date));
    emit toUpdateTitle(m_date.toString(DATE_MONTHYEAR));
    emit toUpdateInfo(m_date);
}

void MainWindow::migration()
{
/*    Trans *trans = new Trans;
    Payee *payee = new Payee;
    Category *category = new Category;
    Account *account = new Account;
    CategoryType categorytype;
    AccountType accounttype;

    JDB db("trans");
    db.migrate_fc_mc(BASEPATH.append("fc.db"), BASEPATH.append("mc.db"));
    */
}

MainWindow::~MainWindow()
{
    writeSettings("MW_Geometry", saveGeometry());
    writeSettings("MW_RightSplit", m_rightsp->saveState());
    writeSettings("MW_LeftSplit", m_leftsp->saveState());
}

/* *******
 * SLOTS *
 *********/

void MainWindow::onEditSettings()
{
    SettingsDlg *dlg = new SettingsDlg();
    dlg->show();
}

void MainWindow::onEditCalendar(int id, QDate date, QPoint pos)
{
    TransDlg *dlg = new TransDlg(id, date);
    dlg->show();
    dlg->move(pos.x(), pos.y()+10);
    connect(dlg, SIGNAL(toUpdate(int, QDate)), this, SLOT(onUpdateCalendar(int, QDate)));
}

void MainWindow::onPasteCalendar(int id, QDate date, CalendarCopyMode copyMode)
{
    if (copyMode == NONE)
        return;

    Trans trans;
    int newId = trans.copyData(id, date);

    if (copyMode == CUT)
        trans.removeData(id);

    emit toUpdateCalendar(newId, date.toString("d").toInt(), trans.itemData(newId));
}

void MainWindow::onUpdateCalendar(int id, QDate date)
{
 //   qDebug() << "MainWindow::onUpdateCalendar() " << id << date;

    Trans trans;
    emit toUpdateCalendar(id, date.toString("d").toInt(), trans.itemData(id));
}

void MainWindow::onDeleteCalendar(int id)
{
    Trans trans;
    trans.removeData(id);
}

void MainWindow::onMoveCalendar(int id, QDate date)
{
 //   qDebug() << "MainWindow::onMoveCalendar() " << id << date;

    Trans trans;
    trans.moveDate(id, date);
}

void MainWindow::onPreviousMonthCalendar()
{
    updateCalendarDate(m_date.addMonths(-1));
}

void MainWindow::onNextMonthCalendar()
{
    updateCalendarDate(m_date.addMonths(1));
}

void MainWindow::onThisMonthCalendar()
{
    updateCalendarDate(QDate::currentDate());
}

void MainWindow::onSearchSelected(int id, QDate date)
{
    updateCalendarDate(date);

    emit toSelectItemCalendar(id, ITEM_ID);
}

void MainWindow::onInfoSelected(int id)
{
    emit toSelectItemCalendar(id, ITEM_CATEGORY);
}
