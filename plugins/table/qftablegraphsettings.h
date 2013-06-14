#ifndef QFTABLEGRAPHSETTINGS_H
#define QFTABLEGRAPHSETTINGS_H

#include <QWidget>
#include "qfrawdatapropertyeditor.h"
#include "qfrawdatarecord.h"
#include <cmath>
#include "tools.h"
#include "qfrdrtable.h"
#include "QToolTip"
#include "qfmathparser.h"
#include "qffunctionreferencetool.h"
#include "qffitfunctionvalueinputtable.h"

namespace Ui {
    class QFTableGraphSettings;
}

class QFTableGraphSettings : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFTableGraphSettings(QWidget *parent = 0);
        ~QFTableGraphSettings();
        void setRecord(QFRDRTable* record, int plot);

        void rawDataChanged();

        /** \brief read the settings */
        virtual void readSettings(QSettings& settings, const QString& prefix=QString(""));
        /** \brief write the settings */
        virtual void writeSettings(QSettings& settings, const QString& prefix=QString(""));

        void writeGraphData(QFRDRTable::GraphInfo &graph);
        void loadGraphData(const QFRDRTable::GraphInfo &graph);
        void updatePlotWidgetVisibility();
        QIcon getGraphIcon(int i) const;
    signals:
        void graphDataChanged();
        void performFit(int xCol, int yCol, int sigmaCol, int plot, QString function);
        void performRegression(int xCol, int yCol, int sigmaCol, int  plot);
    public slots:
        void initFocus();

    protected slots:
        void on_btnFunctionHelp_clicked();
        void on_edtFunction_textChanged(const QString &text);
        void cmbFunctionTypeCurrentIndexChanged(int index);

        void writeGraphData();
        void reloadColumns(QComboBox* combo);

        void connectWidgets();
        void disconnectWidgets();

        void doFit();
        void doRegression();

        void updateComboboxes();
    protected:

    private:
        QFTableModelColumnHeaderModel* headerModel;
        //QStringListModel* headerModel;
        Ui::QFTableGraphSettings *ui;
        QFRDRTable* current;
        int plot;
        bool updating;
        QFFunctionReferenceTool* functionRef;

        QFFitFunctionValueInputTable* fitfuncValuesTable;
        QVector<double> fitfuncValues, fitfuncValuesDummy;

        QAction* actFit;
        QAction* actRegression;
};

#endif // QFTABLEGRAPHSETTINGS_H
