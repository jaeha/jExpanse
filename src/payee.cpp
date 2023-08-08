#include "payee.h"

Payee::Payee() : JDB("payee")
{
    if (!tableExist()) {
        createTable("desc varchar(128), address varchar(256), contacts varchar(256),note text");
        add("Coles", "Sydney CBD", "", "");
    }
}

int Payee::add(QString desc, QString address, QString contacts, QString note)
{
    return insert(QStringList() << desc << address << contacts << note);
}

QList<Field> Payee::fields()
{
    QList<Field> fields;

    QStringList names = {"desc", "address", "contacts", "note"};
    QStringList descs = {"Description", "Address", "Contacts", "Note"};
    QList<FieldType> types = {LINE, LINE, LINE, TEXT};

    for (int i=0; i<names.count(); i++) {
        Field f;
        f.name = names[i];
        f.desc = descs[i];
        f.type = types[i];
        fields.append(f);
    }
    return fields;
}
