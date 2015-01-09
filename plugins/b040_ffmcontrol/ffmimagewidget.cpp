#include "ffmimagewidget.h"
#include "qftools.h"

#define LOG_PREFIX "[ffmimagewidget]: "

FFMImageWidget::FFMImageWidget(QWidget *parent) :
    QWidget(parent)
{
    layout = new QGridLayout(this);
    int img_x = IMG_X;
    int img_y = IMG_Y;
    int img_z = IMG_Z;

    log=NULL;

    // plotter
    imgplotter = new JKQTFastPlotter(this);
    imgplot = NULL;
    imgs_red=NULL;
    imgs_green=NULL;
    imgs_blue=NULL;
    imgplotter->set_xTickDistance(img_x/10);
    imgplotter->set_yTickDistance(img_y/10);
    imgplotter->setXYRange(0.0, (double) (img_x)   , 0.0, (double) (img_y) );
    imgplotter->setToolTip("Click left to mark a position\nfor doing FCS measurements");
    //imgplotter->setMaximumSize((int) 1.5*img_x, (int) img_y);
    imgplotter->setMinimumSize(img_x, img_y);
    imgplotter->set_maintainAspectRatio(true);
    layout->addWidget(imgplotter, 0, 0, 10, 10);

    // make test images
    //begin testpart
    testr = (uint32_t*) qfMalloc(img_x * img_y * img_z * sizeof(uint32_t)); // get memory
    testg = (uint32_t*) qfMalloc(img_x * img_y * img_z * sizeof(uint32_t));
    testb = (uint32_t*) qfMalloc(img_x * img_y * img_z * sizeof(uint32_t));
    testimage1(testr);                                                    // initialize images
    testimage2(testg);
    testimage3(testb);
    ///end testpart
    setImages(testr, testg, testb, img_x, img_y, img_z); // chop image stack memory into images
                                                         // and set pointer lists
    // create image plots
    imgplot = new JKQTFPRGBImageOverlayPlot(imgplotter);
    imgplot->set_height(img_y);
    imgplot->set_width(img_x);
    imgplot->set_xmin(0);
    imgplot->set_ymin(0);
    imgplot->set_xmax(img_x-1);
    imgplot->set_ymax(img_y-1);

    if(imgs_red){
        imgplot->set_imageRed(imgs_red->first(), JKQTFP_uint32);
    }
    if(imgs_green){
        imgplot->set_imageGreen(imgs_green->first(), JKQTFP_uint32);
    }
    if(imgs_blue){
        imgplot->set_imageBlue(imgs_blue->first(), JKQTFP_uint32);
    }

    // create plots for pixel marking
    tp_markimg_x = new QVector<double>(0);
    tp_markimg_y = new QVector<double>(0);
    tp_markimg_z = new QVector<int>(0);
    lp_markimg_x = new QVector<double>(0);
    lp_markimg_y = new QVector<double>(0);
    lp_markimg_z = new QVector<int>(0);
    hp_markimg_x = new QVector<double>(0);
    hp_markimg_y = new QVector<double>(0);
    hp_markimg_z = new QVector<int>(0);
    tp_markplot = new JKQTFPVCrossPlot(imgplotter, tp_markimg_x, tp_markimg_y, QColor("blue"), Qt::SolidLine, 3.0);
    lp_markplot = new JKQTFPVCrossPlot(imgplotter, lp_markimg_x, lp_markimg_y, QColor("black"), Qt::SolidLine, 1.5);
    hp_markplot = new JKQTFPVCrossPlot(imgplotter, hp_markimg_x, hp_markimg_y, QColor("cyan"), Qt::SolidLine, 1.5);
    ;
    // add plots to plotter
    imgplotter->addPlot(imgplot);
    imgplotter->addPlot(tp_markplot);
    imgplotter->addPlot(lp_markplot);
    imgplotter->addPlot(hp_markplot);

    // image control stuff
    channel_red = false;
    channel_green = false;
    channel_blue = false;
    PixDeleterButton = new QPushButton("Unmark this plain");
    PixDeleterButton->setToolTip("Unmark all marked Pixel of the current plain\nand delete their positions");
    layout->addWidget(PixDeleterButton, 0, 11);
    AllPixDeleterButton = new QPushButton("Unmark all plains");
    AllPixDeleterButton->setToolTip("Unmark all marked Pixels\nand delete their positions");
    layout->addWidget(AllPixDeleterButton, 0, 12);
    PixelWriterButton = new QPushButton("Get Pixel Coords:");
    AllPixDeleterButton->setToolTip("Write coordinates of all marked Pixels");
    layout->addWidget(PixelWriterButton, 0, 10);
    z_slider = new QSlider(Qt::Vertical, this);
    z_slider->setMinimum(0);
    z_slider->setMaximum(img_z - 1);
    z_slider->setToolTip("Move through the z-plains\nof your sample");
    layout->addWidget(z_slider, 1, 10, 6, 1);
    ColorChannels = new QComboBox();
    ColorChannels->setInsertPolicy(QComboBox::NoInsert);
    ColorChannels->addItem("RED");        // index 0
    ColorChannels->addItem("GREEN");      // index 1
    ColorChannels->addItem("BLUE");       // index 2
    ColorChannels->addItem("RED&BLUE");   // index 3
    ColorChannels->addItem("BLUE&GREEN"); // index 4
    ColorChannels->addItem("GREEN&RED");  // index 5
    ColorChannels->addItem("FULL RGB");   // index 6
    ColorChannels->setToolTip("Choose the displayed images\nat different wavelngths");
    ColorChannels->setCurrentIndex(2);
    channels_chosen(ColorChannels->currentIndex());
    label = new QLabel("<b>Color Channels:</b>");
    label->setToolTip("Choose the displayed images\nat different wavelngths");
    layout->addWidget(label ,7,10);
    layout->addWidget(ColorChannels, 7, 11 );

    QObject::connect(imgplotter, SIGNAL(clicked(double, double)), this, SLOT(markpixel(double, double)));
    QObject::connect(AllPixDeleterButton, SIGNAL(clicked()), this, SLOT(delete_all_pixels()));
    QObject::connect(PixDeleterButton, SIGNAL(clicked()), this, SLOT(delete_this_plain_pixels()));
    QObject::connect(PixelWriterButton, SIGNAL(clicked()), this, SLOT(write_pixel_coords()));
    QObject::connect(z_slider, SIGNAL(valueChanged(int)), this, SLOT(resortpixels(int)));
    QObject::connect(z_slider, SIGNAL(valueChanged(int)), this, SLOT(change_image(int)));
    QObject::connect(ColorChannels, SIGNAL(currentIndexChanged(int)), this, SLOT(channels_chosen(int)));

}

