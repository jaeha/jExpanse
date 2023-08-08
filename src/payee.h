#ifndef PAYEE_H
#define PAYEE_H

#include "jdb.h"

class Payee : JDB
{
public:
    Payee();
    QString desc() { return m_data.desc; };
    QString address() { return m_data.address; };
    QString contacts() { return m_data.contats; };
    QString note() { return m_data.note; };

    QStringList desclist() { return records("desc", ""); };
    int desc2id(QString desc) { return id(desc); };
    QString id2desc(int id) { return JDB::desc(id); };
    int count() { return JDB::count(); };

    int add(QString desc, QString address, QString contacts, QString note);
    QList<Field> fields();
    QString dbtable() { return "payee"; };

private:
    struct data {
        int id;
        QString desc;
        QString address;
        QString contats;
        QString note;
    } m_data;
};

#endif // PAYEE_H
