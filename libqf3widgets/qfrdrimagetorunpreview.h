/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QFRDRIMAGETORUNPREVIEW_H
#define QFRDRIMAGETORUNPREVIEW_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>
#include <QSplitter>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QAbstractTableModel>
#include <QToolButton>
#include "jkqtfastplotter.h"
#include "qfrdrimagetoruninterface.h"
#include "qfrawdatarecord.h"
#include "qfrdrrunselection.h"
#include "qfrdrfcsdatainterface.h"
#include "jkqtplotter.h"
#include "qfplotter.h"
#include <QToolBar>
#include "qfrdrimagemaskedittools.h"
#include "libwid_imexport.h"

class QFWIDLIB_EXPORT QFRDRImageToRunPreview : public QWidget
{
        Q_OBJECT
    public:
        explicit QFRDRImageToRunPreview(QWidget *parent = 0);
        virtual  ~QFRDRImageToRunPreview();
        void setRDR(QFRawDataRecord* record);
        QFRawDataRecord* getRDR() const;

        int getCurrentRun() const;
        void setRunSelectWidgetActive(bool active);

        void draw(JKQTPEnhancedPainter *painter, QSize* size);
        void setDisplayGotoAverageButton(bool display);
    signals:
        void currentRunChanged(int run);
        void currentPixelChanged(int x, int y);
        void overviewClicked(double x, double y, Qt::KeyboardModifiers modifiers);
    public slots:
        void setCurrentRun(int run, bool replotAlways=false);
        void setCurrentPixel(int x, int y);
        void setMaskEditable(bool editable);
        void setSelectionEditable(bool editable);
        void gotoAverageRun();

    protected slots:
        /** \brief draw overview plot */
        void replotOverview();

        void previewClicked(double x, double y, Qt::KeyboardModifiers modifiers, Qt::MouseButton button=Qt::LeftButton);
        void moveColorbarsAuto();

        void excludeByImage();


    protected:
        QFRawDataRecord* record;
        QFRDRImageToRunInterface* rrRecord;
        QFRDRFCSDataInterface* rfcs;

        QToolBar* tbEdit;
        QFRDRImageMaskEditTools* maskEditTools;

        int currentRun;
        int runmax;
        int runmin;
        bool runSelectWidget;

        QGridLayout* gl;
        QLabel* labInfo;
//         /** \brief fast plotter for overview image */
//        JKQTFastPlotter* pltOverview;
//        /** \brief plot for the overview image in pltOverview */
//        JKQTFPimagePlot* plteOverview;
//        /** \brief plot for the 2-channel overview image in pltOverview */
//        JKQTFPRGBImageOverlayPlot* plteOverviewRGB;
//        /** \brief plot for the selected runs in pltOverview, plot plteOverviewSelectedData */
//        JKQTFPimageOverlayPlot* plteOverviewSelected;
//        /** \brief plot for the excluded runs in pltOverview, plot plteOverviewSelectedData */
//        JKQTFPimageOverlayPlot* plteOverviewExcluded;
//         /** \brief fast plotter for overview image */
        QFPlotter* pltOverview;
        /** \brief plot for the overview image in pltOverview */
        JKQTPMathImage* plteOverview;
        /** \brief plot for the 2-channel overview image in pltOverview */
        JKQTPRGBMathImage* plteOverviewRGB;
        /** \brief plot for the selected runs in pltOverview, plot plteOverviewSelectedData */
        JKQTPOverlayImageEnhanced* plteOverviewSelected;
        /** \brief plot for the excluded runs in pltOverview, plot plteOverviewSelectedData */
        JKQTPOverlayImageEnhanced* plteOverviewExcluded;

        JKQTPxyLineGraph* plteCurrentPixel;


        /** \brief data in plteOverviewSelected */
        bool* plteOverviewSelectedData;
        /** \brief data in plteOverviewExcluded */
        bool* plteOverviewExcludedData;
        /** \brief size of plteOverviewSelectedData */
        int plteOverviewSelectedSize;
        int plteOverviewSelectedW;
        int plteOverviewSelectedH;

        QSpinBox* spinRun;
        QLabel* labRun;
        QPushButton* btnAvgRun;
        bool btnAvgRunVisible;

        QToolButton* btnEditSlected;
        QAction* actEditSelection;
        QAction* actMaskByImage;



        
};

#endif // QFRDRIMAGETORUNPREVIEW_H
