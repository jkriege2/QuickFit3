#include "qfefcssimscripttab.h"
#include "ui_qfefcssimscripttab.h"
#include <QFileInfo>
#include <QFile>
#include "qftools.h"
#include "programoptions.h"
#include "qfefcssimmainwidnow.h"
#include "qfpluginservices.h"
#include "qfrawdatarecordfactory.h"
#include "qfproject.h"
#include "qfrawdatarecord.h"

QFEFCSSimScriptTab::QFEFCSSimScriptTab(QFEFCSSimMainWidnow *parent) :
    QWidget(parent),
    ui(new Ui::QFEFCSSimScriptTab)
{
    this->mainWin=parent;
    ui->setupUi(this);
    txtChanged=false;

    //setFilename(QFileInfo(ProgramOptions::getConfigValue("QFEFCSSimScriptTab/filedlglastDir",  QDir::homePath()).toString()).filePath()+"/new_script.ini");
    setFilename("");
    ui->grpSim->setEnabled(false);
    ui->grpScript->setEnabled(true);
    ui->progressBar->setValue(0);
    ui->labETA->clear();
    connect (ui->edtScript->getEditor(), SIGNAL(textChanged()), this, SLOT(editorChanged()));
    ProgramOptions::getConfigQSplitter(ui->splitter, "QFEFCSSimScriptTab/splitter");
}

QFEFCSSimScriptTab::~QFEFCSSimScriptTab()
{
    ProgramOptions::setConfigQSplitter(ui->splitter, "QFEFCSSimScriptTab/splitter");
    delete ui;
}

void QFEFCSSimScriptTab::setRunning(bool running)
{
    ui->grpSim->setEnabled(true);
    ui->grpScript->setEnabled(!running);
    if (!running) {
        ui->progressBar->setValue(ui->progressBar->maximum());
        ui->labETA->clear();
    }
    ui->btnImport->setEnabled(resultfiles.size()>0 && !running);
}

bool QFEFCSSimScriptTab::loadFile(const QString &filename)
{
    QString fn=filename;
    if (fn.isEmpty()) {
        fn=qfGetOpenFileNameSet("QFEFCSSimScriptTab/filedlg", this, tr("Open Simulator Script ..."), QFPluginServices::getInstance()->getPluginExamplesDirectory("qfe_fcssimulator")+"/example_configs", tr("Simulator Scripts (*.ini);;All Files (*.*"));
        if (fn.isEmpty()) return false;
    }
    setFilename(fn);
    if (!this->filename.isEmpty()) {
        ui->edtScript->getEditor()->setPlainText(QString::fromLatin1(readFile(this->filename)));
        txtChanged=false;
        emit textChanged(false);
        return true;
    }
    return false;
}

void QFEFCSSimScriptTab::save()
{
    if (filename.isEmpty()) {
        saveAs();
    } else {
        QFile f(filename);
        if (f.open(QFile::WriteOnly|QFile::Text)) {
            f.write(ui->edtScript->getEditor()->toPlainText().toLatin1());
            f.close();
            txtChanged=false;
            emit textChanged(false);
        } else {
            QMessageBox::critical(this, tr("Error saving file"), tr("Could not save file\n  %1\nReason: %2").arg(filename).arg(f.errorString()));
        }
    }
}

bool QFEFCSSimScriptTab::saveAs(const QString &filename)
{
    QString fn=filename;
    if (fn.isEmpty()) {
        fn=qfGetSaveFileNameSet("QFEFCSSimScriptTab/filedlg", this, tr("Save Simulator Script ..."), "", tr("Simulator Scripts (*.ini);;All Files (*.*"));
        if (fn.isEmpty()) return false;
    }
    setFilename(fn);
    QFile f(fn);
    if (f.open(QFile::WriteOnly|QFile::Text)) {
        f.write(ui->edtScript->getEditor()->toPlainText().toLatin1());
        f.close();
        txtChanged=false;
        emit textChanged(false);
        return true;
    } else {
        QMessageBox::critical(this, tr("Error saving file"), tr("Could not save file\n  %1\nReason: %2").arg(fn).arg(f.errorString()));
        return false;
    }
    return false;
}

