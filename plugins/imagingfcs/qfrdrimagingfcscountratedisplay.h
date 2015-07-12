/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#ifndef QFRDRIMAGINGFCSCOUNTRATEDISPLAY_H
#define QFRDRIMAGINGFCSCOUNTRATEDISPLAY_H

#include <QWidget>
#include "qfrawdatarecord.h"
#include "qfrawdataeditor.h"
#include "qfplotter.h"
#include "jkqtpelements.h"
#include "jkqtpimageelements.h"
#include "jkqttools.h"
#include "jkqtpoverlayelements.h"

namespace Ui {
    class QFRDRImagingFCSCountrateDisplay;
}

/*! \brief editor for count rate and overview images
    \ingroup qf3rdrdp_imaging_fcs

*/
class QFRDRImagingFCSCountrateDisplay : public QWidget {
        Q_OBJECT

    public:
        explicit QFRDRImagingFCSCountrateDisplay(QWidget *parent = 0);
        ~QFRDRImagingFCSCountrateDisplay();

        /** \brief connect widgets to current data record */
    void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) ;
public slots:
        void showFrameIndicator1(double pos);
    protected slots:
        /** \brief connected to the rawDataChanged() signal of the current record */
        void rawDataChanged() ;

        void displayData();



        void calc2ExpFit();
        void calcExpFit();
    public slots:
        /** \brief read the settings */
        virtual void readSettings(QSettings &settings, const QString &prefix=QString("")) ;
        /** \brief write the settings */
        virtual void writeSettings(QSettings &settings, const QString &prefix=QString("")) ;

    private:
        Ui::QFRDRImagingFCSCountrateDisplay *ui;
        JKQTPxyLineErrorGraph* avgGraph;
        JKQTPxyLineGraph* minGraph;
        JKQTPxyLineGraph* maxGraph;
        JKQTPxFunctionLineGraph* avgFit;
        JKQTPoverlayVerticalLine* avgIndicator;
        JKQTPhorizontalRange* rangeGraph;

        QFRawDataRecord* current;

        double* data;
        double* dataT;
        int32_t dataN;
        double expParam[3];
        double exp2Param[5];

        void updateFitFuncPlot();
};

#endif // QFRDRIMAGINGFCSCOUNTRATEDISPLAY_H
