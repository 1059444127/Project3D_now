#include"VideoToGet.hpp"
#include<QDebug>
#include <QApplication>
#include <QMetaType>
#include <QTime>

#include <stdio.h>
#include <iostream>
#include"Okapi32.h"
VideoToGet::VideoToGet(QObject  * parent):
    QThread(parent),
    _init(false),
    _stop(false),
    _can_capture(false),
    hBoard(NULL),
    lpbi(NULL),
    lpdib(NULL)

{
    qRegisterMetaType<cv::Mat>("cv::Mat");
}

VideoToGet::~VideoToGet()
{
    //stop_camera();
}

void VideoToGet::run()
{
    _init = false;
    _stop = false;
    bool success = start_camera();

    _init=true;
    if (!success)
    {
        return;
    }


    LPBYTE	lpbuf;

    short	height,width;
    long	stride;
    long no=0;
    int _is_new_picture;
    lpbuf=(unsigned char *)okGetTargetInfo(hBoard,BUFFER,no,&width,&height,&stride);

    //int i=0;ceshi
//    QTime t;
//    t.start();
//    cv::Mat _buf_image(576,768,CV_8UC3);
//    int _i=0;
    while(!_stop)
    {
        while(_can_capture)
        {

            _is_new_picture=okCaptureTo(hBoard,BUFFER,0,0);//第四个参数是0；代表连续采集，即是实时采集，无回调支持

            if(_is_new_picture>0)
            {
                emit new_image(lpbuf);
                _is_new_picture=0;
                _can_capture=false;

            }
        }

    }

}

bool VideoToGet::start_camera(void)//打开相机
{
    long lIndex=-1;
    //LONG form;
    hBoard=okOpenBoard(&lIndex);
    if(hBoard==0)
    return 0;
    //lpbi=(LPBITMAPINFOHEADER)GlobalAlloc(GPTR,768*576*10+5120);//用来后期操作buffer，创建合理的内存空间去复制buffer；or用opencv去取出buffer
   // lpdib=(LPBYTE)lpbi+5120;
    LONG form=okSetCaptureParam(hBoard,CAPTURE_BUFRGBFORMAT,FORM_RGB888);
    if(HIWORD(form)==24&&LOWORD(form)==1)//用来检验设置存储图像到缓存的格式是否正确
        return 1;
    else return 0;

    //okSetSeqCallback(hBoard,BeginCapture,Process,EndCapture);
    //okCaptureTo(hBoard,BUFFER,0,-1);


}

void VideoToGet::waitForStart(void)
{
   while (isRunning() && !_init)
   {
        QApplication::processEvents();
    }
}

void VideoToGet::stop_camera()//#############需要完善…………………………………………………………………………
{
    _stop=true;//#############需要完善关闭图像采集卡………………………………………………………………………………
}

void VideoToGet::get_sign(bool sign)
{
    _can_capture=sign;
}

