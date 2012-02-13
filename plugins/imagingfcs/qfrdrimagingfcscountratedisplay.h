#ifndef QFRDRIMAGINGFCSCOUNTRATEDISPLAY_H
#define QFRDRIMAGINGFCSCOUNTRATEDISPLAY_H

#include <QWidget>
#include "qfrawdatarecord.h"
#include "qfrawdataeditor.h"
#include "qfrdrimagingfcs_data.h"
#include "jkqtplotter.h"
#include "jkqtpelements.h"
#include "jkqtpimageelements.h"
#include "jkqttools.h"

namespace Ui {
    class QFRDRImagingFCSCountrateDisplay;
}

/*! \brief editor for count rate and overview images
    \ingroup qf3rdrdp_imaging_fcs

*/
class QFRDRImagingFCSCountrateDisplay : public QFRawDataEditor {
        Q_OBJECT

    public:
        explicit QFRDRImagingFCSCountrateDisplay(QFPluginServices* services, QWidget *parent = 0);
        ~QFRDRImagingFCSCountrateDisplay();

    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) ;
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged() ;

        void displayData();
        void displayImage();

        void showFrame(int frame);

        void calcFit();
    public slots:
        /** \brief read the settings */
        virtual void readSettings() ;
        /** \brief write the settings */
        virtual void writeSettings() ;

    private:
        Ui::QFRDRImagingFCSCountrateDisplay *ui;
        JKQTPxyLineErrorGraph* avgGraph;
        JKQTPxyLineGraph* minGraph;
        JKQTPxyLineGraph* maxGraph;
        JKQTPxyLineGraph* avgFit;
        JKQTPMathImage* image;
        JKQTPgeoRectangle* cropRegion;
        JKQTPgeoRectangle* imageRegion;
        JKQTPgeoInfiniteLine* avgIndicator;
};

#endif // QFRDRIMAGINGFCSCOUNTRATEDISPLAY_H
