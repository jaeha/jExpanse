#ifndef JDBTABLEDLG_H
#define JDBTABLEDLG_H

#include "common.h"
#include "jdb.h"
#include <QDialog>

class JDBTableDlg : public QDialog
{
    Q_OBJECT

public:
    explicit JDBTableDlg(QString table, QString ptable, QList<Field> fields, bool needDone = true, QWidget *parent = 0);

private:
    void gui();
    void refresh();
    void closeEvent(QCloseEvent *event);

    QString m_table, m_ptable;
    int m_fieldsCount;

signals:
    void toAddList(int id, QString desc);
    void toUpdateList(QString desc);
    void toDeleteList();

private slots:
    void onClickAdd();
    void onClickDel();
    void onClickDone();
    void onDeleteDB(int id);

};

class JDBField : public QWidget
{
    Q_OBJECT

public:
    explicit JDBField(QString table, Field field, QWidget *parent = 0);

private:
    QString m_table;
    Field m_field;
    int m_selectedID;

signals:
    void toUpdateField(QString);
    void toUpdateList(QString);

public slots:
    void onListChanged(int id, QString data);
    void onUpdateDB(QString);
};

#endif // JDBTABLEDLG_H
