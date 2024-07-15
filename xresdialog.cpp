#include "xresdialog.h"
#include "ui_xresdialog.h"

XResDialog::XResDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XResDialog)
{
    ui->setupUi(this);
}

XResDialog::~XResDialog()
{
    delete ui;
}

void XResDialog::setResList(QStringList lstRes) {
    if (lstRes.count() % 2 != 0) {
        return;
    }
    m_lstRes.clear();
    for (int i = 0; i < lstRes.count(); i += 2) {
        QRadioButton* pRadio = new QRadioButton(lstRes[i], this);
        pRadio->setStyleSheet(QString("QRadioButton { font-size: 14pt }"));
        pRadio->move(30, i * 30);
        connect(pRadio, SIGNAL(released()), this, SLOT(radioSelected()));
        QLabel* pLabel = new QLabel(lstRes[i + 1], this);
        pLabel->setStyleSheet(QString("QLabel { font-size: 14pt }"));
        pLabel->move(30, (i + 1) * 30);
        pRadio->setChecked(true);
    }
}

void XResDialog::radioSelected() {
    QRadioButton* pRadio = (QRadioButton*)sender();
    emit pairSelect(pRadio->y() / 60);
}
