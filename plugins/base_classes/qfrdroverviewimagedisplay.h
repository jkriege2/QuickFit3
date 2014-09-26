/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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



#ifndef QFRDROVERVIEWIMAGEDISPLAY_H
#define QFRDROVERVIEWIMAGEDISPLAY_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QToolBar>
#include "qfrawdatarecord.h"
#include "qfrawdataeditor.h"
#include "qfplotter.h"
#include "jkqtpelements.h"
#include "jkqtpimageelements.h"
#include "jkqttools.h"
#include "jkqtpgeoelements.h"
#include "qfplayercontrols.h"
#include <QTabWidget>
#include "qfhistogramview.h"

class QFRDROverviewImageDisplay : public QWidget
{
        Q_OBJECT
    public:
        explicit QFRDROverviewImageDisplay(QWidget *parent = 0);
        ~QFRDROverviewImageDisplay();
        /** \brief connect widgets to current data record */
        void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) ;

    signals:
        void displayedFrame(double time);
    protected slots:
        /** \brief connected to the rawDataChanged() signal of the current record */
        void rawDataChanged() ;

        void showFrame(int frame);

        void displayImage();

        void showHistograms(double* data, int size);
        /** \brief replot the selection displays only */
        void replotSelection(bool replot=true);

        void setImageEditMode();
        void updateSelectionArrays();
    public slots:
        /** \brief read the settings */
        virtual void readSettings(QSettings &settings, const QString &prefix=QString("")) ;
        /** \brief write the settings */
        virtual void writeSettings(QSettings &settings, const QString &prefix=QString("")) ;

    protected slots:
        void mouseMoved(double x, double y);
        void imageClicked(double x, double y, Qt::KeyboardModifiers modifiers);
        void imageScribbled(double x, double y, Qt::KeyboardModifiers modifiers, bool first, bool last);
        void imageRectangleFinished(double x, double y, double width, double height, Qt::KeyboardModifiers modifiers);
        void imageLineFinished(double x1, double y1, double x2, double y2, Qt::KeyboardModifiers modifiers);
        void imageCircleFinished(double x, double y, double radius, Qt::KeyboardModifiers modifiers);
        void imageEllipseFinished(double x, double y, double radiusX, double radiusY, Qt::KeyboardModifiers modifiers);
    protected:
        QLabel* labDescription;
        QLabel* labValue;
        QLabel* labImageAvg;
        QLabel* labStackPosition;
        QComboBox* cmbImage;
        QFPlotter* pltImage;
        QToolBar* toolbar;
        QFHistogramView* histogram;
        QCheckBox* chkHistVideo;
        QCheckBox* chkRelaxedColorScale;

        QTabWidget* tabMain;

        QFPLayerControls* player;

        JKQTPMathImage* image;
        JKQTPRGBMathImage* imageRGB;
        QList<JKQTPgraph*> overlayGraphs;
        /** \brief plot for the selected runs in pltOverview, plot plteOverviewSelectedData */
        JKQTPOverlayImageEnhanced* plteSelected;

        QFRawDataRecord* current;
        /** \brief set which contains all currently selected runs */
        QSet<int32_t> selected;
        int selected_width;
        int selected_height;
        /** \brief data in plteOverviewSelected */
        bool* plteOverviewSelectedData;
        /** \brief size of plteOverviewSelectedData */
        int32_t plteOverviewSize;


        QAction* actImagesZoom;
        QAction* actImagesDrawPoints;
        QAction* actImagesDrawRectangle;
        QAction* actImagesDrawCircle;
        QAction* actImagesDrawEllipse;
        QAction* actImagesDrawLine;
        QAction* actImagesScribble;
        QActionGroup* agImageSelectionActions;


        void createWidgets();

        void clearOverlays();
        
};

#endif // QFRDROVERVIEWIMAGEDISPLAY_H
