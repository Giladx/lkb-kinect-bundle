#include "qtimagewidget.h"

#include <stdlib.h>

qtimagewidget::qtimagewidget(QWidget *parent)
    : QWidget(parent)
{
    floatBased = false;
    antialiased = false;
    frameNo = 0;

    setBackgroundRole(QPalette::Base);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    onNewClick=NULL;
    created=false;
}
qtimagewidget::~qtimagewidget()
{
    onNewClick=NULL;
    //delete cam;
}
void qtimagewidget::setFloatBased(bool floatBased)
{
    this->floatBased = floatBased;
    update();
}
void qtimagewidget::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}
QSize qtimagewidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize qtimagewidget::sizeHint() const
{
    return QSize(180, 180);
}

void qtimagewidget::nextAnimationFrame()
{
    ++frameNo;
    update();
}

void qtimagewidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if(created){
        QImage imgn(image.data, image.size().width, image.size().height, image.step, QImage::Format_RGB888);
        painter.drawImage(0,0,imgn);}

}
void qtimagewidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    globalLastPos=event->globalPos();
    isNewClick=true;
    cout << "Click in : " << globalLastPos.x() << "," << globalLastPos.y() << endl;
    if(onNewClick!=NULL){
        onNewClick(globalLastPos);
    }
}
void qtimagewidget::updateImage(bool col, Mat &img,double scale)
{
    //img.copyTo(image);
    image=img;
    color=col;
    //QImage imgn((uchar*)img.data, img.cols, img.rows, QImage::Format_RGB32);
    created=true;
    update();

}
