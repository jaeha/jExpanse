#include <QListWidget>
#include <QMap>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QListWidgetItem>
#include <QGroupBox>
#include "jdbtabledlg.h"
#include "jdb.h"
#include "jinputdlg.h"
#include "jlistwidget.h"
#include "jtextedit.h"

#define LIST_FIELD  "desc"

JDBTableDlg::JDBTableDlg(QString table, QString ptable, QList<Field> fields, bool needDone, QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    m_table = table;
    m_ptable = ptable; // parent table
    m_fieldsCount = fields.count();

    setGeometry(DIALOG_SIZE);
    restoreGeometry(readSettings("JT_Geometry").toByteArray());
    setModal(true);

    JDB db(m_table);

    // desc list
    JListWidget *descList = new JListWidget();
    descList->update(db.recordMap(LIST_FIELD));

    // other fields
    QGridLayout *grid = new QGridLayout();
    for (int i=0; i<fields.count(); i++) {
        JDBField *jf = new JDBField(table, fields.at(i));
        grid->addWidget(new QLabel(fields.at(i).desc), i, 0);
        grid->addWidget(jf, i, 1);

        connect(descList, SIGNAL(toListChanged(int, QString)), jf, SLOT(onListChanged(int, QString)));
        connect(jf, SIGNAL(toUpdateList(QString)), descList, SLOT(onUpdateCurrent(QString)));
    }
    grid->setRowStretch(fields.count(), 50);

    QFrame *frame = new QFrame();
    frame->setFrameShape(QFrame::Panel);
    frame->setFrameShadow(QFrame::Raised);
    frame->setLayout(grid);

    // buttons for add/remove
    QPushButton *addBn = new QPushButton(tr("Add"));
    QPushButton *delBn = new QPushButton(tr("Delete"));
    QPushButton *doneBn = new QPushButton(tr("Done"));
    connect(addBn, SIGNAL(clicked()), this, SLOT(onClickAdd()));
    connect(delBn, SIGNAL(clicked()), this, SLOT(onClickDel()));
    connect(doneBn, SIGNAL(clicked()), this, SLOT(onClickDone()));

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(addBn);
    buttonLayout->addWidget(delBn);
    buttonLayout->addStretch();

    if (needDone)
        buttonLayout->addWidget(doneBn);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(frame);
    rightLayout->addLayout(buttonLayout);

    QWidget *rightWg = new QWidget;
    rightWg->setLayout(rightLayout);

    QSplitter *split = new QSplitter(Qt::Horizontal);
    split->addWidget(descList);
    split->addWidget(rightWg);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(split);

    setLayout(mainLayout);

    // signals
    connect(this, SIGNAL(toAddList(int, QString)), descList, SLOT(onAdd(int, QString)));
    connect(this, SIGNAL(toDeleteList()), descList, SLOT(onDeleteCurrent()));
    connect(descList, SIGNAL(toDeleteDB(int)), this, SLOT(onDeleteDB(int)));

    // add one if the list is empty
    if (descList->count() == 0)
        onClickAdd();
}

/*
 SLOTS
*/
void JDBTableDlg::onClickAdd()
{
 //   qDebug() << "JDBTableDlg::onClickAdd()" << m_fieldsCount;

    JDB db(m_table);
    QStringList list;

    for (int i=0; i<m_fieldsCount; i++)
        list.append("");
    int id = db.insert(list);
    if (id == NODATA) {
        message(J_ERROR, "Add to DB", QString("Failed to add empty data. Error: %1").arg(db.lastError()));
        return;
    }

    emit toAddList(id, "");
}

void JDBTableDlg::onClickDone()
{
    done(0);
  //  emit toUpdateList()
}

void JDBTableDlg::onClickDel()
{
    emit toDeleteList();
}