FFMImageWidget::~FFMImageWidget()
{
    qfFree(testr);
    qfFree(testg);
    qfFree(testb);
}

void FFMImageWidget::setLog(QFPluginLogService *log)
{
    this->log=log;
}



ImageCoordinates* FFMImageWidget::get_marked_coordinates()
{ // return all coordinates marked in the image, without specific order
    int i;
    QVector<double> markimg_x;
    QVector<double> markimg_y;
    QVector<int> markimg_z;
    coords.markimg_x.clear();
    coords.markimg_y.clear();
    coords.markimg_z.clear();
    // merge vectors
    markimg_x << *lp_markimg_x;
    markimg_y << *lp_markimg_y;
    markimg_z << *lp_markimg_z;
    markimg_x << *tp_markimg_x;
    markimg_y << *tp_markimg_y;
    markimg_z << *tp_markimg_z;
    markimg_x << *hp_markimg_x;
    markimg_y << *hp_markimg_y;
    markimg_z << *hp_markimg_z;

    coords.markimg_x = markimg_x;
    coords.markimg_y = markimg_y;
    coords.markimg_z = markimg_z;
    return &coords;
}

/*Splits the data into zslides;
  If a color channel has no input,
  the method requires a "0" instead of a pointer.
*/
void FFMImageWidget::setImages(uint32_t *red, uint32_t *green, uint32_t *blue, int xsize, int ysize, int zsize)
{
    int z=0;
    if(red){
        imgs_red = new QList<uint32_t*>;

        for (z=0; z < zsize ; z++){
            imgs_red->append(&(red[z*xsize*ysize]));
        }
    }
    if(green){
        imgs_green = new QList<uint32_t*>;

        for (z=0; z < zsize ; z++){
            imgs_green->append(&(green[z*xsize*ysize]));
        }
    }
    if(blue){
        imgs_blue = new QList<uint32_t*>;

        for (z=0; z < zsize ; z++){
            imgs_blue->append(&(blue[z*xsize*ysize]));
        }
    }
}

