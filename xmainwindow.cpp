#include "xmainwindow.h"
#include "ui_xmainwindow.h"

XMainWindow::XMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::XMainWindow)
{
    ui->setupUi(this);

    m_uStatus = -1;
    m_bDrawLine = false;
    m_nSource = -1;
    m_nEndPoint = -1;
    m_pResDlg = NULL;

    for (int i = 0; i < MAX_NODE_COUNT; i ++) {
        for (int j = 0; j < MAX_NODE_COUNT; j ++) {
            m_dblWeight[i][j] = 0;
        }
    }
}

XMainWindow::~XMainWindow()
{
    delete ui;
}

void XMainWindow::on_actionFarm_toggled(bool bToggled)
{
    if (bToggled) {
        m_uStatus = EDIT_FARM;
        ui->actionShop->setChecked(false);
        ui->actionMid->setChecked(false);
        ui->actionPath->setChecked(false);
    }
}

void XMainWindow::on_actionShop_toggled(bool bToggled)
{
    if (bToggled) {
        m_uStatus = EDIT_SHOP;
        ui->actionFarm->setChecked(false);
        ui->actionMid->setChecked(false);
        ui->actionPath->setChecked(false);
    }
}

void XMainWindow::on_actionMid_toggled(bool bToggled)
{
    if (bToggled) {
        m_uStatus = EDIT_MID;
        ui->actionFarm->setChecked(false);
        ui->actionShop->setChecked(false);
        ui->actionPath->setChecked(false);
    }
}

void XMainWindow::on_actionPath_toggled(bool bToggled)
{
    if (bToggled) {
        m_uStatus = EDIT_PATH;
        ui->actionFarm->setChecked(false);
        ui->actionShop->setChecked(false);
        ui->actionMid->setChecked(false);
    }
}

void XMainWindow::mousePressEvent(QMouseEvent *me) {
    if (EDIT_PATH == m_uStatus) {
        m_nLineBeg = -1;
        m_nLineEnd = -1;
        foreach (XNode node, m_lstNode) {
            m_nLineBeg ++;
            QRegion reg(node.ptPos.x() - RECT_SIZE / 2, node.ptPos.y() - RECT_SIZE / 2, RECT_SIZE, RECT_SIZE);
            if (node.uType == NODE_MID) {
                reg = QRegion(node.ptPos.x() - RECT_SIZE / 2, node.ptPos.y() - RECT_SIZE / 2, RECT_SIZE, RECT_SIZE, QRegion::Ellipse);
            }
            if (reg.contains(me->pos())) {
                m_bDrawLine = true;
                m_lineTemp.setP1(node.ptPos);
                break;
            }
        }
    }
}

void XMainWindow::mouseMoveEvent(QMouseEvent *me) {
    if (EDIT_PATH == m_uStatus && m_bDrawLine) {
        m_lineTemp.setP2(me->pos());
        update();
    }
}

void XMainWindow::mouseReleaseEvent(QMouseEvent *me) {
    if (m_uStatus < EDIT_PATH) {
        XNode node;
        node.ptPos = me->pos();
        node.uType = m_uStatus;
        // Highlight current town
        // Get town name
        bool ok;
        QString strTownName = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                             tr("Town name:"), QLineEdit::Normal,
                                             QString(), &ok);
        if (!ok || strTownName.isEmpty())
            return;
        node.strTownName = strTownName;
        if (NODE_FARM == node.uType)
            m_lstFarm.append(m_lstNode.count());
        else if (NODE_SHOP == node.uType)
            m_lstShop.append(m_lstNode.count());
        m_lstNode.append(node);
        update();
    }
    else if (m_uStatus == EDIT_PATH){
        m_bDrawLine = false;
        foreach (XNode node, m_lstNode) {
            m_nLineEnd ++;
            QRegion reg(node.ptPos.x() - RECT_SIZE / 2, node.ptPos.y() - RECT_SIZE / 2, RECT_SIZE, RECT_SIZE);
            if (node.uType == NODE_MID) {
                reg = QRegion(node.ptPos.x() - RECT_SIZE / 2, node.ptPos.y() - RECT_SIZE / 2, RECT_SIZE, RECT_SIZE, QRegion::Ellipse);
            }
            if (reg.contains(me->pos()) && node.ptPos != m_lineTemp.p1()) {
                double dblDist = sqrt(pow(node.ptPos.x() - m_lineTemp.p1().x(), 2) + pow(node.ptPos.y() - m_lineTemp.p1().y(), 2));
                qDebug() << m_nLineBeg << "---" << dblDist << "---" << m_nLineEnd;
                m_dblWeight[m_nLineBeg][m_nLineEnd] = dblDist;
                m_dblWeight[m_nLineEnd][m_nLineBeg] = dblDist;
                break;
            }
        }
        update();
    }
}

