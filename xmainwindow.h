#ifndef XMAINWINDOW_H
#define XMAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QPainter>
#include <QListWidget>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QRect>
#include <QBrush>
#include <QPen>
#include <QRegion>
#include <QDebug>
#include <QColor>
#include <math.h>
#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QDataStream>
#include <QInputDialog>
#include <QDir>
#include "xresdialog.h"

namespace Ui {
    class XMainWindow;
}

#define NODE_FARM 0
#define NODE_SHOP 1
#define NODE_MID 2

#define EDIT_FARM 0
#define EDIT_SHOP 1
#define EDIT_MID 2
#define EDIT_PATH 3
#define EDIT_SOURCE 4
#define EDIT_ENDPOINT 5

#define MAX_NODE_COUNT 50

#define LINE_WIDTH 3
#define RECT_SIZE 25
#define FONT_SIZE 16

#define RATIO_MILE 6.4

#define MIN_MACRO(a, b) ((a) < (b) ? (a) : (b))

struct XNode {
    QPoint ptPos;
    QString strTownName;
    uint uType;
};

class XMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit XMainWindow(QWidget *parent = 0);
    ~XMainWindow();

    void Dijsktra();
    void printPath(int [], int);

private:
    Ui::XMainWindow *ui;

    QList<XNode> m_lstNode;
    QList<int> m_lstFarm, m_lstShop;
    uint m_uStatus;
    bool m_bDrawLine;
    QLine m_lineTemp;
    int m_dblWeight[MAX_NODE_COUNT][MAX_NODE_COUNT];
    int m_nLineBeg, m_nLineEnd;
    int m_nSource, m_nEndPoint;
    QList<int> m_lstShortPath;

    XResDialog* m_pResDlg;

private slots:
    void on_actionLoad_triggered();
    void on_actionShow_triggered();
    void on_actionSave_triggered();
    void on_actionStart_triggered();
    void on_actionPath_toggled(bool );
    void on_actionMid_toggled(bool );
    void on_actionShop_toggled(bool );
    void on_actionFarm_toggled(bool );

public slots:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    void paintEvent(QPaintEvent *);

    void showPair(int);
};

#endif // XMAINWINDOW_H
