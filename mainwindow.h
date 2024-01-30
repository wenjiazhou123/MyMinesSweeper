#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QTimer>
const int first_row_height=24;
const int edge_pix=1;
const int pic_pix=19;
const int item_weight=edge_pix+pic_pix;
const int rows=9;
const int columns=9;
const int mine_number=10;
const int WINDOW_WIDTH=columns*item_weight;
const int WINDOW_HEIGHT=rows*item_weight;

enum itemType{
    ZERO=0,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    MINES,
    UNKNOWN,
    FLAG,
    BOOM,
    TEMPZERO
};
enum GAMESTATUS{
    READY,
    START,
    OVER,
    WIN
};


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    itemType view[rows][columns] ;
    itemType mines[rows][columns];
    int cx;
    int cy;
    int known_number;
    GAMESTATUS gameStatus=READY;
    void DFS(int x,int y);
public slots:
    void handleSingleClick();
private:
    Ui::MainWindow *ui;
    bool clickPending;
    QTimer *clickTimer;

    // QWidget interface
    void initializeMinefield();
    void double_click();
    void single_click(int x, int y);
protected:
    void paintEvent(QPaintEvent *event);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);

    // QWidget interface
protected:
    void mouseReleaseEvent(QMouseEvent *event);
};
#endif // MAINWINDOW_H
