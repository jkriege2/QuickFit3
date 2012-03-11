#ifndef QFRDRIMAGINGFCSDATAEXPLORER_H
#define QFRDRIMAGINGFCSDATAEXPLORER_H

#include <QDialog>
#include "qfimporterimageseries.h"
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

        void readSettings(QSettings& settings, QString prefix=QString(""));
        void writeSettings(QSettings& settings, QString prefix=QString(""));

        void setBleachDecay(double frames);
        double getBleachDecay() const;
        void setBleachA(double A);
        double getBleachA() const;
        void setBleachB(double B);
        double getBleachB() const;

        int getBinning() const;
        bool getUseCropping() const;
        int getCropX0() const;

        uint32_t getFirst() const;
        uint32_t getLast() const;
        bool getUseFirst() const;
        bool getUseLast() const;



        /** \brief initializes the data explorer */
        bool init(QFImporterImageSeries *reader, QFImporterImageSeries *readerRaw, const QString &filename, bool useFirst, uint32_t first, bool useLast, uint32_t last, bool use, int x0, int x1, int y0, int y1, int binning);
        int getCropX1() const;
        int getCropY0() const;
        int getCropY1() const;
protected:
        Ui::QFRDRImagingFCSDataExplorer *ui;
        QFImporterImageSeries* reader;
        QFImporterImageSeries* readerRaw;
        QString filename;
        uint32_t frames;
        QString fileFormat;

        double* avg;
        double* stddev;
        double* fit;
        uint32_t Nfit;
        uint32_t N;
        uint32_t imgWidth;
        uint32_t imgHeight;
        uint32_t imgRawWidth;
        uint32_t imgRawHeight;
        int64_t currentFrame;

        float* frame;
        float* frameRaw;

        int playDelay;

        bool initing;

        JKQTPMathImage* image;
        JKQTPMathImage* imageRaw;
        JKQTPgeoRectangle* cropRegion;
        JKQTPgeoRectangle* binRegion;
        JKQTPxyLineErrorGraph* avgGraph;
        JKQTPxyLineGraph* avgFit;
        JKQTPverticalRange* fitRange;

        void reallocStatistics(uint32_t N);
        void reallocFrames();

        void readFrames(bool next=true);
        void skipFrames(int N);

        bool readStatistics(QModernProgressDialog* prg=NULL);
        void calcAvgFrontBack(double &avgFront, double &avgEnd);
        void getDataRange(uint32_t& start, uint32_t& end);

    protected slots:
        void on_btnNext_clicked();
        void on_btnNext100_clicked();
        void on_btnFirst_clicked();
        void rereadFrame();
        void on_btnFit_clicked();
        void calcFit();
        void updateRange(bool doReplot=true);
        void nextPlayFrame();
        void on_btnPlay_clicked();

};

#endif // QFRDRIMAGINGFCSDATAEXPLORER_H

