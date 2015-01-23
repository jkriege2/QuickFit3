#ifndef FFMIMAGEWIDGET_H
#define FFMIMAGEWIDGET_H

#include <QWidget>
#include "qvisiblehandlesplitter.h"
#include "jkqtfastplotter.h"
#include<QtGlobal>
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QPushButton>
#include <QVector>
#include <cmath>
#include "qtlogfile.h"
#include "qfpluginservices.h"


#define IMG_X 500
#define IMG_Y 500
#define IMG_Z 30
struct ImageCoordinates{
    QVector<double> markimg_x;
    QVector<double> markimg_y;
    QVector<int> markimg_z;
};
class FFMImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FFMImageWidget(QWidget *parent = 0);
    ~FFMImageWidget();
    void setLog(QFPluginLogService* log);

    ImageCoordinates* get_marked_coordinates();
    void setImages(uint32_t *red, uint32_t *green, uint32_t *blue, int xsize, int ysize, int zsize);
    //GetMacro(int, plotWidth);
protected:
    QLabel *label;
    QGridLayout *layout;
    JKQTFastPlotter *imgplotter;
    JKQTFPRGBImageOverlayPlot *imgplot;
    bool channel_red;
    bool channel_green;
    bool channel_blue;
    JKQTFPVCrossPlot *tp_markplot;
    JKQTFPVCrossPlot *lp_markplot;
    JKQTFPVCrossPlot *hp_markplot;
    QSlider *z_slider;
    QPushButton *AllPixDeleterButton;
    QPushButton *PixDeleterButton;
    QPushButton *PixelWriterButton;
    QComboBox *ColorChannels;

    QList<uint32_t*> *imgs_red;
    QList<uint32_t*> *imgs_green;
    QList<uint32_t*> *imgs_blue;

    QVector<double> *tp_markimg_x; // marked Pixels of the current plain
    QVector<double> *tp_markimg_y;
    QVector<int> *tp_markimg_z;
    QVector<double> *lp_markimg_x; // marked Pixels in any plain below
    QVector<double> *lp_markimg_y; // i.e. with lower z
    QVector<int> *lp_markimg_z;
    QVector<double> *hp_markimg_x; // marked Pixels in any plain above
    QVector<double> *hp_markimg_y; // i.e. with higher z
    QVector<int> *hp_markimg_z;
    ImageCoordinates coords;      // structure for handling merged coordinates
                                   // of marked pixels

    JKQTFPimagePlot *imgplottest;

    uint32_t *testr;
    uint32_t *testg;
    uint32_t *testb;
    uint32_t *testimage1(uint32_t *pt);
    uint32_t *testimage2(uint32_t *pt);
    uint32_t *testimage3(uint32_t *pt);

    QFPluginLogService* log;
signals:

public slots:
    void markpixel(double xco, double yco);
    void delete_all_pixels();
    void delete_this_plain_pixels();
    void resortpixels(int newplane);
    void change_image(int newplane);
    void channels_chosen(int cbindex);
    // test
    void write_pixel_coords();

private:
    void log_text(QString message);
    void log_warning(QString message);
    void log_error(QString message);
};

#endif // FFMIMAGEWIDGET_H
