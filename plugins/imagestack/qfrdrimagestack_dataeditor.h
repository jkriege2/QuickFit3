#ifndef QFRDRIMAGESTACKEDITOR_H
#define QFRDRIMAGESTACKEDITOR_H

#include "qfrawdataeditor.h"
#include <QLabel>
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
#include "qvisiblehandlesplitter.h"
#include "qftools.h"
#include "statistics_tools.h"

/*! \brief editor for QFRawDataRecord
    \ingroup qf3rdrdp_image_stack
*/
class QFRDRImageStackDataEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRImageStackDataEditor(QFPluginServices* services, QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRImageStackDataEditor();
    protected slots:
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);

        void showFrame(int frame, bool startPlayer=true);

        void displayImage();
        void stackChanged();
        void channelModeChanged();


    signals:
        void displayedFrame(double time);
    protected slots:

    protected:
        QLabel* labDescription;
        JKQtPlotter* pltImage;
        JKQtPlotter* pltData;
        QToolBar* toolbar;
        QComboBox* cmbImageStack;
        QComboBox* cmbChannelR;
        QComboBox* cmbChannelG;
        QComboBox* cmbChannelB;
        QComboBox* cmbChannelMode;
        QSpinBox* spinBins;
        QLabel* labTAxis;
        QLabel* labFrame;
        QVisibleHandleSplitter* splitter;

        //JKQTPbarHorizontalGraph* plteHistogram;
        //JKQTPxyLineGraph* plteLine;


        enum DataMode {
            dmFullHistogram=0,
            dmLineProfile=1,
            dmSelectionHistogram=2
        };

        DataMode dataMode;


        QFPLayerControls* player;

        JKQTPMathImage* image;
        JKQTPRGBMathImage* imageRGB;
        /** \brief create widgets on object creation */
        void createWidgets();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        void addDataHistogram(double* data, int size, const QString& title, const QString& colX, const QString& colY, QColor col=QColor("darkblue"), double shift=0, double width=0.9);
};

#endif // QFRDRIMAGESTACKEDITOR_H
