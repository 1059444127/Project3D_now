#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <QApplication>
#include <QList>
#include <QMap>

#include <opencv2/core/core.hpp>

#include "mainwindow.hpp"
#include "CalibrationData.hpp"
#include "scan3d.hpp"

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int & argc, char ** argv);
    ~Application();

//    //data dir
//    void set_root_dir(const QString & dirname);
//    QString get_root_dir(void) const;
//    bool change_root_dir(QWidget * parent_widget = NULL);

//    void clear(void);

//    const cv::Mat get_image(unsigned level, unsigned n, Role role = GrayImageRole) const;
//    int get_camera_width(unsigned level = 0) const;
//    int get_camera_height(unsigned level = 0) const;
//    int get_projector_width(unsigned level = 0) const;
//    int get_projector_height(unsigned level = 0) const;

//    bool extract_chessboard_corners(void);
//    void decode_all(void);
//    void decode(int level, QWidget * parent_widget = NULL);
//    void calibrate(void);

//    bool decode_gray_set(unsigned level, cv::Mat & pattern_image, cv::Mat & min_max_image, QWidget * parent_widget = NULL) const;

//    void load_config(void);

//    //Detection/Decoding/Calibration processing
//    inline void processing_set_current_message(const QString & text) const {processingDialog.set_current_message(text); processEvents();}
//    inline void processing_reset(void) {processingDialog.reset(); processEvents();}
//    inline void processing_set_progress_total(unsigned value) {processingDialog.set_progress_total(value); processEvents();}
//    inline void processing_set_progress_value(unsigned value) {processingDialog.set_progress_value(value); processEvents();}
//    inline void processing_message(const QString & text) const {processingDialog.message(text); processEvents();}
//    inline bool processing_canceled(void) const {return processingDialog.canceled();}

//    //calibration
//    bool load_calibration(QWidget * parent_widget = NULL);
//    bool save_calibration(QWidget * parent_widget = NULL);

//    //reconstruction
//    void reconstruct_model(int level, scan3d::Pointcloud & pointcloud, QWidget * parent_widget = NULL);
//    void compute_normals(scan3d::Pointcloud & pointcloud);

//    void make_pattern_images(int level, cv::Mat & col_image, cv::Mat & row_image);
//    cv::Mat get_projector_view(int level, bool force_update = false);

//    //model
//    void select_none(void);
//    void select_all(void);

//public slots:
//    void deinit(void);

//Q_SIGNALS:
//    void root_dir_changed(const QString & dirname);

public:
 //   QSettings  config;
 //   TreeModel  model;
      MainWindow mainWin;
 //   mutable ProcessingDialog processingDialog;

    CalibrationData calib;

    cv::Size chessboard_size;//棋盘上角点的个数
    cv::Size2f corner_size;//角点的大小
    std::vector<std::vector<cv::Point2f> > chessboard_corners;//检测到的角点位置坐标，原始图像上的
    std::vector<std::vector<cv::Point2f> > projector_corners;//投影仪中角点的编码值（角点在投影仪上的位置）
    std::vector<cv::Mat> pattern_list;//解码后的图像，已经将像素点分类好了；每一个Mat是两通道的矩阵，分别是垂直和水平的（码值的解法？？）编码值
    std::vector<cv::Mat> min_max_list;//每一对图案的灰度值的最大和最小的值
    std::vector<cv::Mat> projector_view_list;
    scan3d::Pointcloud pointcloud;
};

#define APP dynamic_cast<Application *>(Application::instance())













#endif // APPLICATION_HPP
