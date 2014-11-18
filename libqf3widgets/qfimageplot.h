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

        void setImage(double* image, int32_t width, int32_t height);
        void setMaskAround(int size);
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
        Ui::QFImagePlot *ui;
        QString prefix;
        double* image_data;
        bool* mask;
        int32_t image_width;
        int32_t image_height;
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
        void setImage(const QString& filename, const QString& imageReaderID, int frameNum, double*& image, int& width, int& height, QFImporter::FileInfo *fileinfo=NULL);
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