//begin testimage methods

uint32_t *FFMImageWidget::testimage1(uint32_t *pt)
{
    int img_x = IMG_X;
    int img_y = IMG_Y;
    int img_z = IMG_Z;
    uint32_t a = 0;
    int i = 0;
    int width = img_x / 10 +1;
    for (int z=0; z<img_z; z++) {
        for (int y=0; y<img_y; y++) {
            for (int x=0; x<img_x; x++) {
                a=z*img_y*img_x+y*img_x+x;
                if (i < width){
                    pt[a]= (uint32_t) 0;
                } else {
                    pt[a]= (uint32_t) 1000;
                }
                i++;
                if (i>=(width*2) ){
                    i=0;
                }
            }
        }
       // qDebug() << a << " " << i << " " << pt[a];
    }

    return pt;
}

uint32_t *FFMImageWidget::testimage2(uint32_t *pt)
{
    int a=0;
    int img_x = IMG_X;
    int img_y = IMG_Y;
    int img_z = IMG_Z;
    for (int z=0; z<img_z; z++) {
        for (int y=0; y<img_y; y++) {
            for (int x=0; x<img_x; x++) {
                //a=z*img_y*img_x+y*img_x+x;
                pt[a]=(uint32_t) sin(z*M_PI+M_PI/2.0)*1500.0*(abs(x)+abs(y))/(2.0*(double(img_x)+double(img_y)));
                a++;
            }
        }
        //qDebug() << "called:" << a<<"  :  " << pt[a-100] ;
    }
    return pt;
}

uint32_t *FFMImageWidget::testimage3(uint32_t *pt)
{
    uint32_t a = 0;
    int i = 0;
    int img_x = IMG_X;
    int img_y = IMG_Y;
    int img_z = IMG_Z;
    int width = img_x * img_y / 11 +35;
    for (int z=0; z<img_z; z++) {
        for (int y=0; y<img_y; y++) {
            for (int x=0; x<img_x; x++) {
                a=z*img_y*img_x+y*img_x+x;
                if (i < width){
                    pt[a]= (uint32_t) 0;
                } else {
                    pt[a]= (uint32_t) 1000;
                }
                i++;
                if (i>=(width*2)){
                    i=0;
                }
            }
        }
       // qDebug() << a << " " << i << " " << pt[a];
    }
    return pt;
}

// end testimage methods


            void FFMImageWidget::markpixel(double xco, double yco)
{
    if ((xco < IMG_X) && (xco >= 0) && (yco < IMG_Y) && (yco >= 0)){
     tp_markimg_x->append(xco);
     tp_markimg_y->append(yco);
     tp_markimg_z->append(z_slider->value());
     tp_markplot->replot();
    }
}

void FFMImageWidget::delete_all_pixels()
{
    tp_markimg_x->clear();
    tp_markimg_y->clear();
    tp_markimg_z->clear();
    lp_markimg_x->clear();
    lp_markimg_y->clear();
    lp_markimg_z->clear();
    hp_markimg_x->clear();
    hp_markimg_y->clear();
    hp_markimg_z->clear();
    tp_markplot->replot();
    lp_markplot->replot();
    hp_markplot->replot();
}

