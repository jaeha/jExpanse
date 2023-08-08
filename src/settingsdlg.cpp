#include "settingsdlg.h"
#include "jcalendar.h"
#include "jdb.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QFileDialog>

SettingsDlg::SettingsDlg(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Settings");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    JDB db("info");
    QLabel *infoLb = new QLabel(QString(" %1 v%2, DB:%3, Build:%4").arg(APP_TITLE)\
                                .arg(APP_VER).arg(db.dbversion()).arg(BUILD_DATE));
    infoLb->setStyleSheet(INFO_STYLE);

    //BasePath
    m_basepathEd = new QLineEdit;
    m_basepathEd->setText(readSettings("BasePath").toString());
    m_basepathBt = new QPushButton();
    m_basepathBt->setIcon(QIcon(":folder"));
    m_basepathBt->setIconSize(ICON_SIZE);
    m_basepathBt->setFixedSize(ICON_SIZE);
    m_basepathBt->setFlat(true);

    QHBoxLayout *baseLayout = new QHBoxLayout;
    baseLayout->addWidget(m_basepathBt);
    baseLayout->addWidget(m_basepathEd);
    baseLayout->setStretch(2,2);
    baseLayout->setMargin(0);

    //CalendarDispItem
    QRadioButton *dispDescBt = new QRadioButton("Description");
    QRadioButton *dispPayeeBt = new QRadioButton("Payee");
    m_dispItem = readSettings("CalendarDispItem").toInt();
    if (m_dispItem == DESC)
        dispDescBt->setChecked(true);
    else
        dispPayeeBt->setChecked(true);

    QHBoxLayout *dispLayout = new QHBoxLayout();
    dispLayout->addWidget(dispDescBt);
    dispLayout->addWidget(dispPayeeBt);
    dispLayout->addStretch(1);

    // ApplicationStyleSheet
    m_appStyleTx = new QTextEdit();
    m_appStyleTx->setText(readSettings("ApplicationStyleSheet").toString());

    // save
    QPushButton *saveBt = new QPushButton("Save");
    QPushButton *cancelBt = new QPushButton("Cancel");
    QHBoxLayout *saveLayout= new QHBoxLayout();
    saveLayout->addStretch(1);
    saveLayout->addWidget(saveBt);
    saveLayout->addWidget(cancelBt);

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(new QLabel(tr("Base Directory:")), 0, 0);
    grid->addLayout(baseLayout, 0, 1);
    grid->addWidget(new QLabel(tr("Display Item:")), 1, 0);
    grid->addLayout(dispLayout, 1, 1);
    grid->addWidget(new QLabel(tr("StyleSheet:")), 2, 0);
    grid->addWidget(m_appStyleTx, 2, 1, 1, 1);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(infoLb);
    vbox->addLayout(grid);
    vbox->addLayout(saveLayout);

    setLayout(vbox);

    connect(m_basepathBt, SIGNAL(clicked()), this, SLOT(onClickedBasepathBt()));
    connect(dispDescBt, SIGNAL(clicked()), this, SLOT(onClickedDispDesc()));
    connect(dispPayeeBt, SIGNAL(clicked()), this, SLOT(onClickedDispPayee()));
    connect(saveBt, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(cancelBt, SIGNAL(clicked()), this, SLOT(close()));

    setGeometry(DIALOG_SIZE);
    restoreGeometry(readSettings("SD_Geometry").toByteArray());
}

// SLOT

void SettingsDlg::onSave()
{
    writeSettings("BasePath", m_basepathEd->text());
    writeSettings("CalendarDispItem", m_dispItem);
    writeSettings("ApplicationStyleSheet", m_appStyleTx->toPlainText());

    message(J_INFO, "Save Settings", "Successfully saved settings. BasePath and Application StyleSheet will be applied on next Startup.\
                      CalendarDispItem will be applied on next Refesh screen!");
    close();
}

void SettingsDlg::onClickedBasepathBt()
{
    QString basepath = QFileDialog::getExistingDirectory(this, tr("Select Base Directory"), \
                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if (!basepath.isEmpty()) {
          m_basepathEd->setText(basepath);
    }
}

void SettingsDlg::onClickedDispDesc()
{
    m_dispItem = DESC;
}

void SettingsDlg::onClickedDispPayee()
{
    m_dispItem = PAYEE;
}

void SettingsDlg::closeEvent(QCloseEvent *event)
{
    writeSettings("SD_Geometry", saveGeometry());
}
