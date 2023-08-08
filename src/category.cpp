#include "category.h"

Category::Category() : JDB("category")
{
    if (!tableExist()) {
        createTable("desc varchar(128), categorytype varchar(32), budget double, note text");
        add("Grocery", "Expense", 0, "");
        add("Salary", "Income", 0, "");
        add("Saving", "Transfer", 0, "");
    }

}

int Category::add(QString desc, QString categorytype, double budget, QString note)
{
    return insert(QStringList() <<desc <<categorytype <<D2S(budget) << note);
}

QList<Field> Category::fields()
{
    QList<Field> fields;

    QStringList names = {"desc", "categorytype", "budget", "note"};
    QStringList descs = {"Description", "Category Type", "Budget", "Note"};
    QList<FieldType> types = {LINE, COMBO, LINE, TEXT};
    QStringList foreigns = {"", "categorytype", "", ""};

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
 * CategoryType
 * ***********/
CategoryType::CategoryType() : JDB("categorytype")
{
    if (!tableExist()) {
        if (createTable("desc varchar(64)")) {
            add("Expense");
            add("Income");
            add("Trnasfer");
        };
    };
}

int CategoryType::add(QString desc)
{
    return insert(QStringList() << desc);
}
