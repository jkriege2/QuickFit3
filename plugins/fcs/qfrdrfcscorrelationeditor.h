#ifndef QFRDRFCSCorrelationEditor_H
#define QFRDRFCSCorrelationEditor_H

#include "qfrawdataeditor.h"
#include "qt/jkqtplotter.h"
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>
#include <QSplitter>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QAbstractTableModel>


class QFRDRFCSCorrelationEditor : public QFRawDataEditor {
        Q_OBJECT
    protected:
        class runsModel: public QAbstractTableModel {
            public:
                runsModel(QObject* parent=NULL);
                void setCurrent(QFRawDataRecord* current);
                virtual QVariant data(const QModelIndex &index, int role) const;
                virtual Qt::ItemFlags flags(const QModelIndex &index) const;
                virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
                virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
                virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
            protected:
                QFRawDataRecord* current;
        };

        runsModel runs;
    public:
        /** Default constructor */
        QFRDRFCSCorrelationEditor(QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRFCSCorrelationEditor();
    protected slots:
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();
        /** \brief displays the data from the current data element in the plotter widget
         *
         * the parameter dummy has no function it is only there so this function may be used
         * as a slot for a signal with an integer parameter.
         */
        void replotData(int dummy=0);
        /** \brief excludes the currently selected items from the evaluation and recalculates the average */
        void excludeRuns();
        /** \brief includes the currently selected items from the evaluation and recalculates the average */
        void includeRuns();
        void selectionChanged(const QModelIndex & current, const QModelIndex & previous );
        void selectionChanged(const QItemSelection & current, const QItemSelection & previous );
        void runsModeChanged(int c);
        void slidersChanged(int userMin, int userMax, int min, int max);
    protected:
        /** \brief create widgets on object creation */
        void createWidgets();
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        /** \brief plotter widget for the correlation curve */
        JKQtPlotter* plotter;
        /** \brief sliders to set the cut of the correlation curve */
        //DataCutSliders* sliders;
        /** \brief splitter between plot and other widgets */
        QSplitter* splitter;
        /** \brief a listwidget to display only a subset of runs */
        QListView* lstRunsSelect;
        /** \brief a combobox to select how the error of the average run are displayed */
        QComboBox* cmbAverageErrors;
        /** \brief a button which excludes the selected runs from the evaluation */
        QPushButton* btnDontUse;
        /** \brief a button which includes the selected runs to the evaluation */
        QPushButton* btnUse;
        /** \brief checkbox to select log tau-axis */
        QCheckBox* chkLogTauAxis;
        /** \brief combobox to select the run display mode */
        QComboBox* cmbRunDisplay;
        /** \brief group box for data about the correlation functions */
        QGroupBox* grpInfo;
        /** \brief label for the number of runs in the file */
        QLabel* labRuns;
        /** \brief label for the number of points in the correlation function */
        QLabel* labCorrelationPoints;
    private:
};

#endif // QFRDRFCSCorrelationEditor_H
