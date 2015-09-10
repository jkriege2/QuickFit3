#ifndef QFETCSPCIMPORTERFRETCHEN2_H
#define QFETCSPCIMPORTERFRETCHEN2_H

#include <QDialog>
#include "qftcspcreader.h"
#include "qfpluginservices.h"
#include "tttrtools.h"
#include "qfplotter.h"
#include "datatools.h"
#include "qcheckablestringlistmodel.h"

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

        void on_btnAppendToMulti_clicked();
        void on_btnSaveToMulti_clicked();
        void on_btnSaveMultiToProject_clicked();
        void on_btnClear_clicked();
        void on_btnDelete_clicked();
        void on_btnDown_clicked();
        void on_btnUp_clicked();

        void on_btnLeftBG_clicked();
        void on_btnLeftBR_clicked();


        void setEditControlsEnabled(bool en);

        void updateFromFile();

        void updateCTRTrace();
        void updateAnalysis();
        void updateBurstSelection();
        void loadTCSPCFiles();
        void updateAnalysisPlots();

        void updateMultiFile();

    protected:
        void calcHistParams(const QVector<double>& PVec, double& pmin, double& pmax, int& nbins, double binw);
        void saveData(const QFDataExportTool& outData, const QFDataExportTool& outDataFiltered);
        void saveData(const QFDataExportTool& outData, const QString &filename, int filterID);
        void saveToProject(const QFDataExportTool& outData, const QFDataExportTool& outDataFiltered, const QString& grp, int coloffset=0);

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

        QList<QFDataExportTool> ms_outData;
        QList<QFDataExportTool> ms_outDataFiltered;
        QCheckableStringListModel ms_model;
        int outDataPCol;
        int outDataECol;


};

#endif // QFETCSPCIMPORTERFRETCHEN2_H
