
#include "CaptureDialog.hpp"
#include <QDesktopWidget>
#include <QMessageBox>
#include<QTime>

#include <iostream>

CaptureDialog::CaptureDialog(QWidget * parent, Qt::WindowFlags flags): 
    QDialog(parent, flags),
    _projector(),
    _cancel(false),
    _video_input(this),
    _i(0),
    timer()
{
    setupUi(this);
    //部件设为不可见
    current_message_label->setVisible(false);
    progress_label->setVisible(false);
    progress_bar->setVisible(false);
    //test buttons
    test_prev_button->setEnabled(false);
    test_next_button->setEnabled(false);

    projector_patterns_spin->setValue(10);

    update_screen_combo();//更新所用的屏幕为哪一个，是配置文件保存的，还是所选的
    //update projector view
    _projector.set_screen(screen_combo->currentIndex());//设置要投影在第几屏幕,设置_screen变量的值（索引值：0 or 1 or 2 ...）
\
    //初始化buf对应的image大小；可修改#######这里采集图片的大小是768*576
    _buf_image=cv::Mat(576,768,CV_8UC3);



    //start video preview
    start_camera();//开启相机预览#####改写
}

CaptureDialog::~CaptureDialog()
{
}


void CaptureDialog::on_close_cancel_button_clicked()
{
    if (_video_input.isRunning())//#############需要完善
    {
        _video_input.stop();//#############需要完善
        _video_input.wait();//#############需要完善****************加了这句，在单机close时不会出现程序异常终止************
    }//#############需要完善

    if (close_cancel_button->text()=="Close")
    {
        accept();//是对话框的一个槽，执行这个槽，会是模态对话框隐藏，并返回一个值；看书《快速入门》P54
    }
    else if (!_cancel)
    {
        _cancel = true;
    }
}

bool CaptureDialog::start_camera(void){


    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QApplication::processEvents();


    _video_input.start();
    _video_input.waitForStart();

    //connect display signal now that we know it worked
    connect(&_video_input, SIGNAL(new_image(unsigned char *)), this, SLOT(_on_new_camera_image(unsigned char *)), Qt::DirectConnection);//是不安全的connect，this所指的对象有事件循环，最好槽函数没有事件循环部分，否则信号发送线程无法出来

    QApplication::restoreOverrideCursor();
    QApplication::processEvents();
    //t.start();//测试时间，多长时间写一张图片；
    return true;

}
void CaptureDialog::_on_new_camera_image(unsigned char *lpbuf)//##################此函数只用于显示，因为为了显示，故意加了延时50ms
{

    //QTime t;
    //t.start();
    //qDebug( "%c\n", *lpbuf );
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
    camera_image->setImage(_buf_image);////////////////////////////////////////////////_buf_image是要去处理的图像；
    //qDebug( "%d\n", t.elapsed() );
    //update();
    //存储所抓取的每一张图片；不成熟的代码，要改#######################
    _i++;
    QString ii=(QString("[%1]").arg(_i));
    QString str="C:/Users/Administrator/Desktop/111/1"+ii+".jpg";
    //cv::imwrite(str.toStdString(),_buf_image);//写文件耗时45ms

    //1)加了一段延时函数之后绘制图像流畅；延时50ms######
     timer.start();
     while (timer.elapsed()<50)
     {
         QApplication::processEvents();
         //qDebug( "%d\n", t.elapsed() );
     }
    //1)加了一段延时函数之后绘制图像流畅；

    //qDebug( "%d\n", t.elapsed() );



}

void CaptureDialog::on_test_check_stateChanged(int state)
{
    //adjust the GUI
    bool checked = (state==Qt::Checked);
    test_prev_button->setEnabled(checked);
    test_next_button->setEnabled(checked);
    capture_button->setEnabled(!checked);
    screen_combo->setEnabled(!checked);
    projector_patterns_spin->setEnabled(!checked);

    if (checked)
    {   //start preview

        //connect projector display signal
        connect(&_projector, SIGNAL(new_image(QPixmap)), this, SLOT(_on_new_projector_image(QPixmap)))/*,Qt::DirectConnection)*/;

        //open projector
        _projector.set_pattern_count(projector_patterns_spin->value());
        _projector.start();//将widget全屏显示，并且根据此时的屏幕设置bit数###################关键
        _projector.next();//使curren_pattern 为0，显示第一张全白色的投影
    }
    else
    {   //stop preview

        //close projector
        _projector.stop();

        //disconnect projector display signal
        disconnect(&_projector, SIGNAL(new_image(QPixmap)), this, SLOT(_on_new_projector_image(QPixmap)));
    }
}

void CaptureDialog::on_screen_combo_currentIndexChanged(int index)
{
    _projector.set_screen(index);

}

void CaptureDialog::_on_new_projector_image(QPixmap image)//会让投影的图像显示在imagelabel上，通过paintEvent（）函数
{
    projector_image->setPixmap(image);
    //以下三行代码产生编码图案，保存下来；
     //_i++;
     //QString ii=(QString("[%1]").arg(_i));
     //QString str="C:/Users/Administrator/Desktop/111/c"+ii+".jpg";

     //image.save(str);



}


int CaptureDialog::update_screen_combo(void)//更新投影仪屏幕分辨率的选项
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

void CaptureDialog::on_test_prev_button_clicked(bool checked)
{
    _projector.clear_updated();//先设置产生新投影的标记为false(_updata=false)
    _projector.prev();//在其中会检查_updata是否为true来验证是否产生新的投影，然后再会调用updata()来重绘witdeg

}

void CaptureDialog::on_test_next_button_clicked(bool checked)
{
    _projector.clear_updated();
    _projector.next();
}
