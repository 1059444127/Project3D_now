
#include "CalibrationDialog.hpp"
#include <QDesktopWidget>
#include <QMessageBox>

#include <iostream>

CalibrationDialog::CalibrationDialog(QWidget *parent, Qt::WindowFlags flags):
    QDialog(parent, flags),
    _projector(),
    _cancel(false),
    _video_toget(this),
    _i(0),
    timer(),
    _success_open_camera(false),
    _success_read_pattern(false)
{
    setupUi(this);

    //projector_patterns_spin->setValue(10);
    update_screen_combo();//更新所用的屏幕为哪一个，是配置文件保存的，还是所选的
    //update projector view
    _projector.set_screen(screen_combo->currentIndex());//设置要投影在第几屏幕,设置_screen变量的值（索引值：0 or 1 or 2 ...）
\
    //初始化buf对应的image大小；可修改#######这里采集图片的大小是768*576
    _buf_image=cv::Mat(576,768,CV_8UC3);



    //start video preview
    _success_open_camera=start_camera();//开启相机预览#####改写
    _success_read_pattern=read_pattern();

}
CalibrationDialog::~CalibrationDialog()
{

}
bool CalibrationDialog::start_camera(void){


    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QApplication::processEvents();

    _video_toget.start_camera();
    //……………………………………………………以下四句写入button按下中
    //_video_toget.start();
    //_video_toget.waitForStart();

    //connect display signal now that we know it worked
    //connect(&_video_toget, SIGNAL(new_image(unsigned char *)), this, SLOT(_on_new_camera_image(unsigned char *)), Qt::DirectConnection);//是不安全的connect，this所指的对象有事件循环，最好槽函数没有事件循环部分，否则信号发送线程无法出来

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();
    //t.start();//测试时间，多长时间写一张图片；
    return true;

}
bool CalibrationDialog::read_pattern()//^^^^^^^^^^^^^^^^^^^^^^写成打开对话框,读入投影图案
{

    for(int i=1;i<13;i++)
    {
        QString str="C:/Users/Administrator/Desktop/111/pattern/calibration_1024/";
        QString num=(QString("%1").arg(i));
        str=str+num+".jpg";
        patterns.push_back(cv::imread(str.toStdString()));
    }


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



void CalibrationDialog::on_calibrate_button_clicked()
{
    if(_success_open_camera&&_success_read_pattern)
    {
        //按键设为不可用
        screen_combo->setEnabled(0);
        //projector_patterns_spin->setEnabled(0);

        connect(&_projector, SIGNAL(new_image(QPixmap)), this, SLOT(_on_new_projector_image(QPixmap)))/*,Qt::DirectConnection)*/;

        //open projector
        //_projector.set_pattern_count(10);//^^^^^^^^^^^^^^^^^^^^^^这里只用到10个pattern
        _projector.start();//将widget全屏显示，并且根据此时的屏幕设置bit数###################关键
        _projector.next();//使curren_pattern 为0，显示第一张全白色的投影


        _video_toget.start();
        _video_toget.waitForStart();

    //connect display signal now that we know it worked
        connect(&_video_toget, SIGNAL(new_image(unsigned char *)), this, SLOT(_on_new_camera_image(unsigned char *)), Qt::DirectConnection);//是不安全的connect，this所指的对象有事件循环，最好槽函数没有事件循环部分，否则信号发送线程无法出来
    }
    else
        return;//……………………………………………………………………………………这里写上close按下的函数；或者对text输出失败的提示

}
