#ifndef QFSPIMLIGHTSHEETEVALUATIONEDITOR_H
#define QFSPIMLIGHTSHEETEVALUATIONEDITOR_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include <QProgressDialog>
#include <QTextDocument>
#include "qfrdrimagestackinterface.h"
#include "jkqtplotter.h"
#include "jkqtpimageelements.h"
#include "qffitfunctionmanager.h"
#include "qffitalgorithmmanager.h"
#include "qffitfunctionplottools.h"
#include "qftablemodel.h"

class QFSPIMLightsheetEvaluationItem; // forward

namespace Ui {
    class QFSPIMLightsheetEvaluationEditor;
}

/*! \brief editor for QFSPIMLightsheetEvaluationItem
    \ingroup qf3rdrdp_GROUPNAME
*/
class QFSPIMLightsheetEvaluationEditor : public QFEvaluationEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFSPIMLightsheetEvaluationEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFSPIMLightsheetEvaluationEditor();
    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void resultsChanged();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        void updateStack();

        void on_cmbOrientation_currentIndexChanged(int index);
        void on_cmbModel_currentIndexChanged(int index);
        void on_cmbAlgorithm_currentIndexChanged(int index);
        void on_spinDeltaX_valueChanged(double value);
        void on_spinDeltaZ_valueChanged(double value);
        void on_pltImage_plotMouseClicked(double x, double y, Qt::KeyboardModifiers modifiers=Qt::NoModifier, Qt::MouseButton button=Qt::LeftButton);
        void on_chkErrorsParam_toggled(bool checked);
        void on_chkErrorsBeamPos_toggled(bool checked);
        void on_chkUseMask_toggled(bool checked);
    protected:

        void prepareImagePlotter(JKQtPlotter *pltImage);

        void showImageCut(QFSPIMLightsheetEvaluationItem *eval, QFRawDataRecord* record, QFRDRImageStackInterface *data, int stack, int stack_pos, double x, double y, int channel=-1);


        /** \brief the user interface object (using QtDesigner) */
        Ui::QFSPIMLightsheetEvaluationEditor *ui;

        JKQTPMathImage* plteImage;
        JKQTPxyLineGraph* plteMark;
        double markDataX[2];
        double markDataY[2];
        int c_markDataX;
        int c_markDataY;

        double lastMousePreviewX;
        double lastMousePreviewY;

        QFTableModel* tableFitResults;

        /** \brief progress dialog used by the fits */
        QProgressDialog* dlgEvaluationProgress;
        
        /** \brief where did we save the last report */
        QString currentSaveDirectory;
        
        /*! \brief a flag that indicates whether we are currently updating the display data 
        
            This may be used to distinguish between events emitted by the user, changing something (in a widget, ...) or
            automtically by this class reloading some display ...
        */
        bool updatingData;

        /*! \brief execute the evaulation

            \param record the record to do the evaluation for
        */
        void doEvaluation(QFRawDataRecord* record);
        
        /*! \brief create an evaluation report for the current record */
        void createReportDoc(QTextDocument* document);

        bool* getCurrentMask();

    protected slots:
        /** \brief activated when the highlighted record changed */
        void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);
    
        /** \brief evaluate all files */
        void evaluateAll();
        /** \brief evaluate current file */
        void evaluateCurrent();
        /** \brief display the data from the current raw data record */
        void displayPreview();
        /** \brief display the current evaluation results */
        void displayEvaluationResults();

        void paramChanged(int i);

        void updateFitResultRanges();




};

#endif // QFSPIMLIGHTSHEETEVALUATIONEDITOR_H
