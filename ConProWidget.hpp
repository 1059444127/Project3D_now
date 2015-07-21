#ifndef CONPROWIDGET_HPP
#define CONPROWIDGET_HPP
#include <QWidget>
#include<Qtime>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
class ConProWidget : public QWidget
{
    Q_OBJECT

public:
    ConProWidget(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    ~ConProWidget();

    void reset(void);
    inline void set_screen(int screen) {_screen = screen;}

    inline int get_current_pattern(void) const {return _current_pattern;}

    //projection cycle
    void start(void);
    void stop(void);
    void prev(void);
    void next(void);
    bool finished(void);
    bool read_pattern(void);
    void clear() { _pixmap = QPixmap(); update(); }
    const QPixmap * pixmap() const {return &_pixmap;}
    void setPixmap(const QPixmap & pixmap) { _pixmap = pixmap; update(); }

    inline bool is_updated(void) const {return _updated;}
    inline void clear_updated(void) {_updated = false;}

    //bool save_info(QString const& filename) const;

signals:
    void make_new_image(bool sign);

protected:
    virtual void paintEvent(QPaintEvent *);

   // void make_pattern(void);
   //void update_pattern_bit_count(void);
   // static QPixmap make_pattern(int rows, int cols, int vmask, int voffset, int hmask, int hoffset, int inverted);

private:
    int _screen;
    QPixmap _pixmap;
    int _current_pattern;
    int _pattern_count;
    int _vbits;
    int _hbits;
    volatile bool _updated;//标记，用于检查是否产生新的投影图案
    int _i;//ceshiyong可删
    std::vector<cv::Mat> patterns;
    QTime timer;//可删
};

#endif // CONPROWIDGET_HPP
