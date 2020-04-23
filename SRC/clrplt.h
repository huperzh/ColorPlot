#ifndef CLRPLT_H
#define CLRPLT_H

#include <QMainWindow>
#include <QFileDialog>
#include <QtConcurrent>
#include <QPainter>
#include <QPixmap>
#include <QMessageBox>
#include <QToolTip>
#include <QMouseEvent>

namespace Ui {
class clrplt;
}

class clrplt : public QMainWindow
{
    Q_OBJECT

public:
    explicit clrplt(QWidget *parent = nullptr);
    void GetPath(QString);
    ~clrplt() override;

signals:
    void sendwrongpath(QString);
    void sendrepaint();
    void sendendofpaint();
    void sendloadpercent(QString);
    void sendCo(QString);
    void sendclear();


private slots:
    void on_actionAdd_file_triggered();

    void on_actionClear_triggered();

    void getwrongpath(QString);

    void getrepaint();

    void getloadpercent(QString);

    void clear();

    void on_actionblack_gray_white_triggered();

    void on_actionblue_white_red_triggered();

    void on_actiongreen_yellow_red_triggered();

    void ShowCo();

    void on_actionGrid_triggered();

    void on_actionColor_scaling_triggered();

    void on_actionColorPlot_triggered();

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::clrplt *ui;
    QVector<QVector<double>> Zco;
    double kX = 1;
    double kY = 1;
    double colorcoef;
    double Min;
    QPixmap img;
    QPixmap imagesaved;
    QFuture<void> future;
    bool interruptflag = false;
    QTimer *timer;
    QPointF startpos;
    QPointF translate = QPointF(0,0);
    QPointF bpoint = QPointF(0,0);
    QPointF selectbpoint = QPointF(0,0);
    QPointF selectepoint = QPointF(0,0);
    QPointF oldsize = QPointF(0,0);
    bool moveflag = false;
    bool selectflag = false;

    void Load(QString);
    void Convert(QVector<QString> &);
    QColor white_gray_black(int);
    QColor blue_white_red(int);
    QColor green_yellow_red(int);
    QColor AllColors(double);
    QColor GridColor(QColor);
    double ColorCoef();
    void Painting();
    void StartPaint();
    void Back();
};

#endif // CLRPLT_H

