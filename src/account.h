#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "jdb.h"

class Account : JDB
{
public:
    Account();
    QString desc() { return m_data.desc; };
    QString accounttype() { return m_data.accounttype; };
    QString note() { return m_data.note; };

    QStringList desclist() { return records("desc", ""); };
    int desc2id(QString desc) { return id(desc); };
    QString id2desc(int id) { return JDB::desc(id); };
    int count() { return JDB::count(); };

    int add(QString desc, QString accounttype, QString note);
    QList<Field> fields();
    QString dbtable() { return "account"; };


private:
    struct data {
        int id;
        QString desc;
        QString accounttype;
        QString note;
    } m_data;

};

class AccountType : JDB
{
public:
    AccountType();

    QString desc() { return m_data.desc; };
    bool add(QString desc);
    IdMap desclist() { return recordMap("desc"); };

private:
    struct data {
        int id;
        QString desc;
    } m_data;

};

#endif // ACCOUNT_H
