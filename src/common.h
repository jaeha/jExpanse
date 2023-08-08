#ifndef COMMON_H
#define COMMON_H

#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QSettings>

#define COMPANY     "jLab"
#define APP_TITLE   "jExpense"
#define APP_VER     "1.0"
#define BUILD_DATE  "26 April 2020"

#define NODATA  -1
#define DOLLAR_FORMAT(amount)   QString("%L1").arg(amount, 0, 'f', 2)
//#define N2S(number)    QString("%1").arg(number, 0, 'f', 2)
#define N2S(n)  QString::number(n)
#define D2S(d)  QString::number(d,'f',2)

#define DATE_YMD      "yyyy-MM-dd"
#define DATE_YM       "yyyy-MM"
#define DATE_MONTHYEAR  "MMMM yyyy"
#define DATE_DMY      "d MMM yyyy"

#define MAIN_WINDOW_TITLE	APP_TTILE + APP_VER
#define MAIN_WINDOW_SIZE    QRect(500, 500, 2000, 1200)
#define DIALOG_SIZE         QRect(500, 500, 400, 300)
#define DIALOG_BIG_SIZE     QRect(500, 500, 800, 400)
#define ICON_SIZE           QSize(24, 24)
#define COLOR_TOTAL_BG      Qt::darkCyan
#define COLOR_TOTAL_FG      Qt::white
#define COLOR_TITLE_BG      Qt::darkBlue
#define COLOR_TITLE_FG      Qt::white

#define FONT_DAY    QFont("Helvetica [Cronyx]", 18)
#define FONT_APP    QFont("Helvetica [Cronyx]", 11)
//#define APP_STYLE   "QWidget {font-size:11pt 'Helvetica [Cronyx]';} QSplitter::handle{background:darkgray;}"
#define APP_STYLE   "QWidget {font-size:11pt 'Helvetica [Cronyx]';}"
#define INFO_STYLE  "QLabel {background-color:darkblue; color:white;}"

//#define PATHSP     QDir::separator()
#define PATHSP     "/"
#define BASEPATH   QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)\
                    + PATHSP + APP_TITLE
#define ATTACHPATH  BASEPATH + PATHSP + "attachments" + PATHSP
#define DBFILE      "jexpense.dat"
#define DBVERSION   100
typedef QMultiMap<int, QStringList> IdMMap;
typedef QMap<int, QString> IdMap;
typedef QMapIterator<int, QString> IdMapIterator;
enum FieldType { LINE, TEXT, COMBO };
struct Field{
    QString name;
    QString desc;
    FieldType type;
    QString foreign;
};

enum MsgLevel { J_ERROR, J_INFO };
inline void message(MsgLevel level, const QString &title, const QString &msg)
{
    switch (level) {
    case J_ERROR:
        QMessageBox::critical(0, title, msg, QMessageBox::Close);
        qDebug() <<"ERROR:" <<QString("ERROR: %1 %2").arg(title).arg(msg);
        break;
    case J_INFO:
        QMessageBox::information(0, title, msg);
        qDebug() <<"INFO:" <<QString("INFO: %1 %2").arg(title).arg(msg);
        break;
    }
};

inline bool confirm(QWidget *parent, QString msg)
{
    int button = QMessageBox::question(parent,"Question",
                                        msg, QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
    if (button == QMessageBox::Yes)
        return true;
    else
        return false;
}

inline void writeSettings(QString key, QVariant value)
{
    QSettings s(COMPANY, APP_TITLE);
    s.setValue(key, value);
}

inline QVariant readSettings(QString key)
{
    QSettings s(COMPANY, APP_TITLE);
    return s.value(key);
}

#endif // COMMON_H
