#

#include "ImageLabel.hpp"
#include<QApplication>
#include <QPainter>

#include "io_util.hpp"

ImageLabel::ImageLabel(QWidget * parent, Qt::WindowFlags flags): 
    QWidget(parent, flags),
    _pixmap(),//初始化为空
    _image(),
    _mutex(),
    timer()



{



}

ImageLabel::~ImageLabel()
{
}

void ImageLabel::paintEvent(QPaintEvent *)
{
    //qDebug( "%d\n", t.elapsed() );
    QPainter painter(this);

    if (_image.data && _image.type()==CV_8UC3)//要求是RGB的Mat
    {   //copy cv::Mat image
        _mutex.lock();
        _pixmap = QPixmap::fromImage(io_util::qImage(_image));//将mat类型转换为QImage再转换为QPixmap,_image赋给_pixmap
        _image = cv::Mat();
        _mutex.unlock();
    }

    if (!_pixmap.isNull())
    {
        QPixmap scale_pixmap = _pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);//将_pixmap的尺寸匹配与widget相合适，保持宽高比，可在widget中有空余
        QRectF rect = QRectF(QPointF(0,0), QPointF(scale_pixmap.width(),scale_pixmap.height()));
        painter.drawPixmap(rect, scale_pixmap, rect);

       // timer.start();


       // while (timer.elapsed()<60)
        //{
         //   QApplication::processEvents();
        //    qDebug( "%d\n", timer.elapsed() );
        //}


    }
    else 
    {
        QRectF rect = QRectF(QPointF(0,0), QPointF(width(),height()));
        painter.drawText(rect, Qt::AlignCenter, "No image");//没有图片则显示出文字“No image"
    }
}

void ImageLabel::setImage(cv::Mat const& image)
{
    //save image as cv::Mat because QPixmap's cannot be used in threads只能用在GUI线程中 ，这样用，再保证安全复制image的情况下，加快复制速度（多个线程，执行该代码段的机会比执行QPixmap（只在GUI线程中执行，线程少，或只有一个 ）机会大）
    _mutex.lock();//用互斥量来保证对变量（关键的代码段）的排他性访问
    image.copyTo(_image);//耗时1ms
    _mutex.unlock();
    update();

}


