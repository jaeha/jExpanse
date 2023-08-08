/*
 * JDB.cpp
 *
 *  Created on: Mar 09, 2016
 *      Author: jaeha
 *      version: 2.0
 */

#include "jdb.h"
#include <QDir>

JDB::JDB(QString table, QObject *parent) : QObject(parent)
{
    m_table = table;
    if(!open())
        qDebug() << "ERROR: DB was not opened!";
}

// create table if it's not already eixst
bool JDB::createTable(QString columns)
{
    QString sql = QString("CREATE TABLE %1 (id INTEGER primary key, %2, lastmodified DATE);").arg(m_table).arg(columns);
    return execQuery(sql);
}

bool JDB::tableExist()
{
    QSqlDatabase db = QSqlDatabase::database();
    return db.tables().contains(m_table);
}

bool JDB::open()
{
    QSqlDatabase db = QSqlDatabase::database(); // get current db
    if (db.isOpen())
            return true;

    //check dbpath
    QString basePath = readSettings("BasePath").toString();
    if (basePath.isEmpty()) {
        basePath = BASEPATH;
        writeSettings("BasePath", BASEPATH);
    }

    if (!QDir(basePath).exists())
        QDir().mkdir(basePath);

    QString dbfile = basePath + PATHSP +  DBFILE;

    // backup last db
    QFile f(dbfile);
    if (f.exists())
        f.copy(dbfile + ".backup");

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbfile);
    qDebug() <<"JDB::open()" << dbfile;

   if (!db.open()) {
        m_lastError = QString("open: failed open %1 due to %2").arg(dbfile).arg(db.lastError().text());
        message(J_ERROR, "JDB", m_lastError);
        return false;
    }

    // create info db
    if (execQuery("CREATE TABLE info (db_version int, db_created date);"))
        if (!execQuery(QString("INSERT INTO info VALUES (%1, DATETIME('NOW'))").arg(DBVERSION))) {
            message(J_ERROR, "JDB", QString("Failed to create info db.  ERROR:%1").arg(m_lastError));
            return false;
        }

    return true;
}

bool JDB::execQuery(QString sql)
{
    QSqlQuery query;

    if (!query.exec(sql)) {
        m_lastError = QString("execQuery: Failed to execute '%1' with error, %2").arg(sql).arg(query.lastError().text());
        return false;
    }

    return true;
}

int JDB::insert(QStringList list)
{
    int lastID = NODATA;
    QSqlQuery query;
    QString smt = QString(tr("INSERT INTO %1 VALUES(NULL, %2 DATETIME('NOW'));").arg(m_table).arg(QString("?,").repeated(list.size())));

    query.prepare(smt);
    for (int i=0; i<list.size(); i++) {
            query.addBindValue(list[i]);
    }

    if (query.exec())
        lastID = query.lastInsertId().toInt();
    else
        m_lastError = "insert: " + query.lastError().text();

    query.finish();
    return lastID;
}

bool JDB::remove(int id)
{
    QString query = QString("DELETE FROM %1 WHERE id = %2").arg(m_table).arg(id);
    return execQuery(query);
}

bool JDB::update(QString column, QVariant value, QString where)
{
 //   qDebug() << "JDB::update()" <<column <<value <<where;

    QSqlQuery query;
    QString sql = QString("UPDATE %1 SET %2 = :value, lastmodified = DATETIME('NOW') WHERE %3").arg(m_table).arg(column).arg(where);

  //  qDebug() <<sql;

    query.prepare(sql);
    query.bindValue(":value", value);

    if (!query.exec()) {
        m_lastError =  QString("update: Failed to update table(%1), %2").arg(m_table).arg(query.lastError().text());
        return false;
    }

    query.finish();
    return true;
}

// getting all the columns for one record
QStringList JDB::record(QString sql)
{
    QSqlQuery query;
    QStringList list;

    query.exec(sql);
    query.first();

    for (int i=0; i < query.record().count(); i++ )  // all columes except id.
        list.append(query.value(i).toString());

    return list;
}

