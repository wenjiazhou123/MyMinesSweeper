#include "mainwindow.h"
#include <QPainter>
#include "./ui_mainwindow.h"
#include <QDebug>

QMap<itemType,QPixmap> map;

inline bool isNum(itemType b) {
    return b == ONE || b == TWO || b == THREE ||
           b == FOUR || b == FIVE || b == SIX || b == SEVEN ||
           b == EIGHT;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setWindowTitle("扫雷");
    setWindowFlags(Qt::WindowCloseButtonHint);
    QPixmap pixmap(":/images/all.jpg");
    for (int i = 0; i < 13; ++i) {
        QPixmap p=pixmap.copy(edge_pix+item_weight*i,first_row_height,pic_pix,pic_pix);
        map.insert((itemType)i,p);
    }
    map.insert(TEMPZERO,map[ZERO]);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            view[i][j]=UNKNOWN;
        }
    }
    initializeMinefield();
    known_number=0;
    clickPending = false;
    clickTimer = new QTimer(this);
    clickTimer->setSingleShot(true);
    connect(clickTimer, SIGNAL(timeout()), this, SLOT(handleSingleClick()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::DFS(int x, int y)
{
    if(view[y][x]!=UNKNOWN) return;
    if(isNum(mines[y][x])){
        view[y][x]=mines[y][x];
        known_number++;
    }else if(mines[y][x]==ZERO){
        view[y][x]=mines[y][x];
        known_number++;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int nr = x + dx, nc = dy + y;
                if (nr >= 0 && nr < rows && nc >= 0 && nc < columns) {
                    DFS(nr,nc);
                }
            }
        }
    }
}

void MainWindow::handleSingleClick()
{
    if (clickPending) {
        single_click(cx, cy);
        clickPending = false;
        clickTimer->stop();
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if(gameStatus==READY||gameStatus==START){
        painter.setBrush(Qt::gray);
        painter.drawRect(this->rect());
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < columns; ++j) {
                painter.drawPixmap(
                    edge_pix+j*item_weight,edge_pix+i*item_weight,pic_pix,pic_pix,
                    map[view[i][j]]);
            }
        }
    }else if(gameStatus==OVER){
        painter.fillRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, QColor(255, 0, 0, 100));
        painter.drawText(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Qt::AlignCenter, "You LOSE!");
    }else if(gameStatus==WIN){
        painter.fillRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, QColor(0, 255, 0, 100));
        painter.drawText(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Qt::AlignCenter, "You Win!");
    }

}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(gameStatus==OVER) return;
    cx=event->position().x()/item_weight;
    cy=event->position().y()/item_weight;
    if((event->buttons() & Qt::LeftButton && event->buttons() & Qt::RightButton)|event->button()&Qt::MiddleButton){
        double_click();
    }else if(event->button()==Qt::LeftButton){
        clickPending = true;
        clickTimer->start(50);  // 延时50毫秒
    }else if(event->button()==Qt::RightButton){
        if(view[cy][cx]==UNKNOWN) view[cy][cx]=FLAG;
        else if(view[cy][cx]==FLAG)view[cy][cx]=UNKNOWN;
    }
    repaint();

}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int nr = cx + dx, nc = dy + cy;
            if (nr >= 0 && nr < rows && nc >= 0 && nc < columns && view[nc][nr] == TEMPZERO) {
                view[nc][nr] = UNKNOWN;
            }
        }
    }
    repaint();
}
void MainWindow::initializeMinefield() {
    // 初始化所有格子为 UNKNOWN
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            mines[i][j] = ZERO;
        }
    }

    // 随机选择不同的位置放置雷
    srand(time(nullptr));
    QVector<QPair<int, int>> minePositions;
    while (minePositions.size() < mine_number) {
        int r = rand() % rows;
        int c = rand() % columns;
        QPair<int, int> pos(r, c);
        if (!minePositions.contains(pos)) {
            minePositions.push_back(pos);
            mines[r][c] = MINES;
        }
    }

    // 更新雷周围的格子
    for (const QPair<int, int>& mine : minePositions) {
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int nr = mine.first + dx, nc = mine.second + dy;
                if (nr >= 0 && nr < rows && nc >= 0 && nc < columns && mines[nr][nc] != MINES) {
                    mines[nr][nc] = static_cast<itemType>(mines[nr][nc] + 1);
                }
            }
        }
    }
}

void MainWindow::double_click()
{
    clickPending = false;
    if(view[cy][cx]==ZERO) return;
    int flag_Count=0;
    int unknow_Count=0;
    QVector<QPair<int, int>> unknownPositions;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int nc = cx + dx, nr = dy + cy;
            if (nr >= 0 && nr < rows && nc >= 0 && nc < columns) {
                if(view[nr][nc]==FLAG) flag_Count++;
                else if(view[nr][nc]==UNKNOWN) {
                    unknow_Count++;
                    QPair<int, int> pos(nr, nc);
                    unknownPositions.push_back(pos);
                }
            }
        }
    }
    int number=(int)mines[cy][cx];
    int remain=number-flag_Count;
    if(flag_Count==number){
        for(QPair<int,int> pos:unknownPositions) {
            single_click(pos.second,pos.first);
        }
    }else if(unknow_Count==remain){
        for(QPair<int,int> pos:unknownPositions) {
            view[pos.first][pos.second]=FLAG;
        }
    }else{
        for(QPair<int,int> pos: unknownPositions) {
            view[pos.first][pos.second]=TEMPZERO;
        }
    }
}

void MainWindow::single_click(int x, int y)
{
    if(view[y][x]!=UNKNOWN||view[y][x]==FLAG) return;
    if(mines[y][x]==MINES){
        view[y][x]=BOOM;
        gameStatus=OVER;
    }else{
        DFS(x,y);
        if(known_number==(rows*columns-mine_number)){
            gameStatus=WIN;
        }
    }
}

