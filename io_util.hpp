#ifndef IO_UTIL_H
#define IO_UTIL_H

#include <QImage>
#include <opencv2/core/core.hpp>
namespace io_util
{


    QImage qImage(const cv::Mat & image);
    QImage qImageFromRGB(const cv::Mat & image);
    QImage qImageFromGray(const cv::Mat & image);

};
#endif // IO_UTIL_H
