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
#include "qfplotter.h"
#include "jkqtpelements.h"
#include "jkqtpimageelements.h"
#include "jkqttools.h"
#include "jkqtpgeoelements.h"
#include "qfplayercontrols.h"
#include "qvisiblehandlesplitter.h"
#include "qftools.h"
#include "statistics_tools.h"
#include "colorcombobox.h"
#include "qfrdrimagemaskedittools.h"

/*! \brief editor for QFRawDataRecord
    \ingroup qf3rdrdp_image_stack
*/
class QFRDRImageStackDataEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRImageStackDataEditor(QFPluginServices* services, QFRawDataPropertyEditor* propEditor, QWidget* parent);
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
        QFPlotter* pltImage;
        QFPlotter* pltData;
        QToolBar* toolbar;
        QComboBox* cmbImageStack;
        QComboBox* cmbChannelR;
        QComboBox* cmbChannelG;
        QComboBox* cmbChannelB;
        QComboBox* cmbChannelA;
        JKQTPImageModifierModeComboBox* cmbModifierMode;
        JKQTPMathImageColorPaletteComboBox* cmbColorbar;
        QLabel* labModifierMode;
        QLabel* labColorbar;
        QComboBox* cmbChannelMode;
        ColorComboBox* cmbMaskColor;
        QSpinBox* spinBins;
        QLabel* labTAxis;
        QLabel* labFrame;
        QVisibleHandleSplitter* splitter;
        QMenu* menuMask;
        QLabel* labSelectiondata;

        QString lastMaskDir;

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
        /** \brief plot for the excluded runs in pltOverview, plot plteOverviewSelectedData */
        JKQTPOverlayImageEnhanced* plteOverviewExcluded;
        JKQTPOverlayImageEnhanced* plteOverviewSelected;

        bool* selection;
        int selectionWidth;
        int selectionHeight;

        QFRDRImageMaskEditTools* maskTools;

        /** \brief create widgets on object creation */
        void createWidgets();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        void addDataHistogram(double* data, bool *mask, int size, const QString& title, const QString& colX, const QString& colY, QColor col=QColor("darkblue"), double shift=0, double width=0.9);

        void connectWidgets();
        void disconnectWidgets();
};

#endif // QFRDRIMAGESTACKEDITOR_H
