#include "clrplt.h"
#include "ui_clrplt.h"

clrplt::clrplt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::clrplt)
{
    ui->setupUi(this);
    this->setWindowTitle("ColorPlot");
    setAcceptDrops(true);
    Zco.toStdVector();
    oldsize = QPointF(width(),height());
    timer = new QTimer(this); //Отправляет значение координат в соответствии с позицией курсора
    connect(timer,SIGNAL(timeout()),this,SLOT(ShowCo()));
    timer->start(50);
    connect(this,SIGNAL(sendwrongpath(QString)),this,SLOT(getwrongpath(QString))); //если произошла ошибка при открытии файла
    connect(this,SIGNAL(sendrepaint()),this,SLOT(getrepaint()));                   //рисует изображение на виджете
    connect(this,SIGNAL(sendloadpercent(QString)),this,SLOT(getloadpercent(QString))); //отображает процент загрузки
    connect(this,SIGNAL(sendclear()),this,SLOT(clear())); //очищает экран
}

clrplt::~clrplt()
{
    delete ui;
}

void clrplt::clear()
{
    if(future.isRunning())
      {
          interruptflag = true;
          future.waitForFinished();
      }
      Zco.clear();
      img = QPixmap();
      imagesaved = QPixmap();
      colorcoef = 0;
      bpoint = QPointF(0,0);
      oldsize = QPointF(width(),height());
      emit sendrepaint();
}

void clrplt::getwrongpath(QString message)
{
    QMessageBox::warning(this,"Error",message,QMessageBox::Ok);
}
void clrplt::getrepaint()
{
    this->repaint();
}

void clrplt::getloadpercent(QString message)
{
    this->setWindowTitle(message);
}

void clrplt::resizeEvent(QResizeEvent *event) //изменение системы координат, если изменился размер виджета
{
    Q_UNUSED(event)
    bpoint = QPointF(bpoint.x()*width()/oldsize.x(),bpoint.y()*height()/oldsize.y());
    kX = width()/(oldsize.x()/kX);
    kY = height()/(oldsize.y()/kY);
    oldsize = QPointF(width(),height());
    if(future.isRunning())
        StartPaint();
}

void clrplt::dragEnterEvent(QDragEnterEvent *event) //нужны, чтобы можно было перетащить файл на виджет
{
    event->accept();
}
void clrplt:: dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}
void clrplt::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}
void clrplt::dropEvent(QDropEvent *event)
{
    QString path = event->mimeData()->text();
    path = path.right(path.size() - 8);
    if(path.isEmpty())
        return;
    emit sendclear();
    QtConcurrent::run(this,&clrplt::Load,path);
}

void clrplt::closeEvent(QCloseEvent *event) //корректное завершение, если есть активный поток
{
   Q_UNUSED(event)
   if(future.isRunning())
   {
       interruptflag = true;
       future.waitForFinished();
   }
}

void clrplt::paintEvent(QPaintEvent*)
{
    if(!img.isNull())
    {
        QPixmap imgscaled = img.scaled(width(),height(),Qt::IgnoreAspectRatio);
        if(selectflag || moveflag)
        {
            QPainter painter(&imgscaled);
            painter.beginNativePainting();
            painter.setRenderHint(QPainter::Antialiasing, true);
            if(selectflag) // прямоугольник для вырезания изображения
            {
               painter.setCompositionMode(QPainter::RasterOp_SourceAndNotDestination);
               painter.setPen(QPen((QColor(255,255,255)),2,Qt::SolidLine));
               painter.drawRect(QRectF(selectbpoint,selectepoint));
             }
             if(moveflag) // перемещения изображения
                painter.drawPixmap(int(translate.x()),int(translate.y()),imagesaved);
              painter.endNativePainting();
         }
        QPainter painter(this);
        painter.eraseRect(rect());
        painter.drawPixmap(0,0,imgscaled);// конечное изображение
    }
}

void clrplt::mousePressEvent(QMouseEvent *event)
{
    if(Zco.isEmpty() || future.isRunning())
        return;
    if(event->button() == Qt::LeftButton)
    {
        startpos = event->pos();
        moveflag = true;
        return;
    }
    if(event->button() == Qt::RightButton)
    {
        selectbpoint = event->pos();
        selectflag = true;
        return;
    }
}

