#ifndef QFRDRIMAGINGFCSOVERVIEWIMAGEDISPLAY_H
#define QFRDRIMAGINGFCSOVERVIEWIMAGEDISPLAY_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QToolBar>
#include "qfrawdatarecord.h"
#include "qfrawdataeditor.h"
#include "jkqtplotter.h"
#include "jkqtpelements.h"
#include "jkqtpimageelements.h"
#include "jkqttools.h"
#include "jkqtpgeoelements.h"
#include "qfplayercontrols.h"
#include <QTabWidget>
#include "qfhistogramview.h"

class QFRDRImagingFCSOverviewImageDisplay : public QWidget
{
        Q_OBJECT
    public:
        explicit QFRDRImagingFCSOverviewImageDisplay(QWidget *parent = 0);
        ~QFRDRImagingFCSOverviewImageDisplay();
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

        void calcExpFit();
    public slots:
        /** \brief read the settings */
        virtual void readSettings(QSettings &settings, const QString &prefix=QString("")) ;
        /** \brief write the settings */
        virtual void writeSettings(QSettings &settings, const QString &prefix=QString("")) ;

    protected slots:
        void mouseMoved(double x, double y);

    protected:
        QLabel* labDescription;
        QLabel* labValue;
        QComboBox* cmbImage;
        JKQtPlotter* pltImage;
        QToolBar* toolbar;
        QFHistogramView* histogram;
        QCheckBox* chkHistVideo;

        QTabWidget* tabMain;

        QFPLayerControls* player;

        JKQTPMathImage* image;
        QList<JKQTPgraph*> overlayGraphs;

        QFRawDataRecord* current;

        void createWidgets();

        void clearOverlays();
        
};

#endif // QFRDRIMAGINGFCSOVERVIEWIMAGEDISPLAY_H
