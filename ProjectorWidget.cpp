/*
Copyright (c) 2012, Daniel Moreno and Gabriel Taubin
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Brown University nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL DANIEL MORENO AND GABRIEL TAUBIN BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "ProjectorWidget.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>

#include <stdio.h>
#include <iostream>
#include <assert.h>

#include "structured_light.hpp"

ProjectorWidget::ProjectorWidget(QWidget * parent, Qt::WindowFlags flags) : 
    QWidget(parent, flags),
    _screen(0),
    _current_pattern(-1),
    _pattern_count(4),
    _vbits(1),
    _hbits(1),
    _updated(false),
    _i(0)
{
}

ProjectorWidget::~ProjectorWidget()
{
    stop();
}

void ProjectorWidget::reset(void)
{
    _current_pattern = -1;//设置计数为-1
    _updated = false;
    _pixmap = QPixmap();
    emit new_image(_pixmap);
}

void ProjectorWidget::start(void)
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
    move(QPoint(screen_resolution.x(), screen_resolution.y()));//让此widget（即是projectorwidget）大小？？？？#####这句话没有用！！！！注释掉不影响
    showFullScreen();//让此widget全屏显示（有show的功能，最开始projectorwidget是不显示的）

    //update bit count for the current resolution
    update_pattern_bit_count();
}

void ProjectorWidget::stop(void)
{
    hide();//隐藏project widget
    reset();//投影计数为-1，显示文字“no image”在project widget上
}

void ProjectorWidget::prev(void)
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

void ProjectorWidget::next(void)
{
    if (_updated)
    {   //pattern not processed: wait
        return;
    }

    if (finished())
    {
        return;
    }

    _current_pattern++;//_current_pattern+1
    _pixmap = QPixmap();
    update();//repaint widget???
    QApplication::processEvents();
}

bool ProjectorWidget::finished(void)
{
    return (_current_pattern+2 > 2+4*_pattern_count);
}

void ProjectorWidget::paintEvent(QPaintEvent *)
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
            make_pattern();//产生新的投影图案
        }

        //draw
        QRectF rect = QRectF(QPointF(0,0), QPointF(width(),height()));
        painter.drawPixmap(rect, _pixmap, rect);//将投影图案显示在屏幕上（全屏）
          //  _pixmap.
       // _i++;
        //QString ii=(QString("[%1]").arg(_i));
        //QString str="C:/Users/Administrator/Desktop/111/1"+ii+".jpg";

        //_pixmap.save(str);



        if (updated)
        {   //notfy update
            _updated = true;
            emit new_image(_pixmap);//发射信号，使imagelabel的图像更新为新一张要投影的图案
        }
    }
}

void ProjectorWidget::update_pattern_bit_count(void)
{
    int cols = width();
    int rows = height();

    //search bit number
    _vbits = 1;
    _hbits = 1;
    for (int i=(1<<_vbits); i<cols; i=(1<<_vbits)) { _vbits++; }//屏幕width的像素数所需要_vbits个数个比特才能表示出来eg.512*512图片，_vbits=9
    for (int i=(1<<_hbits); i<rows; i=(1<<_hbits)) { _hbits++; }
    _pattern_count = std::min(std::min(_vbits, _hbits), _pattern_count);//需要投影的图案的数量要根据屏幕的分辨率来权衡，取最小值
    std::cerr << " vbits " << _vbits << " / cols="<<cols<<", mvalue="<< ((1<<_vbits)-1) << std::endl;//最大值为512-1=511
    std::cerr << " hbits " << _hbits << " / rows="<<rows<<", mvalue="<< ((1<<_hbits)-1) << std::endl;
    std::cerr << " pattern_count="<< _pattern_count << std::endl; //投影图案的数目
}

void ProjectorWidget::make_pattern(void)
{
    int cols = width();
    int rows = height();

    /*
    if (_current_pattern<1)
    {   //search bit number
        _vbits = 1;
        _hbits = 1;
        for (int i=(1<<_vbits); i<cols; i=(1<<_vbits)) { _vbits++; }
        for (int i=(1<<_hbits); i<rows; i=(1<<_hbits)) { _hbits++; }
        _pattern_count = std::min(std::min(_vbits, _hbits), _pattern_count);
        std::cerr << " vbits " << _vbits << " / cols="<<cols<<", mvalue="<< ((1<<_vbits)-1) << std::endl;
        std::cerr << " hbits " << _hbits << " / rows="<<rows<<", mvalue="<< ((1<<_hbits)-1) << std::endl;
        std::cerr << " pattern_count="<< _pattern_count << std::endl; 
    }
    */

    int vmask = 0, voffset = ((1<<_vbits)-cols)/2, hmask = 0, hoffset = ((1<<_hbits)-rows)/2, inverted = (_current_pattern%2)==0;

    // patterns
    // -----------
    // 00 white
    // 01 black
    // -----------
    // 02 vertical, bit N-0, normal
    // 03 vertical, bit N-0, inverted
    // 04 vertical, bit N-1, normal
    // 04 vertical, bit N-2, inverted
    // ..
    // XX =  (2*_pattern_count + 2) - 2 vertical, bit N, normal
    // XX =  (2*_pattern_count + 2) - 1 vertical, bit N, inverted
    // -----------
    // 2+N+00 = 2*(_pattern_count + 2) horizontal, bit N-0, normal
    // 2+N+01 horizontal, bit N-0, inverted
    // ..
    // YY =  (4*_pattern_count + 2) - 2 horizontal, bit N, normal
    // YY =  (4*_pattern_count + 2) - 1 horizontal, bit N, inverted

    if (_current_pattern<2)
    {   //white or black
        _pixmap = make_pattern(rows, cols, vmask, voffset, hmask, hoffset, inverted);
    }
    else if (_current_pattern<2*_pattern_count+2)//垂直条纹
    {   //vertical
        int bit = _vbits - _current_pattern/2;//eg._vbits=11;bit=10,9,8,7,,,,,,,1
        vmask = 1<<bit;//产生掩码,1024,512,256,,,,2
        //std::cerr << "v# cp: " << _current_pattern << " bit:" << bit << " mask:" << vmask << std::endl;
        _pixmap = make_pattern(rows, cols, vmask, voffset, hmask, hoffset, !inverted);//产生垂直编码的图像
    }
    else if (_current_pattern<4*_pattern_count+2)//水平条纹
    {   //horizontal
        int bit = _hbits + _pattern_count - _current_pattern/2;
        hmask = 1<<bit;
        //std::cerr << "h# cp: " << _current_pattern << " bit:" << bit << " mask:" << hmask << std::endl;
        _pixmap = make_pattern(rows, cols, vmask, voffset, hmask, hoffset, !inverted);//产生水平编码的图像
    }
    else
    {   //error
        assert(false);
        stop();
        return;
    }
     //_i++;
     //QString ii=(QString("[%1]").arg(_i));
     //QString str="C:/Users/Administrator/Desktop/111/1"+ii+".jpg";

     //_pixmap.save(str);

    //_pixmap.save(QString("pat_%1.png").arg(_current_pattern, 2, 10, QLatin1Char('0')));
}

