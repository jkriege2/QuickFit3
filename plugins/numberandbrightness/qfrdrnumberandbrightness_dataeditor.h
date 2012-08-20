#ifndef QFRDRNUMBERANDBRIGHTNESSEDITOR_H
#define QFRDRNUMBERANDBRIGHTNESSEDITOR_H

#include "qfrawdataeditor.h"
#include <QLabel>
#include "qt/jkqtplotter.h"
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>
#include "qvisiblehandlesplitter.h"
#include <QSplitter>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QAbstractTableModel>
#include "datacutslider.h"
#include "qt/jkqtfastplotter.h"
#include "jkqtptools.h"

class QFRDRNumberAndBrightnessData; //forward

/*! \brief editor for QFRawDataRecord
    \ingroup qf3rdrdp_number_and_brightness
*/
class QFRDRNumberAndBrightnessDataEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRNumberAndBrightnessDataEditor(QFPluginServices* services,  QFRawDataPropertyEditor* propEditor, QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRNumberAndBrightnessDataEditor();
    protected slots:
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);
        /** \brief displays the data from the current data element in the plotter widget
         *
         * the parameter dummy has no function it is only there so this function may be used
         * as a slot for a signal with an integer parameter.
         */
        void replotData();

        void imageZoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter *sender);
    protected:
        /** \brief create widgets on object creation */
        void createWidgets();

        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        void addPlotter( JKQtPlotter*& plotter, JKQTPMathImage*& plot, JKQTPOverlayImage*& plteSelected, JKQTPOverlayImage*& plteExcluded);

        void reallocMem(int size);


        QColor selectionColor;
        QColor excludedColor;


        /** \brief plotter widget for the brightness image */
        JKQtPlotter* pltBrightness;
        /** \brief plotter widget for the particle number image */
        JKQtPlotter* pltNumber;
        /** \brief plotter widget for the overview image */
        JKQtPlotter* pltOverview;
        /** \brief plotter widget for the overview image */
        JKQtPlotter* pltCorrelation;

        JKQTPMathImage* plteNumber;
        JKQTPOverlayImage* plteNumberSelected;
        JKQTPOverlayImage* plteNumberExcluded;

        JKQTPMathImage* plteBrightness;
        JKQTPOverlayImage* plteBrightnessSelected;
        JKQTPOverlayImage* plteBrightnessExcluded;

        JKQTPMathImage* plteOverview;
        JKQTPOverlayImage* plteOverviewSelected;
        JKQTPOverlayImage* plteOverviewExcluded;

        JKQTPxyLineGraph* plteCorrelation;



        double* plteNumberData;
        double* plteBrightnessData;
        double* plteOverviewData;
        double* plteCorrelationData;

        /** \brief data in plteOverviewSelected */
        bool* plteOverviewSelectedData;
        /** \brief data in plteOverviewExcluded */
        bool* plteOverviewExcludedData;
        /** \brief size of plteOverviewSelectedData */
        int plotsSize;
};

#endif // QFRDRNUMBERANDBRIGHTNESSEDITOR_H