void QFEFCSSimScriptTab::execute()
{
    if (proc && isRunning()) return;
    if (proc) {
        delete proc;
    }
    save();
    resultfiles.clear();
    ui->progressBar->setValue(0);
    ui->labETA->clear();
    if (mainWin->mayStartProcess()) {
        startProcess();
    } else {
        ui->pteOutput->clearLog();
        ui->pteOutput->log_dline();
        ui->pteOutput->log_text(tr("waiting for a free slot to start simulator ..."));
        ui->pteOutput->log_dline();
        QTimer::singleShot(500, this, SLOT(startProcess()));
    }
    setRunning(true);
}

void QFEFCSSimScriptTab::cancelExecution()
{
    if (!isRunning()) {
        setRunning(false);
        return;
    }
    if (proc ) {
        proc->close();
        proc->deleteLater();
    }
    setRunning(false);

}

bool QFEFCSSimScriptTab::isRunning() const
{
    if (!proc) return false;
    return proc->state()!=QProcess::NotRunning;
}

QString QFEFCSSimScriptTab::getFullFilename() const
{
    return filename;
}

QString QFEFCSSimScriptTab::getFilename() const
{
    return QFileInfo(filename).fileName();
}

bool QFEFCSSimScriptTab::hasChanged() const
{
    return txtChanged;
}

void QFEFCSSimScriptTab::setFilename(const QString &filename)
{
    this->filename=filename;
    ui->labFile->setText(filename);
}

