/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFRDRIMAGINGFCSDATAEXPLORER_H
#define QFRDRIMAGINGFCSDATAEXPLORER_H

#include <QDialog>
#include "qfimporterimageseries.h"
#include "qmodernprogresswidget.h"
#include "qfplotter.h"
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



        int getBinning() const;
        bool getUseCropping() const;
        int getCropX0() const;

        uint32_t getFirst() const;
        uint32_t getLast() const;
        bool getUseFirst() const;
        bool getUseLast() const;



        /** \brief initializes the data explorer */
        bool init(QFImporterImageSeries *reader, QFImporterImageSeries *readerRaw, const QString &filename, bool useFirst, uint32_t first, bool useLast, uint32_t last, bool use, int x0, int x1, int y0, int y1, int binning, bool interleavedBinning, bool binAverage);
        int getCropX1() const;
        int getCropY0() const;
        int getCropY1() const;
        bool getInterleavedBinning() const;
        bool getBinAverage() const;
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
        double* fit2;
        uint32_t Nfit;
        uint32_t N;
        uint32_t imgWidth;
        uint32_t imgHeight;
        uint32_t imgRawWidth;
        uint32_t imgRawHeight;
        int64_t currentFrame;

        double statRangedMin;

        float* frameBack;
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
        JKQTPxyLineGraph* avgFit2;
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
        void on_btnFit2_clicked();
        void calcFit();
        void updateRange(bool doReplot=true);
        void nextPlayFrame();
        void on_btnPlay_clicked();
        void on_chkDisplaySD_toggled(bool checked);

};

#endif // QFRDRIMAGINGFCSDATAEXPLORER_H