void JDBTableDlg::onDeleteDB(int id)
{
    JDB db(m_table);
    JDB pdb(m_ptable);

    // check if the selected data is used.
    QString twhere = m_table + QString("_id=%1").arg(id);
    QString tcolumn = m_table+QString("_id");
    int usedRecords = pdb.count(twhere);

    if (usedRecords != 0) {
        QString delItem = db.desc(id);
        QStringList list = db.records(LIST_FIELD, "");
        list.removeOne(delItem);
        JInputDlg *input = new JInputDlg(QString("%1 used by %2 records. Where do you want to move?: ")\
                           .arg(delItem).arg(usedRecords), COMBO, list);
        input->setWindowTitle("Move");
        if (input->exec() == QDialog::Rejected)
            return;

        int newId = db.id(input->result());
        if (newId == NODATA) {
            message(J_ERROR, "JDBTableDlg", "Selected non-exist data in DB");
            return;
        }
        // update parent table with new id
        if (!pdb.update(tcolumn, QString("%1").arg(newId), twhere)) {
            message(J_ERROR, "JDBTableDlg", "Failed to update to new data");
            return;
        }
        //check agin if anything use removing data
        if (!pdb.records(tcolumn, twhere).isEmpty()) {
            message(J_ERROR, "JDBTableDlg", "Some data are still in use!");
            return;
        }
        message(J_INFO, "JDBTableDlg", QString("%1 records were moved from %2 to %3. ").arg(usedRecords) \
                    .arg(db.desc(id)).arg(db.desc(newId)));
    }

    // remove from db and list
    if (!db.remove(id)) {
        message(J_ERROR, "JDBTableDlg", QString("Failed to remove %1.").arg(db.desc(id)));
        return;
    }
}

void JDBTableDlg::closeEvent(QCloseEvent *event)
{
    writeSettings("JT_Geometry", saveGeometry());
}


//##########
// JDBField
//##########

JDBField::JDBField(QString table, Field field, QWidget *parent)
    : QWidget(parent)
{
    m_table = table;
    m_field = field;

    JDB fdb(field.foreign);
    QHBoxLayout *hbox = new QHBoxLayout();

    switch(field.type) {
        case LINE: {
            QLineEdit *edit = new QLineEdit;
            hbox->addWidget(edit);
            hbox->setSpacing(0);
            setLayout(hbox);
            connect(this, SIGNAL(toUpdateField(QString)), edit, SLOT(setText(QString)));
            connect(edit, SIGNAL(textEdited(QString)), this, SLOT(onUpdateDB(QString)));
            break;
            }
        case TEXT: {
            JTextEdit *text = new JTextEdit;
            hbox->addWidget(text);
            hbox->setSpacing(0);
            setLayout(hbox);
            connect(this, SIGNAL(toUpdateField(QString)), text, SLOT(setText(QString)));
            connect(text, SIGNAL(toTextChanged(QString)), this, SLOT(onUpdateDB(QString)));
            break;
            }
        case COMBO: {
            if (field.foreign.isEmpty()) {
                message(J_ERROR, "JDBField", "Need combo data!");
                return;
            }
            QComboBox *combo = new QComboBox;
            combo->addItem("");
            combo->addItems(fdb.records(LIST_FIELD, ""));
            hbox->addWidget(combo);
            hbox->setSpacing(0);
            setLayout(hbox);
            connect(this, SIGNAL(toUpdateField(QString)), combo, SLOT(setCurrentText(QString)));
            connect(combo, SIGNAL(currentIndexChanged(QString)), this, SLOT(onUpdateDB(QString)));
            break;
            }
        default :;
    }
}

void JDBField::onListChanged(int id, QString data)
{
    m_selectedID = id;

    JDB db(m_table);
//

    QString str = db.record(id, m_field.name).toString();
 /*  if (m_field.type == COMBO) {
        JDB fdb(m_field.foreign);
        str = fdb.desc(str.toInt());
        if (str.isEmpty())
            str = "None";
    }
    */

    if (m_field.name.compare(LIST_FIELD) == 0) {
        QLineEdit *edit = (QLineEdit*)this->layout()->itemAt(0)->widget();
        edit->setPlaceholderText("New");
        this->setFocus();
    }

    emit toUpdateField(str);
}

void JDBField::onUpdateDB(QString str)
{
 //   qDebug() << "JDBField::onUpdateDB(): " << str;

    if (m_selectedID == NODATA) {
        message(J_ERROR, "JDBField", "No selected id!!");
        return;
    }

    JDB db(m_table);

/*   if (m_field.type == COMBO) {
        JDB fdb(m_field.foreign);
        str = N2S(fdb.id(str));
    }
*/
    if (db.update(m_selectedID, m_field.name, str)) {
        if (m_field.name == LIST_FIELD)
            emit toUpdateList(str);
    }
    else
        message(J_ERROR, "JDBField", QString("Update db was failed. ERROR: %1").arg(db.lastError()));
}


