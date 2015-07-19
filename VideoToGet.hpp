#ifndef VIDEOTOGET_HPP
#define VIDEOTOGET_HPP
#include <QThread>
#include <QStringList>

//#include"Okapi32.h"
#include<windows.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class VideoToGet : public QThread
{
    Q_OBJECT

public:
    VideoToGet(QObject * parent = 0);
    ~VideoToGet();

    inline void stop(void) {_stop = true;}

    bool start_camera(void);



    void waitForStart(void);

signals:
    void new_image(unsigned char *);

protected:
    virtual void run();

private:



    void stop_camera(void);

private:

    volatile bool _init;
    volatile bool _stop;



    HANDLE hBoard;
    LPBITMAPINFOHEADER	lpbi;
    LPBYTE				lpdib;
  //  BLOCKINFO	blk;



};
#endif // VIDEOTOGET_H