void FFMImageWidget::delete_this_plain_pixels()
{
    tp_markimg_x->clear();
    tp_markimg_y->clear();
    tp_markimg_z->clear();
    tp_markplot->replot();
}

void FFMImageWidget::resortpixels(int newplane)
{
/*
* redistribute the pixels of each vector group lp
* if necessary, otherwise increment the
* newsize counter n => loop finishes as soon as all pixels
* have been redistributed
*/
    // redistribute old hp
    int n = 0;
    while(hp_markimg_z->size() > n ){
        if (newplane == hp_markimg_z->at(n)){         // hp to tp
            tp_markimg_x->append(hp_markimg_x->at(n));
            hp_markimg_x->remove(n);
            tp_markimg_y->append(hp_markimg_y->at(n));
            hp_markimg_y->remove(n);
            tp_markimg_z->append(hp_markimg_z->at(n));
            hp_markimg_z->remove(n);
        } else if(newplane > hp_markimg_z->at(n)) {   // hp to lp
            lp_markimg_x->append(hp_markimg_x->at(n));
            hp_markimg_x->remove(n);
            lp_markimg_y->append(hp_markimg_y->at(n));
            hp_markimg_y->remove(n);
            lp_markimg_z->append(hp_markimg_z->at(n));
            hp_markimg_z->remove(n);
            } else {                                  // still hp
            n++;
        }
    }
        // redistribute old lp
    n = 0;
    while(lp_markimg_z->size() > n ){
        if (newplane == lp_markimg_z->at(n)){        // lp to tp
            tp_markimg_x->append(lp_markimg_x->at(n));
            lp_markimg_x->remove(n);
            tp_markimg_y->append(lp_markimg_y->at(n));
            lp_markimg_y->remove(n);
            tp_markimg_z->append(lp_markimg_z->at(n));
            lp_markimg_z->remove(n);
        } else if(newplane < lp_markimg_z->at(n)) {  // lp to hp
            hp_markimg_x->append(lp_markimg_x->at(n));
            lp_markimg_x->remove(n);
            hp_markimg_y->append(lp_markimg_y->at(n));
            lp_markimg_y->remove(n);
            hp_markimg_z->append(lp_markimg_z->at(n));
            lp_markimg_z->remove(n);
        } else {                                      // still tp
            n++;
        }
    }
        // redistribute old tp
    n = 0;
    while(tp_markimg_z->size() > n ){
        if (newplane > tp_markimg_z->at(n)){        // tp to lp
            lp_markimg_x->append(tp_markimg_x->at(n));
            tp_markimg_x->remove(n);
            lp_markimg_y->append(tp_markimg_y->at(n));
            tp_markimg_y->remove(n);
            lp_markimg_z->append(tp_markimg_z->at(n));
            tp_markimg_z->remove(n);
        } else if(newplane < tp_markimg_z->at(n)) {  // tp to hp
            hp_markimg_x->append(tp_markimg_x->at(n));
            tp_markimg_x->remove(n);
            hp_markimg_y->append(tp_markimg_y->at(n));
            tp_markimg_y->remove(n);
            hp_markimg_z->append(tp_markimg_z->at(n));
            tp_markimg_z->remove(n);
        } else {                                      // still tp
            n++;
        }
    }
    // replot tp, lp and hp
    tp_markplot->replot();
    lp_markplot->replot();
    hp_markplot->replot();
    imgplotter->update();
}

void FFMImageWidget::change_image(int newplane)
{
    unsigned int img_x = IMG_X;
    unsigned int img_y = IMG_Y;
    if(channel_red){
    imgplot->set_imageRed(imgs_red->at(newplane), JKQTFP_uint32);
    }
    if(channel_green){
    imgplot->set_imageGreen(imgs_green->at(newplane), JKQTFP_uint32);
    }
    if(channel_blue){
    imgplot->set_imageBlue(imgs_blue->at(newplane), JKQTFP_uint32);
    }
    imgplotter->update();
}