// all the columns for a id
QList<QVariant> JDB::record(int id)
{
    QSqlQuery query;
    QString sql = QString("SELECT * FROM %1 WHERE id=%2").arg(m_table).arg(id);
    QList<QVariant> list;

   // qDebug() << sql;

    query.exec(sql);
    query.first();

    for (int i=1; i < query.record().count(); i++ )  // all columes except id.
        list.append(query.value(i));

  //  qDebug() << "results: " << list;
    return list;
}

QVariant JDB::record(int id, QString column)
{
    QSqlQuery query;
    QString sql = QString("SELECT %1 FROM %2 WHERE id=%3").arg(column).arg(m_table).arg(id);

    query.exec(sql);
    query.first();
    if (query.isValid())
        return query.value(0);
    else
        return "";
}

// list of record for single column
QStringList JDB::records(QString column, QString where)
{
    QSqlQuery query;
    QStringList list;
    QString sql = QString("SELECT %1 FROM %2").arg(column).arg(m_table);
    if (!where.isEmpty())
            sql += QString(" WHERE %1").arg(where);
    sql += QString(" ORDER BY %1").arg(column);

    query.exec(sql);
    while (query.next() && query.isValid())
            list.append(query.value(0).toString());

    return list;
}

// getting all the columns for all records
QList<QStringList> JDB::records(QString sql)
{
    QSqlQuery query;
    QList<QStringList> list;

    query.exec(sql);
    while (query.next()) {
        QStringList sl;
        for (int i=0; i < query.record().count(); i++ )
            sl.append(query.value(i).toString());
        list.append(sl);
    }

    return list;
}

IdMap JDB::recordMap(QString column, QString where)
{
    QSqlQuery query;
    IdMap map;
    QString sql = QString("SELECT id,%1 FROM %2").arg(column).arg(m_table);
    if (!where.isEmpty())
            sql += QString(" WHERE %1").arg(where);
    sql += QString(" ORDER BY %1").arg(column);

    query.exec(sql);
    while (query.next() && query.isValid()) {
            int id = query.value(0).toInt();
            QString text = query.value(1).toString();
            map.insert(id, text);
    }
    return map;
}

int JDB::count (QString where)
{
    QSqlQuery query;
    QString sql = QString("SELECT count(*) FROM %1").arg(m_table);
    if (!where.isEmpty())
            sql += QString(" WHERE %1").arg(where);

    query.exec(sql);
    query.first();
    if (query.isValid())
        return query.value(0).toInt();
    else
        return NODATA;
}

int JDB::id(QString desc)
{
    QSqlQuery query;
    QString sql = QString("SELECT id FROM %1 where desc = '%2'").arg(m_table).arg(desc);

    query.exec(sql);
    query.first();
    if (query.isSelect())
        return query.value(0).toInt();
    else
        return NODATA;
}

bool JDB::updateLastModified(int id)
{
    if (id < 0)
        return false;

    QString sql = QString("UPDATE %1 SET lastmodified = DATETIME('NOW') WHERE id = %2").arg(m_table).arg(id);
    return execQuery(sql);
}

void JDB::close()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (db.isOpen())
        db.close();

    db.removeDatabase(db.connectionName());
}

JDB::~JDB()
{
}

// Calendar

IdMMap JDB::recordMMap(QString sql)
{
   // qDebug() << "JDB::recordMMap() " << sql;

    QSqlQuery query;
    IdMMap mmap;

    query.exec(sql);
    while (query.next()) {
        QStringList slist;
        int key = query.value(0).toInt();
        for (int i=1; i<query.record().count(); i++)
                slist.append(query.value(i).toString());
        mmap.insert(key, slist);
    }
 //   qDebug() << "JDB::recordMMap() " << mmap;
    return mmap;
}

