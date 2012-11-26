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
#include "colorcombobox.h"

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



        /** \brief load a mask file as a selection from hard disk */
        void loadMask();
        /** \brief load a mask file as a selection from the clipboard */
        void pasteMask();
        /** \brief save mask to harddisk */
        void saveMask();
        /** \brief copy mask to clipboard */
        void copyMask();
        /** \brief delete mask */
        void includeAll();
        /** \brief invert mask */
        void invertMask();
        /** \brief signal: emitted whenever the mouse was clicked over the plot */
        void plotMouseClicked(double x, double y, Qt::KeyboardModifiers modifiers, Qt::MouseButton button);


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
        ColorComboBox* cmbMaskColor;
        QSpinBox* spinBins;
        QLabel* labTAxis;
        QLabel* labFrame;
        QVisibleHandleSplitter* splitter;
        QAction* actEditMask;
        QAction* actCopyMask;
        QAction* actPasteMask;
        QAction* actInvertMask;
        QAction* actLoadMask;
        QAction* actSaveMask;
        QAction* actClearMask;
        QMenu* menuMask;

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

        /** \brief create widgets on object creation */
        void createWidgets();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        void addDataHistogram(double* data, bool *mask, int size, const QString& title, const QString& colX, const QString& colY, QColor col=QColor("darkblue"), double shift=0, double width=0.9);
};

#endif // QFRDRIMAGESTACKEDITOR_H
