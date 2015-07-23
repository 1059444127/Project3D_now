
#include "CalibrationDialog.hpp"
#include <QDesktopWidget>
#include <QMessageBox>

#include"Application.hpp"
#include <iostream>

CalibrationDialog::CalibrationDialog(QWidget *parent, Qt::WindowFlags flags):
    QDialog(parent, flags),
    _projector(),
    _cancel(false),
    _video_toget(this),
    _i(0),//可删
    timer(),//可删
    _success_open_camera(false),
    _success_read_pattern(false),
    _patterns_to_corner(),
    _num_pictrue(0),//第几张拍下的照片，第一张需要剔除
    _num_position(0),//一共多少个摆姿
    _current_positon(0),//初始化为第一个摆放位置
    _sum_pattern_to_corner(0)//存储所有摆放位置的图
{
    setupUi(this);
    //_patterns_to_corner.resize(42);//设置容器大小为42
    update_screen_combo();//更新所用的屏幕为哪一个，是配置文件保存的，还是所选的
    //update projector view
    _projector.set_screen(screen_combo->currentIndex());//设置要投影在第几屏幕,设置_screen变量的值（索引值：0 or 1 or 2 ...）
    position_spinBox->setValue(1);
    //初始化buf对应的image大小；可修改#######这里采集图片的大小是768*576
    _buf_image=cv::Mat(576,768,CV_8UC3);



    //start video preview
    _success_read_pattern=read_pattern();

     _projector.start();//将widget全屏显示，并且根据此时的屏幕设置bit数###################关键
     _success_open_camera=start_camera();//开启相机预览#####改写//可删


}
CalibrationDialog::~CalibrationDialog()
{

}
bool CalibrationDialog::start_camera(void){


    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QApplication::processEvents();


    _video_toget.start();
    _video_toget.waitForStart();
    connect(&_video_toget, SIGNAL(new_image(unsigned char *)), this, SLOT(_on_new_camera_image(unsigned char *)), Qt::DirectConnection);//是不安全的connect，this所指的对象有事件循环，最好槽函数没有事件循环部分，否则信号发送线程无法出来


    QApplication::restoreOverrideCursor();
    QApplication::processEvents();
    //t.start();//测试时间，多长时间写一张图片；
    return true;

}
bool CalibrationDialog::read_pattern()//^^^^^^^^^^^^^^^^^^^^^^写成打开对话框,读入投影图案
{


    return _projector.read_pattern();


}
int CalibrationDialog::update_screen_combo(void)//更新投影仪屏幕分辨率的选项
{
    //disable signals
    screen_combo->blockSignals(true);//禁止screen_combo接收信号

    //save current value
    int current = screen_combo->currentIndex();//得到现在所选的屏幕

    //update combo
    QStringList list;//存储的是显示器都有哪些可用，及其分辨率大小
    screen_combo->clear();
    QDesktopWidget * desktop = QApplication::desktop();
    int screens =  desktop->screenCount();//统计有多少个桌面，有多少个屏幕可用
    for (int i=0; i<screens; i++)
    {
        const QRect rect = desktop->screenGeometry(i);//屏幕几何大小（分辨率）
        list.append(QString("Screen %1 [%2x%3]").arg(i).arg(rect.width()).arg(rect.height()));//显示屏幕的分辨率Screen 0[1920*1080]
    }
    screen_combo->addItems(list);

    //set current value
    int saved_value = 1;
    int default_screen = (saved_value<screen_combo->count() ? saved_value : 0);//默认的选择屏幕号为0，否则为saved_value
    screen_combo->setCurrentIndex((-1<current && current<screen_combo->count() ? current : default_screen));//从所选的屏幕和默认的屏幕中决定是哪一个

    //enable signals
    screen_combo->blockSignals(false);

    return screen_combo->count();//返回屏幕的个数（总数）
}



void CalibrationDialog::on_synchron_get_button_clicked()
{

    _num_position=position_spinBox->value();
    _current_positon++;
    if(_current_positon==_num_position)
    {
        synchron_get_button->setEnabled(false);
    }
    result_label->setText(QString("the %1 time take picture of %2").arg(_current_positon).arg(_num_position));

    _projector.reset();//重新设置投影仪
   // _success_open_camera=start_camera();//开启相机预览#####改写
    if(_success_open_camera&&_success_read_pattern)
    {
        //按键设为不可用
        screen_combo->setEnabled(0);
        //projector_patterns_spin->setEnabled(0);

        connect(&_projector, SIGNAL(make_new_image(bool)), &_video_toget, SLOT(get_sign(bool)))/*,Qt::DirectConnection)*/;//将_updata标记传入进来到_video_toget线程中

        //open projector

  //      _projector.start();//将widget全屏显示，并且根据此时的屏幕设置bit数###################关键可删
        _projector.next();//使curren_pattern 为0，显示第一张全白色的投影

        timer.start();
    }
    else
    {
        result_label->setText("Fail,cant take pic!");
    }

}


