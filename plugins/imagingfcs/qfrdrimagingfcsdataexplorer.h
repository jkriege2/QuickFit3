#ifndef QFRDRIMAGINGFCSDATAEXPLORER_H
#define QFRDRIMAGINGFCSDATAEXPLORER_H

#include <QDialog>
#include "qfrdrimagereader.h"
#include "qmodernprogresswidget.h"
#include "jkqtplotter.h"
#include "jkqtpimageelements.h"
#include "jkqtpgeoelements.h"
#include "jkqtpelements.h"

namespace Ui {
    class QFRDRImagingFCSDataExplorer;
}

/*! \brief A image series explorer dialog for the imagingFCS plugin (used by the auto-correlator dialog)
    \ingroup qf3rdrdp_imaging_fcs
*/
class QFRDRImagingFCSDataExplorer : public QDialog{
        Q_OBJECT
        
    public:
        explicit QFRDRImagingFCSDataExplorer(QWidget *parent = 0);
        ~QFRDRImagingFCSDataExplorer();

        void setBleachDecay(double frames);
        double getBleachDecay() const;

        int getBinning() const;
        bool getUseCropping() const;
        int getCropX0() const;



        /** \brief initializes the data explorer */
        bool init(QFRDRImageReader *reader, QFRDRImageReader *readerRaw, const QString &filename, bool use, int x0, int x1, int y0, int y1, int binning);
        int getCropX1() const;
        int getCropY0() const;
        int getCropY1() const;
protected:
        Ui::QFRDRImagingFCSDataExplorer *ui;
        QFRDRImageReader* reader;
        QFRDRImageReader* readerRaw;
        QString filename;
        uint32_t frames;
        QString fileFormat;

        double* avg;
        double* stddev;
        uint32_t N;
        uint32_t imgWidth;
        uint32_t imgHeight;
        uint32_t imgRawWidth;
        uint32_t imgRawHeight;
        int64_t currentFrame;

        float* frame;
        float* frameRaw;

        JKQTPMathImage* image;
        JKQTPMathImage* imageRaw;
        JKQTPgeoRectangle* cropRegion;
        JKQTPgeoRectangle* binRegion;
        JKQTPxyLineErrorGraph* avgGraph;
        JKQTPxyLineGraph* avgFit;

        void reallocStatistics(uint32_t N);
        void reallocFrames();

        void readFrames(bool next=true);

        bool readStatistics(QModernProgressDialog* prg=NULL);

    protected slots:
        void on_btnNext_clicked();
        void on_btnFirst_clicked();
        void rereadFrame();
};

#endif // QFRDRIMAGINGFCSDATAEXPLORER_H
