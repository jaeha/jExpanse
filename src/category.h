#ifndef CATEGORY_H
#define CATEGORY_H

#include "jdb.h"

class Category : JDB
{
public:
    Category();
    QString desc() { return m_data.desc; };
    QString categorytype() { return m_data.categorytype; };
    double budget() { return m_data.budget; };
    QString note() { return m_data.note; };

    QStringList desclist() { return records("desc", ""); };
    int desc2id(QString desc) { return id(desc); };
    QString id2desc(int id) { return JDB::desc(id); };
    int count() { return JDB::count(); };

    int add(QString desc, QString categorytype, double budget, QString note);
    QList<Field> fields();
    QString dbtable() { return "category"; };

private:
    struct data {
        int id;
        QString desc;
        QString categorytype;
        double budget;
        QString note;
    } m_data;

};

class CategoryType : JDB
{
public:
    CategoryType();

    QString desc() { return m_data.desc; };
    int add(QString desc);
    IdMap desclist() { return recordMap("desc"); };

private:
    struct data {
        int id;
        QString desc;
    } m_data;

};

#endif // CATEGORY_H