void JDB::migrate_fc_mc(QString source, QString target)
{
    qDebug() << "DB::migrate_fc_mc()" << source << target;

    QSqlDatabase sdb = QSqlDatabase::addDatabase("QSQLITE", "sdb");
    sdb.setDatabaseName(source);
    sdb.open();

    QSqlDatabase tdb = QSqlDatabase::addDatabase("QSQLITE", "tdb");
    tdb.setDatabaseName(target);
    tdb.open();

    QSqlQuery query_sdb(sdb);
    QSqlQuery query_tdb(tdb);

    //Transactions, Change table name TRS to transaction and remove tag.
    query_sdb.prepare("select indate, amount, payee_id, category_id, account_id, note, lastmodified from transactions;");
    query_sdb.exec();
    while (query_sdb.next()) {
        query_tdb.prepare("INSERT INTO trans (trdate, amount, category_id, payee_id, account_id, note, lastmodified) \
                            VALUES(:trdate, :amount, :category_id, :payee_id, :account_id, :note, :lastmodified)");
        query_tdb.bindValue(":trdate", query_sdb.value(0).toDate());
        query_tdb.bindValue(":amount", query_sdb.value(1).toDouble());
        query_tdb.bindValue(":payee_id", query_sdb.value(2).toInt());
        query_tdb.bindValue(":category_id", query_sdb.value(3).toInt());
        query_tdb.bindValue(":account_id", query_sdb.value(4).toInt());
        query_tdb.bindValue(":note", query_sdb.value(5).toString());
        query_tdb.bindValue(":lastmodified", query_sdb.value(6).toDate());
        if (!query_tdb.exec())
            qDebug() << QString("unable to insert to transactions, %1").arg(query_tdb.lastError().text());
    }

    // Payee
    query_sdb.prepare("select id, desc, address, contacts, note, lastmodified from payee;");
    query_sdb.exec();
    while (query_sdb.next()) {
        query_tdb.prepare("INSERT INTO payee (id, desc, address, contacts, note, lastmodified) \
                                VALUES(:id, :desc, :address, :contacts, :note, :lastmodified)");
        query_tdb.bindValue(":id", query_sdb.value(0).toInt());
        query_tdb.bindValue(":desc", query_sdb.value(1).toString());
        query_tdb.bindValue(":address", query_sdb.value(2).toString());
        query_tdb.bindValue(":contacts", query_sdb.value(3).toString());
        query_tdb.bindValue(":note", query_sdb.value(4).toString());
        query_tdb.bindValue(":lastmodified", query_sdb.value(5).toDate());
        if (!query_tdb.exec())
            qDebug() << QString("unable to insert to payee, %1").arg(query_tdb.lastError().text());
    }

    // category
    query_sdb.prepare("select id, desc, categorytype_id, budget, lastmodified from category");
    query_sdb.exec();
    while (query_sdb.next()) {
        query_tdb.prepare("INSERT INTO category (id, desc, categorytype, budget, note, lastmodified) \
                                VALUES(:id, :desc, :categorytype, :budget, :note, :lastmodified)");
        query_tdb.bindValue(":id", query_sdb.value(0).toInt());
        query_tdb.bindValue(":desc", query_sdb.value(1).toString());
        query_tdb.bindValue(":categorytype", query_sdb.value(2).toString());
        query_tdb.bindValue(":budget", query_sdb.value(3).toDouble());
        query_tdb.bindValue(":note", "");
        query_tdb.bindValue(":lastmodified", query_sdb.value(4).toDate());
        if (!query_tdb.exec())
            qDebug() << QString("unable to insert to category, %1").arg(query_tdb.lastError().text());
    }

    // Account
    query_sdb.prepare("select id, desc, note, lastmodified from account");
    query_sdb.exec();
    while (query_sdb.next()) {
        query_tdb.prepare("INSERT INTO account (id, desc, accounttype, note, lastmodified) \
                                VALUES(:id, :desc, :accounttype, :note, :lastmodified)");
        query_tdb.bindValue(":id", query_sdb.value(0).toInt());
        query_tdb.bindValue(":desc", query_sdb.value(1).toString());
        query_tdb.bindValue(":accounttype", query_sdb.value(2).toString());
        query_tdb.bindValue(":note", query_sdb.value(2).toString());
        query_tdb.bindValue(":lastmodified", query_sdb.value(3).toDate());
        if (!query_tdb.exec())
            qDebug() << QString("unable to insert to account, %1").arg(query_tdb.lastError().text());
    }

    sdb.close();
    tdb.close();

   // QFile f(source);
   // f.rename(source.append("_done"));

    qDebug() << "Migration completed successfully!";
}