QPixmap ProjectorWidget::make_pattern(int rows, int cols, int vmask, int voffset, int hmask, int hoffset, int inverted)
{
    QImage image(cols, rows, QImage::Format_ARGB32);

    int tvalue = (inverted ? 0 : 255);
    int fvalue = (inverted ? 255 : 0);

    for (int h=0; h<rows; h++)//#####这里重复了很多次，为了产生投影图案。要改写。要么重写（减少重复次数），要么改写（不懂掩码所用的道理？？）
    {
        uchar * row = image.scanLine(h);
        for (int w=0; w<cols; w++)
        {
            uchar * px = row + (4*w);
            //下面六句话由于没有sl::binaryTOGray函数而暂时注释起来
            int test = (sl::binaryToGray(h+hoffset) & hmask) + (sl::binaryToGray(w+voffset) & vmask);
            int value = (test ? tvalue : fvalue);

            px[0] = value; //B
            px[1] = value; //G
            px[2] = value; //R
            px[3] = 0xff;  //A
        }
    }

    return QPixmap::fromImage(image);
}

bool ProjectorWidget::save_info(QString const& filename) const
{
    FILE * fp = fopen(qPrintable(filename), "w");
    if (!fp)
    {   //failed
        std::cerr << "Projector save_info failed, file: " << qPrintable(filename) << std::endl;
        return false;
    }

    int cols = width();
    int rows = height();

    int effective_width = cols;
    int effective_height = rows;

    int max_vert_value = (1<<std::min(_vbits,_pattern_count));
    while (effective_width>max_vert_value )
    {
        effective_width >>= 1;
    }
    int max_horz_value = (1<<std::min(_hbits,_pattern_count));
    while (effective_height>max_horz_value)
    {
        effective_height >>= 1;
    }

    fprintf(fp, "%u %u\n", effective_width, effective_height);

    fprintf(fp, "\n# width height\n"); //help

    std::cerr << "Saved projetor info: " << qPrintable(filename) << std::endl
              << " - Effective resolution: " << effective_width << "x" << effective_height << std::endl;

    //close
    fclose(fp);
    return true;
}
