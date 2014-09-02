/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#ifndef QFETCSPCImporterDIALOG_H
#define QFETCSPCImporterDIALOG_H

#include <QDialog>
#include "qfproject.h"
#include "programoptions.h"
#include "qfetcspcimporterthreadprogress.h"
#include "qfpluginservices.h"
#include <stdint.h>
#include "qftablemodel.h"

class QFETCSPCImporterJobThread; // forward
struct TCSPCImporterJob; // forward
namespace Ui {
    class QFETCSPCImporterDialog; // forward
}

/*! \brief dialog used to correlate image sequences
    \ingroup qf3ext_tcspcimporter

    This dialog is called by QFRDRImagingFCSPlugin::correlateAndInsert(). It allows the user
    to create several correlation jobs that run in parallel and to wait until they are done.
    Finally getFilesToAdd() returns a list of file constaining ACFs and CCFs to be added to the
    project, by calling QFRDRImagingFCSPlugin::insertVideoCorrelatorFile().
*/
class QFETCSPCImporterDialog : public QDialog
{
    Q_OBJECT

public:

    QFETCSPCImporterDialog(QFPluginServices* pluginservices, ProgramOptions* opt, QWidget *parent = 0);
    ~QFETCSPCImporterDialog();

    void setProject(QFProject* project);

    void writeSettings();
    void readSettings();

    QList<QPair<QStringList, QString> > getFilesToAdd() const;

    protected slots:
    void on_spinRangeEnd_valueChanged(double val);
    void on_spinRangeStart_valueChanged(double val);
    void on_btnSelectFile_clicked();
    void on_btnAddJob_clicked();
    void on_btnLoad_clicked();
    void on_btnHelp_clicked();
    void on_spinP_valueChanged(int val);
    void on_spinS_valueChanged(int val);
    void on_spinM_valueChanged(int val);
    void on_spinFCSTauMin_valueChanged(double val);
    void on_spinSegments_valueChanged(int val);
    void on_cmbCorrelator_currentIndexChanged(int idx);

    void updateProgress();
    void updateFromFile(bool readFrameCount=true);
    void updateCorrelator();
    void updateDuration();
    void startNextWaitingThread();
    void on_chkFirstFrame_clicked(bool checked);
    void on_chkLastFrame_clicked(bool checked);
    void setEditControlsEnabled(bool enabled);
protected:
    void done(int status);
    bool allThreadsDone() const;
    int runningThreads() const;
    int waitingThreads() const;
private:
    QFPluginServices* pluginServices;
    Ui::QFETCSPCImporterDialog *ui;
    QPointer<QFProject> project;
    ProgramOptions* options;
    QString lastTCSPCFileDir;
    QString lastTCSPCFileFilter;
    QStringList tcspcFilters;
    QStringList tcspcFormatNames;
    QList<TCSPCImporterJob> jobs;
    QFTableModel* tmCR;
    QFTableModel* tmFCS;
    //QList<Job> jobsToAdd;
    QList<QPair<QStringList, QString> > filesToAdd;
    QString countRateString;
    bool closing;
    double duration;
    int channels;

    int getIDForProgress(const QFETCSPCImporterThreadProgress* w) const;
    int getLayoutIDForProgress(const QWidget* w) const;
};

#endif // QFETCSPCImporterDIALOG_H


