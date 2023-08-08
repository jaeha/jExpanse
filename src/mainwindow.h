#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "jcalendar.h"
#include <QMainWindow>
#include <QDate>
#include <QSplitter>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    void gui();
    void updateCalendarDate(QDate date);
    void migration();

    QDate m_date;
    QSplitter *m_rightsp, *m_leftsp;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void toUpdateCalendar(int id, int day, QStringList list);
    void toSelectMonthCalendar(QDate date, IdMMap data);
    void toUpdateTitle(QString title);
    void toUpdateInfo(QDate);
    void toSelectItemCalendar(int, CalendarItem);

private slots:
    void onEditSettings();
    void onEditCalendar(int id, QDate date, QPoint pos);
    void onUpdateCalendar(int id, QDate date);
    void onPasteCalendar(int id, QDate tdate, CalendarCopyMode copyMode);
    void onDeleteCalendar(int id);
    void onMoveCalendar(int id, QDate date);
    void onPreviousMonthCalendar();
    void onNextMonthCalendar();
    void onThisMonthCalendar();
    void onSearchSelected(int id, QDate date);
    void onInfoSelected(int);
};
#endif // MAINWINDOW_H
