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
#include "qt/jkqtfastplotter.h"
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
    signals:
        void currentRunChanged(int run);
        void currentPixelChanged(int x, int y);
        void overviewClicked(double x, double y, Qt::KeyboardModifiers modifiers);
    public slots:
        void setCurrentRun(int run, bool replotAlways=false);
        void setCurrentPixel(int x, int y);
        void setMaskEditable(bool editable);
        void setSelectionEditable(bool editable);

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

        QToolButton* btnEditSlected;
        QAction* actEditSelection;
        QAction* actMaskByImage;



        
};

#endif // QFRDRIMAGETORUNPREVIEW_H
