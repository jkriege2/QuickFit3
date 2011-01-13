#ifndef QFFCSFITEVALUATIONEDITOR_H
#define QFFCSFITEVALUATIONEDITOR_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include "qt/jkqtplotter.h"
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include "datacutslider.h"


/*! \brief editor class for FCS least-square fits
    \ingroup qf3evalp_fcsfit
*/
class QFFCSFitEvaluationEditor : public QFEvaluationEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFFCSFitEvaluationEditor(QFPluginServices* services, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFFCSFitEvaluationEditor();
    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(const QFEvaluationItem* current, const QFEvaluationItem* old);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void resultsChanged();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();
    protected:
        /** \brief label displaying the current record */
        QLabel* labRecord;
        /** \brief combobox to select a fitting algorithm */
        QComboBox* cmbAlgorithm;
        /** \brief combobox to select a model */
        QComboBox* cmbModel;
        /** \brief combobox to select a model for the weighting */
        QComboBox* cmbWeights;
        /** \brief ploter for the data&curves */
        JKQtPlotter* pltData;
        /** \brief ploter for the residuals */
        JKQtPlotter* pltResiduals;
        /** \brief DataCutSliders */
        DataCutSliders* datacut;
        /** \brief Splitter between Plots and Bottom */
        QSplitter* splitPlot;
        /** \brief pushbutton to configure Fitting Algorithm */
        QPushButton* btnConfigAlgorithm;
        /** \brief pushbutton to show help on Fitting Algorithm */
        QPushButton* btnAlgorithmHelp;
        /** \brief pushbutton to show help on Fitting Algorithm */
        QPushButton* btnModelHelp;


        QMap<QString, QFFitFunction*> fitModels;
    protected slots:
        /** \brief activated when the highlighted record changed */
        void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);

        /** \brief replot curves */
        void replotData();
        /** \brief executed when the sliders values change */
        void slidersChanged(int userMin, int userMax, int min, int max);
    private:
        /** \brief create all widgets on the form */
        void createWidgets();


};

#endif // QFFCSFITEVALUATIONEDITOR_H