void QFEFCSSimScriptTab::startProcess()
{
    if (mainWin->mayStartProcess()) {
        proc=new QProcess(this);
        proc->setWorkingDirectory(mainWin->getWorkingDir());
        resultfiles.clear();
        proc->setProgram(mainWin->getSimulator());
        QStringList args;
        args<<"--spectra";
        args<<QDir::toNativeSeparators(mainWin->getSpectraDir());
        args<<QDir::toNativeSeparators(filename);
        proc->setArguments(args);
        connect(proc.data(), SIGNAL(readyReadStandardOutput()), this, SLOT(dataAvailable()));
        connect(proc.data(), SIGNAL(readyReadStandardError()), this, SLOT(dataAvailable()));
        connect(proc.data(), SIGNAL(error(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)));
        connect(proc.data(), SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
        ui->pteOutput->clearLog();
        ui->pteOutput->log_dline();
        ui->pteOutput->log_text(tr("running FCS simulator: %1\nin working directory: %2\nwith input file: %3\n\n").arg(proc->program()).arg(proc->workingDirectory()).arg(filename));
        ui->pteOutput->log_dline();
        proc->start(QProcess::ReadOnly);
        ui->pteOutput->log_text("\nstarted simulator process ...\n\n");
    } else {
        QTimer::singleShot(500, this, SLOT(startProcess()));
    }
}

void QFEFCSSimScriptTab::editorChanged()
{
    txtChanged=true;
    emit textChanged(true);
}

void QFEFCSSimScriptTab::dataAvailable()
{
    if (proc) {
        QString dat=proc->readAllStandardOutput();
        ui->pteOutput->log_text(dat);
        QRegExp rx("\\n\\s*(\\d+\\.?\\d*)%[^\\n\\r]+ETA:\\s*(\\d+\\:\\d+\\:\\d+)", Qt::CaseInsensitive);
        if (rx.lastIndexIn(dat.right(200))>=0) {
            ui->progressBar->setValue(QStringToDouble(rx.cap(1)));
            ui->labETA->setText(tr("ETA: %1").arg(rx.cap(2)));
        }
    }
}

void QFEFCSSimScriptTab::error(QProcess::ProcessError error)
{
    ui->pteOutput->log_text("\n\n\n");
    switch (error) {
        case QProcess::FailedToStart:
            ui->pteOutput->log_error(tr("The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program."));
            break;
        case QProcess::Crashed:
            ui->pteOutput->log_error(tr("The process crashed some time after starting successfully."));
            break;
        case QProcess::Timedout:
            ui->pteOutput->log_error(tr("The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again."));
            break;
        case QProcess::WriteError:
            ui->pteOutput->log_error(tr("An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel."));
            break;
        case QProcess::ReadError:
            ui->pteOutput->log_error(tr("An error occurred when attempting to read from the process. For example, the process may not be running."));
            break;
        case QProcess::UnknownError:
        default:
            ui->pteOutput->log_error(tr("unknown error"));
            break;
    }


}

void QFEFCSSimScriptTab::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
     ui->pteOutput->log_text("\n\n\n");
    if (exitStatus==QProcess::CrashExit) {
        ui->pteOutput->log_error(tr("The simulator ended with a crash! Results may not be usable!\nexitCode = %1").arg(exitCode));
    } else {
        ui->pteOutput->log_text(tr("The simulator ended normally! Results are ready for further evaluation!\nexitCode = %1").arg(exitCode));
    }
    setRunning(false);


    QString log=ui->pteOutput->toPlainText();
    QStringList outfiles;
    QDir d(mainWin->getWorkingDir());
    if (proc) d=QDir(proc->workingDirectory());

    ui->pteOutput->log_text(tr("\n\n\nSEARCHING FOR FCS RESULTS THAT CAN BE IMPORTED:\n(use the button import results):\n\n"));
    ui->progressBar->setValue(ui->progressBar->maximum());

    log=log.right(log.size()/2);

    //qDebug()<<log<<log.size();

    isqf3acorr=true;
    {
        QRegExp rxACORR("\\n\\s*writing[^\\n\\r]*([\\\"\\'])([^\\n\\r]+\\.qf3acorr)\\1", Qt::CaseInsensitive);
        int idx=0;
        while ((idx=rxACORR.indexIn(log, idx))>=0) {
            if (rxACORR.cap(2).size()>0) {
                QString fn=d.absoluteFilePath(rxACORR.cap(2));
                if (!outfiles.contains(fn) && QFile::exists(fn)) {
                    outfiles.append(fn);
                    ui->pteOutput->log_text(tr("   - %1\n").arg(fn));
                }
                QApplication::processEvents();
            }
            idx=idx+rxACORR.matchedLength();
        }
    }
    if (outfiles.size()<=0) {
        isqf3acorr=false;
        QRegExp rxASC("\\n\\s*writing[^\\n\\r]*([\\\"\\'])([^\\n\\r]+\\.asc)\\1", Qt::CaseInsensitive);
        int idx=0;
        while ((idx=rxASC.indexIn(log, idx))>=0) {
            if (rxASC.cap(2).size()>0) {
                QString fn=d.absoluteFilePath(rxASC.cap(2));
                if (!outfiles.contains(fn) && QFile::exists(fn)) {
                    outfiles.append(fn);
                    ui->pteOutput->log_text(tr("   - %1\n").arg(fn));
                }
                QApplication::processEvents();
            }
            //qDebug()<<idx<<rxASC.cap(2);
            idx=idx+rxASC.matchedLength();
        }
    }
    resultfiles=outfiles;
    setRunning(false);
    ui->pteOutput->log_text(tr("\n\nFINISHED SIMULATION AND POST-PROCESSING!\n"));
}

void QFEFCSSimScriptTab::on_btnImport_clicked()
{
    if (resultfiles.size()>0) {
        //QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        QFPluginCommandsInterface* command=dynamic_cast<QFPluginCommandsInterface*>(QFPluginServices::getInstance()->getRawDataRecordFactory()->getPlugin("fcs"));
        if (command) {
            for (int i=0; i<resultfiles.size(); i++) {
                if (resultfiles[i].trimmed().toLower().endsWith(".qf3qcorr")) command->sendPluginCommand("load_qf3asciicorr", resultfiles[i]);
                else if (resultfiles[i].trimmed().toLower().endsWith(".asc")) command->sendPluginCommand("load_alv5000", resultfiles[i]);
            }
        } else {
            QMessageBox::critical(this, tr("Import FCS-files"), tr("Imternal Error: FCS-plugin does not support importing!"));
        }
    } else {
        QMessageBox::critical(this, tr("Import FCS-files"), tr("Didn't identify files to be imported!\nImport manually!"));
    }
}
