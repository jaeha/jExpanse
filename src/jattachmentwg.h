#ifndef JATTACHMENTWG_H
#define JATTACHMENTWG_H

#include <QWidget>
#include <QPushButton>
#include <QDate>

class JAttachmentWg : public QWidget
{
    Q_OBJECT
public:
    explicit JAttachmentWg(QDate date, QString attachment, QWidget *parent = 0);
    QString attachedFile() { return m_fileBt->text(); };

private:
    QPushButton *m_fileBt, *m_deleteBt, *m_folderBt;
    QDate m_date;

signals:
    void toUpdateAttachment(QString);
    void toDeleteAttachment(int);

public slots:
    void onClickedFolderBt();
    void onClickedFileBt();
    void onClickedDeleteBt();
};

#endif // JATTACHMENTWG_H
