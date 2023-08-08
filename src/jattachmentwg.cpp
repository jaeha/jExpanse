#include "common.h"
#include "jattachmentwg.h"
#include "trans.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QHBoxLayout>
#include <QLabel>
#include <QDesktopServices>

JAttachmentWg::JAttachmentWg(QDate date, QString attachment, QWidget *parent) : QWidget(parent)
{
    m_date = date;
    // make sure attach dir exist
    if (!QDir(ATTACHPATH).exists())
        QDir().mkdir(ATTACHPATH);

    m_fileBt = new QPushButton();
    if (!attachment.isEmpty())
        m_fileBt->setText(attachment);
    else
        m_fileBt->setEnabled(false);
    connect(m_fileBt, SIGNAL(clicked()), this, SLOT(onClickedFileBt()));

    m_folderBt = new QPushButton();
    m_folderBt->setIcon(QIcon(":folder"));
    m_folderBt->setIconSize(ICON_SIZE);
    m_folderBt->setFixedSize(ICON_SIZE);
    m_folderBt->setFlat(true);
    if (!attachment.isEmpty())
        m_folderBt->setEnabled(false);
    connect(m_folderBt, SIGNAL(clicked()), this, SLOT(onClickedFolderBt()));

    m_deleteBt = new QPushButton();
    m_deleteBt->setIcon(QIcon(":delete"));
    m_deleteBt->setIconSize(ICON_SIZE);
    m_deleteBt->setFixedSize(ICON_SIZE);
    m_deleteBt->setFlat(true);
    if (attachment.isEmpty())
        m_deleteBt->setEnabled(false);
    connect(m_deleteBt, SIGNAL(clicked()), this, SLOT(onClickedDeleteBt()));

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(m_folderBt);
    hbox->addWidget(m_fileBt);
    hbox->setStretch(2,2);
    hbox->addWidget(m_deleteBt);
    hbox->setMargin(0);

    setLayout(hbox);
}

// SLOT

void JAttachmentWg::onClickedFolderBt()
{
 //   qDebug() <<"JAttachmentWg::onClickedFolderBt()";

    QString sfname = QFileDialog::getOpenFileName(this, tr("Attach File"), \
                                                QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    if (!sfname.isEmpty()) {
        QFileInfo fi(sfname);
        QString tfname = m_date.toString(DATE_YMD) + "_" + fi.fileName();
//        qDebug() <<sfname <<tfname;
        if (QFile::copy(sfname, ATTACHPATH + tfname)) {
            m_fileBt->setText(tfname);
            m_fileBt->setEnabled(true);
            m_deleteBt->setEnabled(true);
            m_folderBt->setEnabled(false);
            emit toUpdateAttachment(tfname);
        }
        else
            message(J_ERROR, "Attachmets", QString("Copy %1 was failed!").arg(sfname));
    }
}

void JAttachmentWg::onClickedFileBt()
{
 //   qDebug() <<"JAttachmentWg::onClickedFileBt()";
    QDesktopServices::openUrl(QUrl("file:///" + ATTACHPATH + m_fileBt->text(), QUrl::TolerantMode));
}

void JAttachmentWg::onClickedDeleteBt()
{
    if (!confirm(this, "Do you really want to remove this attachment?"))
        return;

    if (QFile::remove(ATTACHPATH + m_fileBt->text())) {
        m_fileBt->setText("");
        m_fileBt->setEnabled(false);
        m_folderBt->setEnabled(true);
        m_deleteBt->setEnabled(false);
        emit toUpdateAttachment("");
    } else
        message(J_ERROR, "Attachment", "Failed to delete the attached file.");


  //  qDebug() << "delete attached file!" << ATTACHPATH + m_fileBt->text();
}
