#include "account.h"

Account::Account() : JDB("account")
{
    if (!tableExist()) {
        createTable("desc varchar(128), accounttype varchar(32), note text");
        add("Westpac", "Bank", "My Primary bank.");
    }
}

int Account::add(QString desc, QString accounttype, QString note)
{
    return insert(QStringList() <<desc <<accounttype << note);
}

QList<Field> Account::fields()
{
    QList<Field> fields;

    QStringList names = {"desc", "accounttype", "note"};
    QStringList descs = {"Description", "Account Type", "Note"};
    QList<FieldType> types = {LINE, COMBO, TEXT};
    QStringList foreigns = {"", "accounttype", ""};

    for (int i=0; i<names.count(); i++) {
        Field f;
        f.name = names[i];
        f.desc = descs[i];
        f.type = types[i];
        f.foreign = foreigns[i];
        fields.append(f);
    }
    return fields;
}

/*************
 * AccountType
 * ***********/
AccountType::AccountType() : JDB("accounttype")
{
    if (!tableExist()) {
        if (createTable("desc varchar(64)")) {
            add("Bank");
            add("Credit");
            add("Investment");
        };
    };
}

bool AccountType::add(QString desc)
{
    return insert(QStringList() << desc);
}
