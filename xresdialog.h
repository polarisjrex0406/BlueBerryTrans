#ifndef XRESDIALOG_H
#define XRESDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QString>
#include <QStringList>
#include <QLabel>
#include <QDebug>

namespace Ui {
    class XResDialog;
}

class XResDialog : public QDialog
{
    Q_OBJECT

public:
    explicit XResDialog(QWidget *parent = 0);
    ~XResDialog();

    void setResList(QStringList);

public slots:
    void radioSelected();

signals:
    void pairSelect(int);

private:
    Ui::XResDialog *ui;

    QStringList m_lstRes;
};

#endif // XRESDIALOG_H
