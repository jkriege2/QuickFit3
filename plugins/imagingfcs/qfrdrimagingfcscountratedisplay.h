#ifndef QFRDRIMAGINGFCSCOUNTRATEDISPLAY_H
#define QFRDRIMAGINGFCSCOUNTRATEDISPLAY_H

#include <QWidget>
#include "qfrawdatarecord.h"
#include "qfrawdataeditor.h"
#include "jkqtplotter.h"
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
        JKQTPxyLineGraph* avgFit;
        JKQTPoverlayVerticalLine* avgIndicator;

         QFRawDataRecord* current;
};

#endif // QFRDRIMAGINGFCSCOUNTRATEDISPLAY_H