void clrplt::mouseMoveEvent(QMouseEvent *event)
{
    if(Zco.isEmpty() || future.isRunning())
        return;
    if(moveflag)
    {
        translate += event->pos() - startpos;
        img = img.scaled(size());
        imagesaved = imagesaved.scaled(size());
        startpos = event->pos();
        QPainter painter;
        painter.begin(&img);
        painter.eraseRect(rect());
        painter.setBrush(QBrush(QColor(30,144,255)));
        painter.setPen(QPen(QColor(30,144,255),1,Qt::SolidLine));
        painter.drawRect(0,0,width(),height());
        painter.end();
        emit sendrepaint();
        return;
    }
    if(selectflag)
    {
        selectepoint = event->pos();
        emit sendrepaint();
        return;
    }
}

void clrplt::mouseReleaseEvent(QMouseEvent *event)
{
    if(Zco.isEmpty())
        return;
    if(event->button() == Qt::LeftButton)
    {
        bpoint += translate;
        StartPaint();
        translate = QPointF(0,0);
        moveflag = false;
        return;
    }
    if(event->button() == Qt::MiddleButton)
    {
      Back();
      imagesaved = img;
      return;
    }
    if(event->button() == Qt::RightButton) // расчет прямоугольника для вырезания
    {
        selectflag = false;
        if(event->pos().x() < selectbpoint.x())
        {
            selectepoint.setX(selectbpoint.x());
            selectbpoint.setX(event->pos().x());
        }
        else
            selectepoint.setX(event->pos().x());
        if(event->pos().y() < selectbpoint.y())
        {
            selectepoint.setY(selectbpoint.y());
            selectbpoint.setY(event->pos().y());
        }
        else
            selectepoint.setY(event->pos().y());
        if(int(selectbpoint.x()) == int(selectepoint.x()) || int(selectbpoint.y()) == int(selectepoint.y()))
        {
            emit sendrepaint();
            return;
        }
        selectbpoint = QPointF((selectbpoint.x() - bpoint.x())/kX*((double(width())/Zco[0].size())),(selectbpoint.y() - bpoint.y())/kY*(double(height())/Zco.size()));
        selectepoint = QPointF((selectepoint.x() - bpoint.x())/kX*((double(width())/Zco[0].size())),(selectepoint.y() - bpoint.y())/kY*(double(height())/Zco.size()));
        kX = double(width())/Zco[0].size();
        kY = double(height())/Zco.size();
        kX *= width()/(selectepoint.x() - selectbpoint.x());
        kY *= height()/(selectepoint.y() - selectbpoint.y());
        bpoint = -(QPointF(selectbpoint.x()*kX/(double(width())/Zco[0].size()),selectbpoint.y()*kY/(double(height())/Zco.size())));
        StartPaint();
    }
}

void clrplt::wheelEvent(QWheelEvent *event) //Зум
{
    if(Zco.isEmpty() || int(cursor().pos().x() - this->geometry().x()) < 0
    ||  int(cursor().pos().x() - this->geometry().x()) > width()
    ||  int(cursor().pos().y() - this->geometry().y()) < 0
    ||  int(cursor().pos().y() - this->geometry().y()) > height()) return;
    QPointF delta = event->angleDelta()/120;
    QPointF oldCo = QPointF(cursor().pos().x() - this->geometry().x(),cursor().pos().y() - this->geometry().y()) - bpoint;
    QPointF newCo;
    if(delta.y() > 0)
    {
        kX*=2;
        kY*=2;
        newCo = QPointF(oldCo.x()*2,oldCo.y()*2);
    }
    if(delta.y() < 0)
    {
       kX/=2;
       kY/=2;
       newCo = QPointF(oldCo.x()/2,oldCo.y()/2);
    }
    bpoint -= QPointF(newCo.x() - oldCo.x(),newCo.y() - oldCo.y());
    StartPaint();
}

void clrplt::on_actionAdd_file_triggered()
{
    QString path = (QFileDialog::getOpenFileName(nullptr,"Choose input file","*.txt",""));
    if(path.isEmpty())
        return;
    emit sendclear();
    QtConcurrent::run(this,&clrplt::Load,path);
}

void clrplt::GetPath(QString path) // Добавление файла, через консоль
{
    if(path.isEmpty())
        return;
    emit sendclear();
    QtConcurrent::run(this,&clrplt::Load,path);
}

void clrplt::on_actionClear_triggered()
{
    emit sendclear();
}

