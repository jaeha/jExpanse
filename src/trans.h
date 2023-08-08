#ifndef TRANS_H
#define TRANS_H

#include <QObject>
#include "common.h"
#include "jdb.h"


enum TransData {TRDATE=0, TAMOUNT, TDESC, TCATEGORY_ID, TPAYEE_ID, TACCOUNT_ID, TATTACHMENT, TTAG, TNOTE};
enum SearchOut {STID=0, STDATE, STAMOUNT, STDESC, SPDESC, SCDESC, SADESC, STNOTE, SCCTYPE};
enum CategorySum {CSID=0, CSDESC, CSTOTAL};

class Trans : JDB
{
public:
    Trans();
    bool fetch(int id);
    int updateData(int id, QDate trdate, double amount, QString desc, \
               int category_id, int payee_id, int account_id, \
               QString attachment, QString tag, QString note);
    bool removeData(int id);
    bool moveDate(int id, QDate date);
    IdMMap monthData(QDate date);
    QStringList itemData(int id);
    int copyData(int copyId, QDate newDate);
    QList<QStringList> search(QString keyword, QString field, QString dateFrom, QString dateTo);
    QList<QStringList> categorySum(QString dateformat, QString date, QString categorytype);
    bool isAttachmentUsed(QString attachment);

    //data
    QDate trdate() { return m_data.trdate; };
    double amount() { return m_data.amount; };
    QString desc() { return m_data.desc; };
    int category_id() { return m_data.category_id; };
    int payee_id() {  return m_data.payee_id; };
    int account_id() { return m_data.account_id; };
    QString attachment() { return m_data.attachment; };
    QString tag() { return m_data.tag; };
    QString note() {return m_data.note; };

private:
    struct data {
        int id;
        QDate   trdate;
        double  amount;
        QString desc;
        int category_id;
        int payee_id;
        int account_id;
        QString attachment;
        QString tag;
        QString note;
    } m_data;
};

#endif // TRANS_H