void CalibrationDialog::_on_new_camera_image(unsigned char *lpbuf)//要改
{
/*
    //QTime t;
    //t.start();
    //qDebug( "%c\n", *lpbuf );
    qDebug( "%d\n", timer.elapsed() );
    int bufnum=0;
    int row_num=_buf_image.rows;
    int col_num=_buf_image.cols*_buf_image.channels();//此循环耗时6-7ms
    for(int j=0;j<row_num;j++){
        unsigned char * data = _buf_image.ptr<uchar>(j);
        for(int i=0;i<col_num;i++){
            data[i]=*(lpbuf+bufnum);
            bufnum++;
        }
    }
    ////////////////////////////////////////////////…………………………………………………………………………………………………………cv::mat：_buf_image是要去处理的图像；
    /// 这里在做充足的数据记录之后才能放下一张pattern，&&&&&&在完整版软件中，这里要调用函数传入图片，传入图片这个函数会有一个连接槽，这个槽进行数据计算
    /// 这里暂时先将图片保存下来；
    _i++;
    QString ii=(QString("%1").arg(_i));
    QString str="C:/Users/Administrator/Desktop/111/pattern/capture_result/"+ii+".jpg";
    cv::imwrite(str.toStdString(),_buf_image);//写文件耗时45ms
*/
    //投下一张图片

//    bool isfinish=false;

 //   if (isfinish)
  //  {
//        save_picture(lpbuf);

//    }
 //   else
 //   {
 //       save_picture(lpbuf);
 //       _projector.clear_updated();
 //       isfinish=_projector.next();
 //   }
      save_picture(lpbuf);
      _projector.clear_updated();
      _projector.next();

}

void CalibrationDialog::on_close_button_clicked()
{
    //close projector
    _projector.stop();
    //disconnect projector display signal
    disconnect(&_projector, SIGNAL(make_new_image(bool)), &_video_toget, SLOT(get_sign(bool)));
    if (_video_toget.isRunning())//
    {
        _video_toget.stop();
        _video_toget.wait();//****************加了这句，在单机close时不会出现程序异常终止************
    }
    accept();

}

void CalibrationDialog::save_picture(unsigned char *lpbuf)//要改
{
    //QTime t;
    //t.start();
    //qDebug( "%c\n", *lpbuf );
    _num_pictrue++;//第一章照片剔除掉（由于采集卡的特性）
    if(_num_pictrue==1)
    {
        return;
    }
    else{
        qDebug( "save_picture_time:%d\n", timer.elapsed() );
        int bufnum=0;
        int row_num=_buf_image.rows;
        int col_num=_buf_image.cols*_buf_image.channels();//此循环耗时6-7ms
        for(int j=0;j<row_num;j++){
            unsigned char * data = _buf_image.ptr<uchar>(j);
            for(int i=0;i<col_num;i++){
                data[i]=*(lpbuf+bufnum);
                bufnum++;
            }
           }
    ////////////////////////////////////////////////…………………………………………………………………………………………………………cv::mat：_buf_image是要去处理的图像；
    /// 这里在做充足的数据记录之后才能放下一张pattern，&&&&&&在完整版软件中，这里要调用函数传入图片，传入图片这个函数会有一个连接槽，这个槽进行数据计算
    /// 这里暂时先将图片保存下来；
        _patterns_to_corner.push_back(_buf_image);//存储图像到vector中

        if(_patterns_to_corner.size()==12)//存入图片已满，够12张
          {
            _sum_pattern_to_corner.push_back(_patterns_to_corner);//压入vector中
            _patterns_to_corner.clear();//清空上一次存储的图片

            disconnect(&_projector, SIGNAL(make_new_image(bool)), &_video_toget, SLOT(get_sign(bool)))/*,Qt::DirectConnection)*/;//断开连接，否则两次connect会发射两次信号
            _num_pictrue=0;//计数从零开始
          }
    //将图片存储到本地文件夹中

    _i++;                                                                              //将图片存储到本地文件夹中
    QString ii=(QString("%1").arg(_i));                                                //将图片存储到本地文件夹中
    QString str="C:/Users/Administrator/Desktop/111/pattern/capture_result/"+ii+".jpg";//将图片存储到本地文件夹中
    cv::imwrite(str.toStdString(),_buf_image);//写文件耗时45ms                           //将图片存储到本地文件夹中     */
    }


}

void CalibrationDialog::on_calibrate_button_clicked()
{
    if(_sum_pattern_to_corner.size()>=3)//如果已存储42张图片，那么可以进行下一步，去标定
        APP->calibrate();
    else
        result_label->setText("Fail,dont have enough pic!");


}


