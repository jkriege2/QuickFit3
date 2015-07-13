/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFRDRIMAGINGFCSCORRELATIONDIALOG_H
#define QFRDRIMAGINGFCSCORRELATIONDIALOG_H

#include <QDialog>
#include "qfproject.h"
#include "programoptions.h"
#include "qfrdrimagingfcsthreadprogress.h"
#include "qfpluginservices.h"
#include "qfrdrimagingfcsseriesdialog.h"
#include <stdint.h>
#include "qfrdrimagingfcscorrelationjobthread.h"
#include "qfimageplot.h"

struct IMFCSJob; // forward
namespace Ui {
    class QFRDRImagingFCSCorrelationDialog; // forward
}

/*! \brief dialog used to correlate image sequences
    \ingroup qf3rdrdp_imaging_fcs

    This dialog is called by QFRDRImagingFCSPlugin::correlateAndInsert(). It allows the user
    to create several correlation jobs that run in parallel and to wait until they are done.
    Finally getFilesToAdd() returns a list of file constaining ACFs and CCFs to be added to the
    project, by calling QFRDRImagingFCSPlugin::insertVideoCorrelatorFile().
*/
class QFRDRImagingFCSCorrelationDialog : public QDialog {
        Q_OBJECT

    public:
        QFRDRImagingFCSCorrelationDialog(QFPluginServices* pluginservices, ProgramOptions* opt, QWidget *parent = 0);
        ~QFRDRImagingFCSCorrelationDialog();

        void setProject(QFProject* project);

        void writeSettings();
        void readSettings();

        QList<QFRDRImagingFCSCorrelationJobThread::Fileinfo> getFilesToAdd() const;

        static void readStackProperties(const QString &filename, int fileFormat, bool readFiles=true, bool countFrames=true, QWidget* parentWid=NULL, int* channels=NULL, int* frame_count_io=NULL, qint64 *filesize_io=NULL, double *frametime_io=NULL, double *baseline_offset_io=NULL, QString *backgroundF_io=NULL, double *pixel_width_io=NULL, double *pixel_height_io=NULL, bool*hasPixel_io=NULL, int *dualViewMode_io=NULL, int *image_width_io=NULL, int* image_height_io=NULL, QString* inputconfigfile_io=NULL, double** frame_data_io=NULL, int *background_width_io=NULL, int *background_height_io=NULL, int *background_frame_count_io=NULL);
        static void readBackgroundProperties(const QString &filename, int fileFormat, QWidget* parentWid=NULL, int *background_width_io=NULL, int *background_height_io=NULL, int *background_frame_count_io=NULL);

    signals:
        void runSimulation();
    public slots:
        void openFile(const QString& file, const QString& filter=QString());
        void userSelectFile();
        void clickAddJob();
        void clickAddJobSeries(const QString& parameter, double start, double end, double inc);

    protected slots:
        bool correctJobCropAndDV();
        void on_btnSelectImageFile_clicked();
        void on_btnSelectImageFileNoCount_clicked();
        void on_btnSelectBackgroundFile_clicked();
        void on_btnAddJob_clicked();
        void onAddSeriesJobClicked(const QString& parameter=QString(""), double start=-1, double end=-1, double inc=-1);
        void on_btnAddBin12Job_clicked();
        void on_btnLoad_clicked();
        void on_btnLoadNoCount_clicked();
        void on_btnHelp_clicked();
        void on_spinP_valueChanged(int val);
        void on_spinS_valueChanged(int val);
        void on_spinM_valueChanged(int val);
        //void on_spinDecay_valueChanged(double val);
        //void on_spinDecay2_valueChanged(double val);
        void on_cmbCorrelator_currentIndexChanged(int idx);
        void on_cmbBackground_currentIndexChanged(int idx);
        void on_cmbBleachType_currentIndexChanged(int idx);
        void on_cmbDualView_currentIndexChanged(int index);
        void on_chkSeparateColorChannels_toggled(bool value);
        void on_btnSimulate_clicked();

        void on_btnPreview_clicked();
        void previewDestroyed(QObject *object);

        void frameTimeChanged(double value);
        void frameRateChanged(double value);
        void updateProgress();
        void updateFromFile(bool readFiles=true, bool countFrames=true, int* channels=NULL);
        void updateCorrelator(bool setS=false);
        void updateFrameCount();
        void updateImageSize();
        void updateBleach();
        void updateBlocking();
        void startNextWaitingThread();
        void on_chkFirstFrame_clicked(bool checked);
        void on_chkLastFrame_clicked(bool checked);
        void setEditControlsEnabled(bool enabled);
        void on_btnDataExplorer_clicked();
        void readBackgroundFramesize();

        void cropCenter();
        void cropLeftCenter();
        void cropRightCenter();
        void cropTopCenter();
        void cropBottomCenter();
        void cropLeftHalf();
        void cropRightHalf();
        void cropTopHalf();
        void cropBottomHalf();

    protected:
        void done(int status);
        bool allThreadsDone() const;
        int runningThreads() const;
        int waitingThreads() const;
        void ensureTiffReader();
        IMFCSJob initJob(int biningForFCCS=-1);
        void addJob(IMFCSJob jobin, bool ignoreDualView=false);
    private:
        QFPluginServices* pluginServices;
        Ui::QFRDRImagingFCSCorrelationDialog *ui;
        QPointer<QFProject> project;
        ProgramOptions* options;
        QString lastImagefileDir;
        QString lastImagefileFilter;
        QStringList imageFilters;
        QStringList imageFormatNames;
        QList<IMFCSJob> jobs;
        QList<QFRDRImagingFCSCorrelationJobThread::Fileinfo> filesToAdd;
        bool closing;
        QString inputconfigfile;

        QAction* actCropCenter;
        QAction* actCropLeftCenter;
        QAction* actCropRightCenter;
        QAction* actCropTopCenter;
        QAction* actCropBottomCenter;
        QAction* actCropLeftHalf;
        QAction* actCropRightHalf;
        QAction* actCropTopHalf;
        QAction* actCropBottomHalf;


        QPointer<QFImagePlot> image;

        int image_width;
        int image_height;
        double* frame_data;
        int segment_length;
        int32_t frame_count;
        QString filenameDisplayed;

        int background_width;
        int background_height;
        int32_t background_frame_count;

        int getIDForProgress(const QFRDRImagingFCSThreadProgress* w) const;
        int getLayoutIDForProgress(const QWidget* w) const;
};

#endif // QFRDRIMAGINGFCSCORRELATIONDIALOG_H


