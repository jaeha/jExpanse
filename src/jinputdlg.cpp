#include "jinputdlg.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>

JInputDlg::JInputDlg(const QString &msg, FieldType type, QStringList fields, QWidget *parent)
    : QDialog(parent)
{
 //   setWindowTitle("Input");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setModal(true);

    QLabel *label = new QLabel(msg);

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(label);

    switch (type) {
        case COMBO: {
            m_combo = new QComboBox();
            m_combo->addItems(fields);
            hbox->addWidget(m_combo);
            break;
            }
        case LINE: {
            m_line = new QLineEdit();
            hbox->addWidget(m_line);

            break;
            }
        default:
            message(J_ERROR, "JInputDlg", "This is not expected data type!");
    }

    QPushButton *btOk = new QPushButton("Ok");
    QPushButton *btCancel = new QPushButton("Cancel");
    connect(btOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btCancel, SIGNAL(clicked()), this, SLOT(reject()));

    QHBoxLayout *bbox = new QHBoxLayout();
    bbox->addStretch();
    bbox->addWidget(btOk);
    bbox->addWidget(btCancel);

    QVBoxLayout *vbox= new QVBoxLayout();
    vbox->addLayout(hbox);
    vbox->addLayout(bbox);

    setLayout(vbox);
}

QString JInputDlg::result()
{
    return (m_combo) ? m_combo->currentText() : m_line->text();
}