void clrplt::Load(QString path)
{
    QFile Inputfile(path);
    QFileInfo InputfileInfo(path);
    if(!Inputfile.exists() || !Inputfile.open(QIODevice::ReadOnly) || InputfileInfo.completeSuffix() != "txt")
    {
        emit sendwrongpath("File doesn't exist or couldn't be open!");
        return;
    }
    QTextStream stream(&Inputfile);
    QVector<QString> lines;
    while(!stream.atEnd())
        lines.push_back(stream.readLine() + '\t');
    Inputfile.close();
    Zco.resize(lines.size());
    Convert(lines);
    kX = double(width())/Zco[0].size();
    kY = double(height())/Zco.size();
    emit sendloadpercent("ColorPlot");
    future = QtConcurrent::run(this,&clrplt::Painting);
}

void clrplt::Convert(QVector<QString> &lines)
{
   QString strforconvert = "";
   for(int i = 0; i < lines.size(); i++)
   {
       for(int j = 0; j < lines[i].size(); j++)
       {
           strforconvert += lines[i][j];
           if(lines[i][j] == '\t')
           {
              Zco[i].push_back(strforconvert.toDouble());
              strforconvert = "";
           }
       }
       int percent = int(double(i+1)/lines.size()*100);
       emit sendloadpercent("Reading and Converting  " + QString::number(percent) + "%");
   }
}

double clrplt::ColorCoef()
{
    double Max = 0;
    bool flag = true;
    int i_start = 0;
    int j_start = 0;
    int i_end = Zco.size();
    int j_end = Zco[0].size();
    if(ui->actionColor_scaling->isChecked())
    {
        if(bpoint.y() < 0)
           i_start = int(-bpoint.y()/kY);
        if(bpoint.y() < 0)
            j_start = int(-bpoint.x()/kX);
        if(int((height()-bpoint.y())/kY) < Zco.size())
           i_end = int((height()-bpoint.y())/kY) + 1;
        if(int((width()-bpoint.x())/kX) < Zco[0].size())
           j_end = int((width()-bpoint.x())/kX) + 1;
    }
    for(int i = i_start; i < i_end; i++)
    {
        for(int j = j_start; j < j_end; j++)
        {
            if(flag)
            {
                Max = Min = Zco[i][j];
                flag = false;
            }
            if(Min > Zco[i][j])
                Min = Zco[i][j];
            if(Max < Zco[i][j])
                Max = Zco[i][j];
        }
    }
    if(Max - Min < 0.0001)
        return 255;

   return 255/(Max-Min);
}

QColor clrplt::white_gray_black(int color) //цвет для прямоугольника
{
    if(color >=0 && color <= 255)
        return QColor(color,color,color);
    return  QColor(30,144,255);
}

QColor clrplt::blue_white_red(int color)
{
    if(color >= 0 && color < 86)
        return QColor(255-color,0,0);
    if(color >= 86 && color < 191)
        return  QColor(255,255,255);
    if(color >= 191 && color <= 255)
        return  QColor(0,0,color);
    return  QColor(30,144,255);
}

QColor clrplt::green_yellow_red(int color)
{
    if(color >= 0 && color < 86)
        return QColor(255-color,0,0);
    if(color >= 86 && color < 191)
        return QColor(color,color,0);
    if(color >= 191 && color <= 255)
       return QColor(0,color,0);
    return  QColor(30,144,255);
}

QColor clrplt::AllColors(double Zco)
{
    double Z = Zco - Min;
    if(ui->actionblack_gray_white->isChecked())
        return white_gray_black(int(colorcoef*Z));
    if(ui->actionblue_white_red->isChecked())
        return blue_white_red(int(colorcoef*Z));
    if(ui->actiongreen_yellow_red->isChecked())
        return green_yellow_red(int(colorcoef*Z));
    return  QColor(30,144,255);
}

QColor clrplt::GridColor(QColor color)
{
    if(ui->actionGrid->isChecked() && !ui->actionblue_white_red->isChecked())
      return QColor(255,255,255);
    else if(ui->actionGrid->isChecked() && ui->actionblue_white_red->isChecked())
      return QColor(0,0,0);
    else
      return color;
}

