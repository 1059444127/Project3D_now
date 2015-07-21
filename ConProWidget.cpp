

#include "ConProWidget.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>

#include <stdio.h>
#include <iostream>
#include <assert.h>

#include<QDebug>

#include "io_util.hpp"

#include "structured_light.hpp"

ConProWidget::ConProWidget(QWidget * parent, Qt::WindowFlags flags) :
    QWidget(parent, flags),
    _screen(0),
    _current_pattern(-1),
    _pattern_count(12),//共12张pattern,为了方便，从0开始计数^^^^^^^^^^^^^^^^^^^^^^^^要改
    _vbits(1),
    _hbits(1),
    _updated(false),
    _i(0),
    timer()
{
}

ConProWidget::~ConProWidget()
{
    stop();
}

bool ConProWidget::read_pattern()//要改
{

    for(int i=1;i<14;i++)
    {
        QString str="C:/Users/Administrator/Desktop/111/pattern/calibration_1024/";//文件名字可改，这里是不成熟代码^^^^^^^^^^^^^^^^^^^^^
        QString num=(QString("%1").arg(i));
        str=str+num+".jpg";
        patterns.push_back(cv::imread(str.toStdString()));
    }
    if (patterns.empty()||patterns.size()!=13)
        return false;
    return true;
}
void ConProWidget::reset(void)
{
    _current_pattern = -1;//设置计数为-1
    _updated = false;
    _pixmap = QPixmap();
    //emit make_new_image(_pixmap);可删
}

void ConProWidget::start(void)
{
    stop();
    reset();

    //validate screen
    QDesktopWidget * desktop = QApplication::desktop();//
    int screens =  desktop->screenCount();
    if (_screen<0 || _screen>=screens)
    {   //error, fix it
        _screen = screens;//所选定的投影屏幕号_screen
    }

    //display
    QRect screen_resolution = desktop->screenGeometry(_screen);//获得屏幕的分辨率，并通过这里设置选择哪个屏幕作为投影输出（确定了desktop所指定的桌面是哪个显示器显示的）
    //#############没有move（）函数则，一直在第二显示屏中全屏显示，有一个bebug，在第一显示屏中显示时不是全屏显示
    move(QPoint(screen_resolution.x(), screen_resolution.y()));//让此widget（即是ConProWidget）大小？？？？#####这句话没有用！！！！注释掉不影响
    showFullScreen();//让此widget全屏显示（有show的功能，最开始ConProWidget是不显示的）

    //update bit count for the current resolution
    //update_pattern_bit_count();可删
}

void ConProWidget::stop(void)
{
    hide();//隐藏project widget
    reset();//投影计数为-1，显示文字“no image”在project widget上
}

void ConProWidget::prev(void)
{
    if (_updated)
    {   //pattern not processed: wait
        return;
    }

    if (_current_pattern<1)
    {
        return;
    }

    _current_pattern--;
    _pixmap = QPixmap();//清空_pixmap，在paintEvent中会检查_current_pattern数来设置_pixmap，即witdeg要显示的图像
    update();
    QApplication::processEvents();
}

void ConProWidget::next(void)
{
    if (_updated)
    {   //pattern not processed: wait
        return ;
    }

    if (finished())
    {
        return ;
    }

    _current_pattern++;//_current_pattern+1
    _pixmap = QPixmap();
    update();//repaint widget???
    QApplication::processEvents();
    return ;
}

bool ConProWidget::finished(void)
{
    return (_current_pattern >= _pattern_count);////////////////////////问题在这里
}

void ConProWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    //if (_pixmap.isNull())
    if (_current_pattern<0)
    {   //stopped
        QRectF rect = QRectF(QPointF(0,0), QPointF(width(),height()));
        painter.drawText(rect, Qt::AlignCenter, "No image");
    }
    else
    {
        bool updated = false;
        if (_pixmap.isNull())
        {   //update
            updated = true;
            _pixmap = QPixmap::fromImage(io_util::qImage(patterns.at(_current_pattern)));////////////////将图片序列中提取图案
            //make_pattern();//产生新的投影图案
        }

        //draw
        QRectF rect = QRectF(QPointF(0,0), QPointF(width(),height()));
        painter.drawPixmap(rect, _pixmap, rect);//将投影图案显示在屏幕上（全屏）
          //  _pixmap.可删
       // _i++;可删
        //QString ii=(QString("[%1]").arg(_i));可删
        //QString str="C:/Users/Administrator/Desktop/111/1"+ii+".jpg";可删

        //_pixmap.save(str);可删



        if (updated)//意义是产生了新的投影图案
        {   //notfy update

            timer.start();
            while (timer.elapsed()<70)//////////////////////////////////////
            {
                QApplication::processEvents();
                //qDebug( "%d\n", t.elapsed() );
            }
            //qDebug()<<"projector"<<timer.currentTime();测试从投影触发到相机拍照之间的间隔
            //QString strname="C:/Users/Administrator/Desktop/111/pattern/capture_result/";可删
            //int time=timer.currentTime().msecsSinceStartOfDay();可删
            //QString name=strname+time+".jpg";可删
            //_pixmap.save(name);可删
            _updated = true;//产生新的pattern，且被投影出来,标记为_updated
            emit make_new_image(_updated);//发射信号，使imagelabel的图像更新为新一张要投影的图案
        }
    }
}