void XMainWindow::paintEvent(QPaintEvent *pe) {
    QPainter paint(this);

    QPixmap pix("://img/bkg.jpg");
    paint.drawPixmap(0, 0, width(), height(), pix, 0, 0, pix.width(), pix.height());

    for (int i = 0; i < m_lstNode.count(); i ++) {
        for (int j = 0; j < m_lstNode.count(); j ++) {
            if (m_dblWeight[i][j] > 0) {
                // Normal path
                QPen penTmp = paint.pen();
                int nLineWidth = penTmp.width();
                penTmp.setWidth(LINE_WIDTH);
                penTmp.setStyle(Qt::DotLine);
                paint.setPen(penTmp);
                paint.drawLine(m_lstNode[i].ptPos, m_lstNode[j].ptPos);
                penTmp.setWidth(nLineWidth);
                penTmp.setStyle(Qt::SolidLine);
                paint.setPen(penTmp);

                QPoint ptText = (m_lstNode[i].ptPos + m_lstNode[j].ptPos) / 2;
                paint.setBackgroundMode(Qt::OpaqueMode);
                QFont font = paint.font();
                int nOldFontSize = font.pointSize();
                font.setPointSize(FONT_SIZE - 6);
                paint.setFont(font);
                paint.drawText(ptText, QString::number(m_dblWeight[i][j] / RATIO_MILE, 'f', 0) + "mile");
                font.setPointSize(nOldFontSize);
                paint.setFont(font);
                paint.setBackgroundMode(Qt::TransparentMode);
            }
        }
    }

    if (m_bDrawLine) {
        QPen penTmp = paint.pen();
        int nLineWidth = penTmp.width();
        penTmp.setWidth(LINE_WIDTH);
        penTmp.setStyle(Qt::DotLine);
        paint.setPen(penTmp);
        paint.drawLine(m_lineTemp);
        penTmp.setWidth(nLineWidth);
        penTmp.setStyle(Qt::SolidLine);
        paint.setPen(penTmp);
    }

    // Shortest path
    int prev = m_nSource;
    foreach (int next, m_lstShortPath) {
        QPen penTmp = paint.pen();
        int nLineWidth = penTmp.width();
        penTmp.setWidth(LINE_WIDTH);
        penTmp.setColor(Qt::red);
        paint.setPen(penTmp);
        paint.drawLine(m_lstNode[prev].ptPos, m_lstNode[next].ptPos);
        penTmp.setWidth(nLineWidth);
        paint.setPen(penTmp);

        QPoint ptText = (m_lstNode[prev].ptPos + m_lstNode[next].ptPos) / 2;
        paint.setBackgroundMode(Qt::OpaqueMode);
        QFont font = paint.font();
        int nOldFontSize = font.pointSize();
        font.setPointSize(FONT_SIZE - 6);
        paint.setFont(font);
        if (prev != next)
            paint.drawText(ptText, QString::number(m_dblWeight[prev][next] / RATIO_MILE, 'f', 0) + "mile");
        font.setPointSize(nOldFontSize);
        paint.setFont(font);
        paint.setBackgroundMode(Qt::TransparentMode);
        prev = next;
    }
    paint.setPen(QPen(QColor(Qt::black)));

    int nNodeNum = 0;
    foreach (XNode node, m_lstNode) {
        QPen penTmp = paint.pen();
        int nLineWidth = penTmp.width();
        if (m_lstShortPath.contains(nNodeNum)) {
            penTmp.setWidth(LINE_WIDTH);
            penTmp.setColor(Qt::red);
            paint.setPen(penTmp);
        }
        if (node.uType == NODE_FARM) {
            paint.setBrush(QBrush(QColor(0, 255, 255, 255)));
            paint.drawRect(node.ptPos.x() - RECT_SIZE / 2, node.ptPos.y() - RECT_SIZE / 2, RECT_SIZE, RECT_SIZE);
        }
        else if (node.uType == NODE_SHOP) {
            paint.setBrush(QBrush(QColor(255, 0, 255, 255)));
            paint.drawRect(node.ptPos.x() - RECT_SIZE / 2, node.ptPos.y() - RECT_SIZE / 2, RECT_SIZE, RECT_SIZE);
        }
        else if (node.uType == NODE_MID) {
            paint.setBrush(QBrush(QColor(255, 255, 0, 255)));
            paint.drawEllipse(node.ptPos.x() - RECT_SIZE / 2, node.ptPos.y() - RECT_SIZE / 2, RECT_SIZE, RECT_SIZE);
        }
        penTmp.setWidth(nLineWidth);
        penTmp.setColor(Qt::black);
        paint.setPen(penTmp);

        QFont font = paint.font();
        int nOldFontSize = font.pointSize();
        font.setPointSize(FONT_SIZE);
        paint.setFont(font);
        QString strTextOnNode;
        switch (node.uType) {
        case NODE_FARM:
            strTextOnNode = 'F';
            break;
        case NODE_SHOP:
            strTextOnNode = 'S';
            break;
        case NODE_MID:
            strTextOnNode = ' ';
            break;
        }

        paint.drawText(QRect(node.ptPos.x() - RECT_SIZE / 2, node.ptPos.y() - RECT_SIZE / 2, RECT_SIZE, RECT_SIZE), Qt::AlignCenter, strTextOnNode);
        font.setPointSize(nOldFontSize);
        paint.setFont(font);
        nNodeNum ++;
    }
}

