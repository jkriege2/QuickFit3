#ifndef QFETCSPCIMPORTERFRETCHEN2_H
#define QFETCSPCIMPORTERFRETCHEN2_H

#include <QDialog>
#include "qftcspcreader.h"
#include "qfpluginservices.h"
#include "tttrtools.h"
#include "qfplotter.h"
#include "datatools.h"

namespace Ui {
    class QFETCSPCImporterFretchen2;
}


class QFETCSPCImporterFretchen2 : public QDialog
{
        Q_OBJECT

    public:
        explicit QFETCSPCImporterFretchen2(QWidget *parent = 0);
        ~QFETCSPCImporterFretchen2();

        void writeSettings();
        void readSettings();


    protected slots:
        void on_btnHelp_clicked();
        void on_btnClose_clicked();
        void on_btnSelectFile_clicked();
        void on_btnSelectBackground_clicked();
        void on_btnLoad_clicked();
        void on_btnLoadFiles_clicked();
        void on_btnApplyBurstSelection_clicked();
        void on_btnApplyBurstAnalysis_clicked();

        void on_btnSaveToFile_clicked();
        void on_btnSaveToProject_clicked();

        void setEditControlsEnabled(bool en);

        void updateFromFile();

        void updateCTRTrace();
        void updateAnalysis();
        void updateBurstSelection();
        void loadTCSPCFiles();
        void updateAnalysisPlots();

        void calcHistParams(const QVector<double>& PVec, double& pmin, double& pmax, int& nbins, double binw);

    private:
        Ui::QFETCSPCImporterFretchen2 *ui;

        QString lastTCSPCFileDir;
        QString lastTCSPCFileFilter;
        QStringList tcspcFilters;
        QStringList tcspcFormatNames;

        TCSPCPhotonsData photons;
        TCSPCBurstsData bursts;
        JKQTPxyLineGraph* plteInterphotonTimes;
        JKQTPgeoLine* plteIPTLevelLine;

        JKQTPxyLineGraph* plteBurstRate;
        JKQTPxFunctionLineGraph* plteMinBurstRate;
        JKQTPxFunctionLineGraph* plteMaxBurstRate;
        JKQTPbarHorizontalGraph* plteProximity;

        QFDataExportTool outData;
        QFDataExportTool outDataFiltered;
        int outDataPCol;
        int outDataECol;


};

#endif // QFETCSPCIMPORTERFRETCHEN2_H
