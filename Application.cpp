
#include "Application.hpp"

#include <QDir>
//#include <QProgressDialog>
//#include <QMessageBox>
#include <QFileDialog>

#include <cmath>
#include <iostream>
#include <ctime>
#include <cstdlib>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

Application::Application(int & argc, char ** argv) :
    QApplication(argc, argv),
    mainWin(NULL),
    calib(),//标定变量的初始化，初始化图像矩阵数据： cv::Mat cam_K;
                                            // cv::Mat cam_kc;
                                            // cv::Mat proj_K;
                                            // cv::Mat proj_kc;
                                            //cv::Mat R;
                                            // cv::Mat T;
    chessboard_size(11, 7),        //初始化棋盘的大小，横纵分别为多少个角点
    corner_size(21.f, 21.f),        //初始化角点矩形的长度
    chessboard_corners(),           //检测出的棋盘角点的位置的向量
    projector_corners(),            //检测出的（计算出的）投影仪图像中角点的位置的向量
    pattern_list(),                 //一系列的投影图案（原始照片）---或者是pattern images选项中的图像
    min_max_list(),//？？？不知道是哪个图片的list
    projector_view_list(),          //将拍照下的原图像反映射到投影仪中的原始图像
    pointcloud()                    //自定义的类型，可能？？？？没用到的选项
{

    //setup the main window state
    mainWin.show();                 //setVisible(true)主窗口可视
}

Application::~Application()
{
}
