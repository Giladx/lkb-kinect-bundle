#ifndef QTIMAGEWIDGET_H
#define QTIMAGEWIDGET_H
#include <QtGui>

#include <QWidget>
#include <cameras.h>
class qtimagewidget : public QWidget
{
    Q_OBJECT

public:
    qtimagewidget(QWidget *parent = 0);
    ~qtimagewidget();
    void setFloatBased(bool floatBased);
    void setAntialiased(bool antialiased);
    void updateImage(bool color,Mat& img, double scale=1.0);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    QPoint lastPos;
    QPoint globalLastPos;
    bool isNewClick;
    bool isResized;
    bool created;
    //void resizeGL(int width, int height);
    void (*onNewClick)(QPoint pos);
public slots:
    void nextAnimationFrame();

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    bool floatBased;
    bool antialiased;
    int frameNo;
    bool color;
    Mat image;
    QImage *im;
};

#endif // QTIMAGEWIDGET_H
