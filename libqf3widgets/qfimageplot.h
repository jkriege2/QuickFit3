#ifndef QFIMAGEPLOT_H
#define QFIMAGEPLOT_H

#include <QWidget>
#include "libwid_imexport.h"
#include <stdint.h>
#include <QPointer>
#include "qfplotter.h"

namespace Ui {
    class QFImagePlot;
}

class QFWIDLIB_EXPORT QFImagePlot : public QWidget
{
        Q_OBJECT
    public:
        explicit QFImagePlot(QWidget *parent = 0, const QString& prefix=QString("QFImagePlot/"));
        ~QFImagePlot();

        void setImage(double* image, int32_t width, int32_t height);
    public slots:
        void update_plot();
        void clear();
    protected slots:
        void clearData();
        void new_plots();
        void plotMouseMove(double x, double y);
    private:
        QPointer<JKQTPColumnMathImage> plteImage;
        Ui::QFImagePlot *ui;
        QString prefix;
        double* image_data;
        int32_t image_width;
        int32_t image_height;
};

#endif // QFIMAGEPLOT_H