void XMainWindow::Dijsktra() {
    int start = m_nSource;
    int n = m_lstNode.count();
    int *visited = new int[n];
    int *distances = new int[n];
    int *path = new int[n];

    for (int i = 0; i < n; i ++) {
        if (m_dblWeight[start][i] > 0) {
            distances[i] = m_dblWeight[start][i];
        }
        else {
            distances[i] = 50000;
        }
        visited[i] = 0;
        path[i] = -1;
    }

    distances[start] = 0;
    visited[start] = 1;

    for (int count = 1; count < n - 1; count ++) {
        int min = 50000;
        int minIndex = -1;
        for (int i = 0; i < n; i ++) {
            if (visited[i] == 0 && distances[i] < min) {
                min = distances[i];
                minIndex = i;
            }
        }
        int u = minIndex;

        visited[u] = 1;

        for (int v = 0; v < n; v++) {
            if (0 == visited[v] && m_dblWeight[u][v] > 0 && distances[u] != 50000 && distances[u] + m_dblWeight[u][v] < distances[v]) {
                distances[v] = distances[u] + m_dblWeight[u][v];
                path[v] = u;
            }
        }
    }

    for (int i = 0; i < n; i ++) {
        qDebug() << distances[i] << endl;
    }
    printPath(path, m_nEndPoint);

    qDebug() << "-----------------" << endl;
    qDebug() << m_lstShortPath;
}

void XMainWindow::printPath(int path[], int dest) {
    if (path[dest] == -1) {
        qDebug() << dest;
        m_lstShortPath.append(m_nSource);
        m_lstShortPath.append(dest);
        return;
    }

    printPath(path, path[dest]);
    qDebug() << "-> " << dest;
    m_lstShortPath.append(dest);
}

void XMainWindow::on_actionStart_triggered()
{
    ui->actionFarm->setChecked(false);
    ui->actionShop->setChecked(false);
    ui->actionMid->setChecked(false);
    ui->actionPath->setChecked(false);

    QStringList lstPairs;
    QString strPair, strPath;
    if (NULL != m_pResDlg) {
        delete m_pResDlg;
    }
    m_pResDlg = new XResDialog(this);
    for (int i = 0; i < MIN_MACRO(m_lstFarm.count(), m_lstShop.count()); i ++) {
        m_nSource = m_lstFarm[i];
        m_nEndPoint = m_lstShop[i];
        strPair = m_lstNode[m_nSource].strTownName + " ---> " + m_lstNode[m_nEndPoint].strTownName;

        m_lstShortPath.clear();
        Dijsktra();
        strPath = " ";
        foreach (int nNodeNum, m_lstShortPath) {
            strPath += m_lstNode[nNodeNum].strTownName;
            if (nNodeNum != m_nEndPoint) {
                strPath += " ---> ";
            }
        }

        lstPairs << strPair;
        lstPairs << strPath;
        m_pResDlg->setResList(lstPairs);

        connect(m_pResDlg, SIGNAL(pairSelect(int)), this, SLOT(showPair(int)));
    }
    m_pResDlg->show();
}

void XMainWindow::on_actionSave_triggered()
{
    QFile file("maps.info");
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    QDataStream ds(&file);
    // Serialize m_lstNode:
    ds << m_lstNode.count();
    foreach (XNode node, m_lstNode) {
        ds << node.strTownName;
        ds << node.uType;
        ds << node.ptPos;
    }

    // Serialize m_lstFarm:
    ds << m_lstFarm;

    // Serialize m_lstShop:
    ds << m_lstShop;

    // Serialize m_dblWeight:
    ds << MAX_NODE_COUNT;
    for (int i = 0; i < MAX_NODE_COUNT; i ++ ) {
        for (int j = 0; j < MAX_NODE_COUNT; j ++ ) {
            ds << m_dblWeight[i][j];
        }
    }

    file.close();
}

void XMainWindow::on_actionLoad_triggered()
{
    QFile file("maps.info");
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QDataStream ds(&file);
    int nCount;
    // Serialize m_lstNode:
    XNode node;
    ds >> nCount;
    m_lstNode.clear();
    for (int i = 0; i < nCount; i ++) {
        ds >> node.strTownName;
        ds >> node.uType;
        ds >> node.ptPos;
        m_lstNode.append(node);
    }

    // Serialize m_lstFarm:
    ds >> m_lstFarm;

    // Serialize m_lstShop:
    ds >> m_lstShop;

    // Serialize m_dblWeight:
    ds >> nCount;
    for (int i = 0; i < nCount; i ++ ) {
        for (int j = 0; j < nCount; j ++ ) {
            ds >> m_dblWeight[i][j];
        }
    }

    file.close();
    update();
}

void XMainWindow::on_actionShow_triggered()
{
}

void XMainWindow::showPair(int nPairNum) {
    m_nSource = m_lstFarm[nPairNum];
    m_nEndPoint = m_lstShop[nPairNum];
    m_lstShortPath.clear();
    Dijsktra();
    update();
}