void clrplt::Painting()
{
    if(interruptflag)
    {
        img = QPixmap(size());
        interruptflag = false;
        return;
    }
    QPainter painter;
    QPixmap image = QPixmap(size());
    painter.begin(&image);
    painter.eraseRect(rect());
    painter.setBrush(QBrush(QColor(30,144,255)));
    painter.setPen(QPen(QColor(30,144,255),1,Qt::SolidLine));
    painter.drawRect(0,0,width(),height());
    colorcoef = ColorCoef();
    const int H = height();
    const int W = width();
    int i_start = 0;
    int j_start = 0;
    int i_end = Zco.size();
    int j_end = Zco[0].size();
    if(bpoint.y() < 0)
       i_start = int(-bpoint.y()/kY);
    if(bpoint.x() < 0)
       j_start = int(-bpoint.x()/kX);
    if(int((H-bpoint.y())/kY) < Zco.size())
       i_end = int((H-bpoint.y())/kY) + 1;
    if(int((W-bpoint.x())/kX) < Zco[0].size())
       j_end = int((W-bpoint.x())/kX) + 1;
    for(int i = i_start; i < i_end; i++)
    {
        for(int j = j_start; j < j_end; j++)
        {
            if(interruptflag)
            {
                painter.end();
                img = QPixmap(size());
                interruptflag = false;
                return;
            }
            QColor color = AllColors(Zco[i][j]);
            painter.setBrush(QBrush(color));
            painter.setPen(QPen(GridColor(color),1,Qt::SolidLine));
            painter.drawRect(QRectF(bpoint.x() + j*kX,bpoint.y() + i*kY,kX,kY));
        }
        if(i % (1 + int(1/kY)) == 0)
        {
           img = image;
           emit sendrepaint();
        }
    }
    painter.end();
    img = image;
    imagesaved = image;
    emit sendrepaint();
}

void clrplt::StartPaint()
{
    if(future.isRunning())
    {
      interruptflag = true;
      future.waitForFinished();
    }
    img = QPixmap(size());
    future = QtConcurrent::run(this,&clrplt::Painting);
}

void clrplt::ShowCo()
{
    if(Zco.isEmpty() || future.isRunning())
        return;
    int xpos = int(((cursor().pos().x() - this->geometry().x())-bpoint.x())/kX);
    int ypos = int(((cursor().pos().y() - this->geometry().y())-bpoint.y())/kY);
    if(xpos >= 0 && xpos < Zco[0].size() && ypos >= 0 && ypos < Zco.size() &&
    (cursor().pos().x() - this->geometry().x() >= 0) && (cursor().pos().y() - this->geometry().y() >= 0) &&
     cursor().pos().x() - this->geometry().x() <= width() && (cursor().pos().y() - this->geometry().y() <= height()) &&
     this->isActiveWindow())
     {
       QToolTip::showText(QPoint(this->pos().x()+width()/2-50,this->pos().y()+15),
       "X: " + QString::number(xpos)  +  "  " + "Y: " + QString::number(ypos) + "  "
       + "Z: " + QString::number(Zco[ypos][xpos]));
     }
}

void clrplt::on_actionblack_gray_white_triggered()
{
    ui->actionblack_gray_white->setChecked(true);
    ui->actionblue_white_red->setChecked(false);
    ui->actiongreen_yellow_red->setChecked(false);
    StartPaint();
}

void clrplt::on_actionblue_white_red_triggered()
{
    ui->actionblue_white_red->setChecked(true);
    ui->actionblack_gray_white->setChecked(false);
    ui->actiongreen_yellow_red->setChecked(false);
    StartPaint();
}

void clrplt::on_actiongreen_yellow_red_triggered()
{
    ui->actiongreen_yellow_red->setChecked(true);
    ui->actionblack_gray_white->setChecked(false);
    ui->actionblue_white_red->setChecked(false);
    StartPaint();
}

void clrplt::Back() // Возвращает изображение в начальное положение
{
    bpoint = QPointF(0,0);
    oldsize = QPointF(width(),height());
    kX = double(width())/Zco[0].size();
    kY = double(height())/Zco.size();
    StartPaint();
}

void clrplt::on_actionGrid_triggered() // Cетка
{
    if(Zco.isEmpty())
        return;
    StartPaint();
}

void clrplt::on_actionColor_scaling_triggered() // Пересчитывает цветовой коэффициент для текущей локальной части
{
    if(Zco.isEmpty())
        return;
    StartPaint();
}

void clrplt::on_actionColorPlot_triggered()
{
    QMessageBox::information(this,"СolorPlot","Приложение строит цветовую схему высот трехмерного графика\nАвтор: Канониров Александр\n"
                                          "Дата создания: 28.02.2020",QMessageBox::Ok);
}
