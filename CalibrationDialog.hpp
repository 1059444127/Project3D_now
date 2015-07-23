#ifndef CALIBRATIONDIALOG_HPP
#define CALIBRATIONDIALOG_HPP

#include<QDialog>
#include<QTime>
#include"ui_CalibrationDialog.h"

#include "ConProWidget.hpp"
#include "VideoToGet.hpp"


class CalibrationDialog : public QDialog, public Ui::CalibrationDialog
{
    Q_OBJECT

public:
    CalibrationDialog(QWidget * parent = 0, Qt::WindowFlags flags = Qt::WindowMaximizeButtonHint);
    ~CalibrationDialog();
    bool start_camera(void);
    bool read_pattern(void);
    int update_screen_combo(void);
private:
    void save_picture(unsigned char *lpbuf);
private slots:

    void _on_new_camera_image(unsigned char * lpbuf);


    void on_close_button_clicked();

    void on_calibrate_button_clicked();

    void on_synchron_get_button_clicked();

private:
    ConProWidget _projector;
    bool _cancel;
    VideoToGet _video_toget;
    cv::Mat _buf_image;
    int _i;//测试用，删除
    QTime timer;
    bool _success_open_camera;
    bool _success_read_pattern;
    std::vector<cv::Mat> _patterns_to_corner;//存储采集的图案，下一步进行角点检测；
    int _num_pictrue;//获取的是第几张照片，第一章照片要剔除掉
    int _num_position;
    int _current_positon;
    std::vector<std::vector<cv::Mat> > _sum_pattern_to_corner;
};




#endif // CALIBRATIONDIALOG_H
