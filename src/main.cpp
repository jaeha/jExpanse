//#include "jmoneycalendar.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();

    //set stylesheet
  //  app.setFont(FONT_APP);
    QString style = readSettings("ApplicationStyleSheet").toString();
    if (style.isEmpty()) {
        style = APP_STYLE;
        writeSettings("ApplicationStyleSheet", style);
    }
    qDebug() << "Font:" << style;
    app.setStyleSheet(style);

    return app.exec();
}
