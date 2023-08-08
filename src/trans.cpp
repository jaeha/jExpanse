#include "trans.h"
#include "payee.h"
#include "account.h"
#include "category.h"

Trans::Trans() : JDB("trans")
{
    m_data.amount = 0;

    if (!tableExist())
        createTable("trdate date, amount double, desc varchar(128),\
                        category_id int, payee_id int, account_id int, \
                        attachment varchar(256), tag varchar(32), note text");
}

bool Trans::fetch(int id)
{
    QList<QVariant> list = record(id);
//    qDebug() << "Trans::fetch() " << list;
    if (list.isEmpty())
        return false;

    m_data.id = id;
    m_data.trdate = list[TRDATE].toDate();
    m_data.amount = list[TAMOUNT].toDouble();
    m_data.desc = list[TDESC].toString();
    m_data.category_id = list[TCATEGORY_ID].toInt();
    m_data.payee_id = list[TPAYEE_ID].toInt();
    m_data.account_id = list[TACCOUNT_ID].toInt();
    m_data.attachment = list[TATTACHMENT].toString();
    m_data.tag = list [TTAG].toString();
    m_data.note = list[TNOTE].toString();

    return true;
}

int Trans::updateData(int id, QDate trdate, double amount, QString desc, \
                       int category_id, int payee_id, int account_id, \
                       QString attachment, QString tag, QString note)
{
    int tid = id;

    if (tid == NODATA) {
        tid = insert(QStringList() <<trdate.toString(DATE_YMD) <<D2S(amount) <<desc \
                      <<N2S(category_id) <<N2S(payee_id) <<N2S(account_id) <<attachment <<tag << note);
        if (tid == NODATA)
            message(J_ERROR, "Transaction Insert", "Failed to insert new transaction! " + lastError());
    } else {
        QString sql = QString("UPDATE trans SET trdate='%1', amount=%2, desc='%3', category_id=%4,\
                                                payee_id=%5, account_id=%6, attachment='%7', tag='%8',\
                                                note='%9', lastmodified=DATETIME('NOW') \
                                             WHERE id=%10").arg(trdate.toString(DATE_YMD)).arg(D2S(amount)).arg(desc)\
                                            .arg(category_id).arg(payee_id).arg(account_id).arg(attachment)\
                                            .arg(tag).arg(note).arg(id);
   //     qDebug() <<"Trans::updateData " <<sql;

        if (!execQuery(sql))
                message(J_ERROR, "Transaction Update", "Failed to update transaction! "+lastError());
    }
    return tid;
}

bool Trans::removeData(int id)
{
    return remove(id);
}

bool Trans::moveDate(int id, QDate date)
{
    return update(id, "trdate", date.toString(DATE_YMD));
}

// OUT: List of [day, id, category_id, desc, payee, amount]
IdMMap Trans::monthData(QDate date)
{
//     data id, catalog, payee, amount
/*    QString sql = QString("SELECT strftime('%d',trans.trdate),\
                                trans.id, trans.category_id, payee.desc, trans.amount \
                                FROM trans JOIN payee ON trans.payee_id = payee.id \
                                WHERE strftime('%Y-%m',trans.trdate) = '%1'")\
                                .arg(date.toString(DATE_YM));
*/
    QString sql = QString("SELECT strftime('%d',trdate), t.id, t.category_id, t.desc, p.desc, t.amount \
                            FROM trans t JOIN payee p ON t.payee_id = p.id \
                            WHERE strftime('%Y-%m',t.trdate) = '%1'").arg(date.toString(DATE_YM));

  //  qDebug() << sql;
  //  qDebug() << db.recordMMap(sql);

    return recordMMap(sql);
}

// OUT:  [id, category_id, desc, payee, amount]
QStringList Trans::itemData(int id)
{
    QString sql = QString("SELECT t.id, t.category_id, t.desc, p.desc, t.amount \
                                FROM trans t JOIN payee p ON t.payee_id = p.id \
                                WHERE t.id = %1").arg(id);
  //  qDebug() << "Trans::itemData()" << sql;
  //  qDebug() << "Trans::itemData()" << record(sql);

    return record(sql);
}

int Trans::copyData(int copyId, QDate newDate)
{
  //  qDebug() << "Trans::copyData()" << copyId << newDate;

    QList<QVariant> list = record(copyId);
    QStringList copyRec = QStringList() <<newDate.toString(DATE_YMD) <<list.at(TAMOUNT).toString()
                                        <<list.at(TDESC).toString() <<list.at(TCATEGORY_ID).toString()
                                        <<list.at(TPAYEE_ID).toString() <<list.at(TACCOUNT_ID).toString()
                                        <<list.at(TATTACHMENT).toString() <<list.at(TTAG).toString()
                                        <<list.at(TNOTE).toString();
   // qDebug() << "copyRec: " << copyRec;
    return insert(copyRec);
}

// List of <t.id, t.trdate, t.amount, t.desc, p.desc, c.desc, a.desc, t.note, c.categorytype>
QList<QStringList> Trans::search(QString keyword, QString field, QString dateFrom, QString dateTo)
{
//    qDebug() <<"Trans::search()" <<keyword <<field <<dateFrom <<dateTo;

    QString where;

    // area
    if (!field.compare("All"))
        where = QString(" (upper(t.desc) like upper('%%1%%') OR upper(p.desc) like upper('%%1%') \
                        OR upper(c.desc) like upper('%%1%') OR upper(t.note) like upper('%%1%')) ").arg(keyword);
    else
        where = QString(" upper(%1.desc) like upper('%%2%%') ").arg(field.toLower().at(0)).arg(keyword);

    // date condition
    if (!dateFrom.compare("--")) {
        if (!dateTo.compare("--"))
            where += QString("");
        else
            where += QString("AND t.trdate <= '%1'" ).arg(dateTo);
    } else {
        if (!dateTo.compare("--"))
            where += QString("AND t.trdate >= '%1'").arg(dateFrom);
        else
           where += QString("AND t.trdate BETWEEN '%1' AND '%2'").arg(dateFrom).arg(dateTo);
    }


    QString sql = QString("SELECT t.id, t.trdate, t.amount, t.desc, p.desc, c.desc, a.desc, t.note, c.categorytype \
                    FROM trans t \
                    LEFT JOIN payee p ON t.payee_id=p.id \
                    LEFT JOIN category c ON t.category_id=c.id \
                    LEFT JOIN account a ON t.account_id=a.id \
                    WHERE %1 ORDER BY t.trdate").arg(where);
  //  qDebug() << sql;

    return records(sql);
}

// List of <category id, desc, total>
QList<QStringList> Trans::categorySum(QString dateformat, QString date, QString categorytype)
{
 //   qDebug() << "Trans::categorySum()" << dateformat << date << categorytype;
    QString sql = QString("SELECT c.id, c.desc, sum(t.amount) \
                           FROM trans t\
                           JOIN category c ON t.category_id = c.id \
                           WHERE strftime('%1', t.trdate) = '%2' \
                                AND c.categorytype = '%3' \
                                GROUP BY c.id \
                                ORDER BY sum(t.amount) DESC").arg(dateformat).arg(date).arg(categorytype);
   //qDebug() << sql;
    return records(sql);
}

