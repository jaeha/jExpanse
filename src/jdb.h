/*
 * JDB.h
 *
 *  Created on: Mar 09, 2016
 *      Author: jaeha
 */

#ifndef JDB_H_
#define JDB_H_

/* desktop */
#include <QtCore>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QMultiMap>
#include "common.h"

/* BlackBerry
#include <qobject.h>
#include <QtCore>
#include <QtSql/QtSql>
#include <bb/data/SqlConnection>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/OrientationSupport>
#include <bb/system/SystemDialog>

using namespace bb::cascades;
*/

class JDB : public QObject
{
    Q_OBJECT

public:
    explicit JDB(QString tablename, QObject *parent = 0);
    virtual ~JDB();

    bool open();
    bool execQuery(QString sql);
    int insert(QStringList list);
    bool remove(int id);
    QStringList record(QString sql);
    QList<QVariant> record(int id);
    QVariant record(int id, QString column);
    QStringList records(QString column, QString where);
    QList<QStringList> records(QString sql);
    IdMMap recordMMap(QString sql);
    IdMap recordMap(QString column, QString where="");
    QString desc(int id) { return record(id, "desc").toString(); };
    int id(QString desc);
    bool update(QString column, QVariant value, QString where);
    bool update(int id, QString column, QString value) { return update(column, value, QString("id=%1").arg(id)); };
    int count(QString where="");
    QString lastError() { return m_lastError; };
    void close();
    int dbversion() { return record("select db_version from info").first().toInt(); };
    bool createTable(QString columns);
    bool tableExist();
    void migrate_fc_mc(QString source, QString target);

private:
    QString m_table;
    QString m_lastError;

    bool updateLastModified(int id);
};

#endif // JDB_H
