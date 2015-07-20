
#ifndef __CAPTURENDIALOG_HPP__
#define __CAPTURENDIALOG_HPP__

#include <QDialog>



#include "ui_CaptureDialog.h"

#include "ProjectorWidget.hpp"
#include "VideoInput.hpp"


class CaptureDialog : public QDialog, public Ui::CaptureDialog
{
    Q_OBJECT

public:
    CaptureDialog(QWidget * parent = 0, Qt::WindowFlags flags = Qt::WindowMaximizeButtonHint);
    ~CaptureDialog();
    bool start_camera(void);
    int update_screen_combo(void);

private slots:
    void on_close_cancel_button_clicked();
    void _on_new_camera_image(unsigned char * lpbuf);
    void on_test_check_stateChanged(int arg1);


    void on_test_prev_button_clicked();

    void on_test_next_button_clicked();

public slots:
    void on_screen_combo_currentIndexChanged(int index);
    void _on_new_projector_image(QPixmap image);


private:

    ProjectorWidget _projector;
    bool _cancel;
    VideoInput _video_input;
    cv::Mat _buf_image;
    int _i;//测试用，删除
    QTime timer;

};

#endif  /* __CAPTURENDIALOG_HPP__ */
