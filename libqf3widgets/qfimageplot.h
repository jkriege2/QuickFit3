#ifndef QFIMAGEPLOT_H
#define QFIMAGEPLOT_H

#include <QWidget>
#include "libwid_imexport.h"
#include <stdint.h>
#include <QPointer>
#include "qfplotter.h"
#include "qfwizard.h"
#include "qfimporter.h"

namespace Ui {
    class QFImagePlot;
}

class QFWIDLIB_EXPORT QFImagePlot : public QWidget
{
        Q_OBJECT
    public:
        explicit QFImagePlot(QWidget *parent = 0, const QString& prefix=QString("QFImagePlot/"));
        ~QFImagePlot();

        void setImage(const double *image, int32_t width, int32_t height);
        void setMaskAround(int size);
        void setROI(double x, double y, double width, double height);
        void setROI(QRectF r);
        void resetROI();
        void setROI2(double x, double y, double width, double height);
        void setROI2(QRectF r);
        void resetROI2();
    public slots:
        void update_plot();
        void clear();
    protected slots:
        void clearData();
        void new_plots();
        void plotMouseMove(double x, double y);
    private:
        QPointer<JKQTPColumnMathImage> plteImage;
        QPointer<JKQTPColumnOverlayImageEnhanced> plteMask;
        QPointer<JKQTPgeoRectangle> plteROI;
        QPointer<JKQTPgeoRectangle> plteROI2;
        Ui::QFImagePlot *ui;
        QString prefix;
        double* image_data;
        bool* mask;
        int32_t image_width;
        int32_t image_height;
        QRectF roi, roi2;
};


class QFWIDLIB_EXPORT QFImagePlotWizardPage : public QFWizardPage
{
        Q_OBJECT
    public:
        explicit QFImagePlotWizardPage(const QString &title, QWidget *parent = 0);
        inline QFImagePlot* getImagePlot() const {
            return plot;
        }
        inline QFormLayout* getFormLayout() const {
            return formLay;
        }

        void addRow(const QString& label, QWidget* widget);
        void addRow(const QString& label, QLayout* layout);

        void setImage(double* image, int32_t width, int32_t height);
        void setImage(const QString& filename, const QString& imageReaderID, int frameNum=0, QFImporter::FileInfo *fileinfo=NULL);
        void setImage(const QString& filename, const QString& imageReaderID, int frameNum, double*& image, int& width, int& height, QFImporter::FileInfo *fileinfo=NULL, int *frames=NULL);
        void setImageAvg(const QString& filename, const QString& imageReaderID, int frameStart, int frameCount, double*& image, int& width, int& height, QFImporter::FileInfo *fileinfo=NULL, int *frames=NULL);
        void setImage(const QString& filename, const QString& imageReaderID,  QFImporter::FileInfo *fileinfo=NULL);
        void setImage(const QString& filename, const QString& imageReaderID,  double*& image, int& width, int& height, QFImporter::FileInfo *fileinfo=NULL);
        void clear();

        virtual void initializePage();
        virtual bool validatePage();
    signals:

    public slots:
    protected:
        QFImagePlot *plot;
        QFormLayout* formLay;


};
#endif // QFIMAGEPLOT_H