void FFMImageWidget::channels_chosen(int cbindex)
{
/*
*  switches between different channel modes and sets the corresponding flags;
*/
    switch (cbindex){
    case 0:
        imgplot->set_imageRed(imgs_red->at(z_slider->value()), JKQTFP_uint32);
        imgplot->set_imageGreen(NULL, JKQTFP_uint32);
        imgplot->set_imageBlue(NULL, JKQTFP_uint32);
        channel_red = true;
        channel_green = false;
        channel_blue = false;
        break;
    case 1:
        imgplot->set_imageRed(NULL, JKQTFP_uint32);
        imgplot->set_imageGreen(imgs_green->at(z_slider->value()), JKQTFP_uint32);
        imgplot->set_imageBlue(NULL, JKQTFP_uint32);
        channel_red = false;
        channel_green = true;
        channel_blue = false;
        break;
    case 2:
        imgplot->set_imageRed(NULL, JKQTFP_uint32);
        imgplot->set_imageGreen(NULL, JKQTFP_uint32);
        imgplot->set_imageBlue(imgs_blue->at(z_slider->value()), JKQTFP_uint32);
        channel_red = false;
        channel_green = false;
        channel_blue = true;
        break;
    case 3:
        imgplot->set_imageRed(imgs_red->at(z_slider->value()), JKQTFP_uint32);
        imgplot->set_imageGreen(NULL, JKQTFP_uint32);
        imgplot->set_imageBlue(imgs_blue->at(z_slider->value()), JKQTFP_uint32);
        channel_red = true;
        channel_green = false;
        channel_blue = true;
        break;
    case 4:
        imgplot->set_imageRed(NULL, JKQTFP_uint32);
        imgplot->set_imageGreen(imgs_green->at(z_slider->value()), JKQTFP_uint32);
        imgplot->set_imageBlue(imgs_blue->at(z_slider->value()), JKQTFP_uint32);
        channel_red = false;
        channel_green = true;
        channel_blue = true;
        break;
    case 5:
        imgplot->set_imageRed(imgs_red->at(z_slider->value()), JKQTFP_uint32);
        imgplot->set_imageGreen(imgs_green->at(z_slider->value()), JKQTFP_uint32);
        imgplot->set_imageBlue(NULL, JKQTFP_uint32);
        channel_red = true;
        channel_green = false;
        channel_blue = true;
        break;
    case 6:
        imgplot->set_imageRed(imgs_red->at(z_slider->value()), JKQTFP_uint32);
        imgplot->set_imageGreen(imgs_green->at(z_slider->value()), JKQTFP_uint32);
        imgplot->set_imageBlue(imgs_blue->at(z_slider->value()), JKQTFP_uint32);
        channel_red = true;
        channel_green = true;
        channel_blue = true;
        break;
    }
    imgplot->replot();
    imgplotter->update();
}

void FFMImageWidget::write_pixel_coords()
{
    log_text("+++++++++++++++++++++++++++++\nMarked PixelCoordinates");
    ImageCoordinates *actCoords = get_marked_coordinates();
    int pixelNumber = actCoords->markimg_x.size();
    for(int i=0; pixelNumber > i; i++){
        log_text(tr("%1: X:%2 Y:%3 Z:%4").arg(i).arg(actCoords->markimg_x.at(i)).arg(actCoords->markimg_y.at(i)).arg(actCoords->markimg_z.at(i)));
    }
    log_text("+++++++++++++++++++++++++++++");
}

void FFMImageWidget::log_text(QString message)
{
    QFPluginServices::getInstance()->log_text(LOG_PREFIX + message + "\n");
}

void FFMImageWidget::log_warning(QString message)
{
    QFPluginServices::getInstance()->log_warning(LOG_PREFIX + message + "\n");
}

void FFMImageWidget::log_error(QString message)
{
    QFPluginServices::getInstance()->log_error(LOG_PREFIX + message + "\n");
}
