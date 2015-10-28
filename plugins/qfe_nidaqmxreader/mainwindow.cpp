#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "programoptions.h"
#include <QIcon>
#include <QMessageBox>
#include <QFileDialog>
#define MIN_REPLOT_DELAY (ui->spinPlotUpdate->value())

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qRegisterMetaType<QList<QVector<double> > >("QList<QVector<double> >");
    NITaskHandle=0;
    channels=new NIChannelsModel(this);
    ui->setupUi(this);
    ui->tabChannels->setModel(channels);
    ui->tabChannels->setItemDelegate(new NIChannelsDelegate(this));

    loadSettings(QString(), "qfe_nidaqmxreader/");

    actExit=new QAction(QIcon(":/qfe_nidaqmxreader/images/close.png"), tr("&Exit"), this);
    connect(actExit, SIGNAL(triggered()), this, SLOT(close()));
    actStartAcquisition=new QAction(QIcon(":/qfe_nidaqmxreader/images/play.png"), tr("Start &Acquisition"), this);
    actStartAcquisition->setCheckable(true);
    actStartAcquisition->setChecked(false);

    actStartPreview=new QAction(QIcon(":/qfe_nidaqmxreader/images/preview.png"), tr("Start &Preview"), this);
    actStartPreview->setCheckable(true);
    actStartPreview->setChecked(false);

    actSaveINI=new QAction(tr("Save Program Settings (INI) ..."), this);
    connect(actSaveINI, SIGNAL(triggered()), this, SLOT(saveINI()));
    actLoadINI=new QAction(tr("Load Program Settings (INI) ..."), this);
    connect(actLoadINI, SIGNAL(triggered()), this, SLOT(loadINI()));


    QMenu* mFile=ui->menuBar->addMenu("File");
    mFile->addAction(actLoadINI);
    mFile->addAction(actSaveINI);
    mFile->addSeparator();
    mFile->addAction(actExit);
    QMenu* mAcquisition=ui->menuBar->addMenu("Acquisition");
    mAcquisition->addAction(actStartPreview);
    mAcquisition->addAction(actStartAcquisition);
    QMenu* mData=ui->menuBar->addMenu("Data");
    mData->addAction(ui->plotter->get_plotter()->get_actCopyData());
    mData->addAction(ui->plotter->get_plotter()->get_actSaveData());
    mData->addAction(ui->plotter->get_plotter()->get_actShowPlotData());
    mData->addSeparator();
    mData->addAction(ui->plotter->get_plotter()->get_actCopyPixelImage());
    mData->addAction(ui->plotter->get_plotter()->get_actSavePlot());
    mData->addAction(ui->plotter->get_plotter()->get_actPrint());
    mData->addSeparator();
    mData->addAction(ui->plotter->get_actCopyToTable());
    mData->addAction(ui->plotter->get_actCopyToCurve());

    ui->mainToolBar->addAction(actExit);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(actStartPreview);
    ui->mainToolBar->addAction(actStartAcquisition);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->plotter->get_plotter()->get_actCopyData());
    ui->mainToolBar->addAction(ui->plotter->get_plotter()->get_actSaveData());
    ui->mainToolBar->addAction(ui->plotter->get_plotter()->get_actShowPlotData());
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->plotter->get_plotter()->get_actCopyPixelImage());
    ui->mainToolBar->addAction(ui->plotter->get_plotter()->get_actSavePlot());
    ui->mainToolBar->addAction(ui->plotter->get_plotter()->get_actPrint());
    tbQF3=new QToolBar(tr("QF3_toolbar"));
    tbQF3->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    addToolBarBreak();
    addToolBar(tbQF3);
    tbQF3->addAction(ui->plotter->get_actCopyToTable());
    tbQF3->addAction(ui->plotter->get_actCopyToCurve());
    connect(actStartAcquisition, SIGNAL(toggled(bool)), this, SLOT(startAcquisition(bool)));
    connect(actStartPreview, SIGNAL(toggled(bool)), this, SLOT(startPreview(bool)));
    connect(this, SIGNAL(addedLogMessage(QString)), this, SLOT(addLogMessage(QString)));
    connect(this, SIGNAL(sentSetAcquisitionEnabled(bool)), this, SLOT(setAcquisitionEnabled(bool)));
    connect(this, SIGNAL(sentData(QList<QVector<double> >)), this, SLOT(addData(QList<QVector<double> >)));

    NIMainWindow=this;
    lastPlotUpdate.restart();
    setWindowIcon(QIcon(":/qfe_nidaqmxreader/qfe_nidaqmxreader_large.png"));
    setWindowTitle(tr("NI-DAQmx Channel-Reader"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    actStartAcquisition->setChecked(false);
    actStartPreview->setChecked(false);
    QApplication::processEvents();
    QApplication::processEvents();
    QApplication::processEvents();
    QApplication::processEvents();
    storeSettings(QString(), "qfe_nidaqmxreader/");
    event->accept();
}

void MainWindow::on_btnRemoveChannel_clicked()
{
    QModelIndexList l=ui->tabChannels->selectionModel()->selectedIndexes();
    if (l.size()>0) {
        QSet<int> r;
        for (int i=0; i<l.size(); i++) {
            if (!r.contains(l[i].row())) r.insert(l[i].row());
        }
        channels->removeChannel(r.toList());
    }
}

void MainWindow::on_btnAddChannel_clicked()
{
    channels->addChannel();
}


/** \brief load widgets contents from INI file */
void MainWindow::loadSettings(const QString &fn, const QString &prefix)
{
    QString f=fn;
    QSettings* set=NULL;
    if (!f.isEmpty()) {
        set=new QSettings(f, QSettings::IniFormat);
    } else {
        set=ProgramOptions::getInstance()->getQSettings();
    }
    if (set) {
        channels->loadSettings(*set, prefix+QString("NIChannels/"));
        ui->chkStartTrigger->setChecked(set->value(prefix+"chkStartTrigger", false).toBool());
        ui->cmbStartTriggerEdge->setCurrentIndex(set->value(prefix+"cmbStartTriggerEdge", 0).toInt());
        ui->edtStartTriggerChannel->setText(set->value(prefix+"edtStartTriggerChannel", "/Dev1/PFI0").toString());
        ui->edtClockSource->setText(set->value(prefix+"edtClockSource", "").toString());
        ui->chkStartTriggerRetrigger->setChecked(set->value(prefix+"chkStartTriggerRetrigger", false).toBool());
        ui->chkRefTrigger->setChecked(set->value(prefix+"chkRefTrigger", true).toBool());
        ui->cmbRefTriggerEdge->setCurrentIndex(set->value(prefix+"cmbRefTriggerEdge", 0).toInt());
        ui->edtRefTriggerChannel->setText(set->value(prefix+"edtRefTriggerChannel", "/Dev1/PFI0").toString());
        ui->spinSampleClock->setValue(set->value(prefix+"spinSampleClock", 1000).toDouble());
        ui->spinUpdatePlotSamples->setValue(set->value(prefix+"spinUpdatePlotSamples", 100).toInt());
        ui->spinSamples->setValue(set->value(prefix+"spinSamples", 10000).toInt());
        ui->spinPreSamples->setValue(set->value(prefix+"spinPreSamples", 1000).toInt());
        ui->spinPreviewSampleClock->setValue(set->value(prefix+"spinPreviewSampleClock", 200).toDouble());
        ui->spinUpdatePreviewSamples->setValue(set->value(prefix+"spinUpdatePreviewSamples", 50).toInt());
        ui->spinMaxPreviewSamples->setValue(set->value(prefix+"spinMaxPreviewSamples", 1000).toInt());
        ui->chkAutoScaleY->setChecked(set->value(prefix+"chkAutoScaleY", false).toBool());
        ui->spinYMin->setValue(set->value(prefix+"spinYMin", -10).toDouble());
        ui->spinYMax->setValue(set->value(prefix+"spinYMax", 10).toDouble());
        ui->spinDisplayAvg->setValue(set->value(prefix+"spinDisplayAvg", 10).toInt());
        ui->spinPlotUpdate->setValue(set->value(prefix+"spinPlotUpdate", 100).toInt());

        ui->cmbCTransform->setCurrentIndex(set->value(prefix+"cmbCTransform", 0).toInt());
        ui->spinCTransform1->setValue(set->value(prefix+"spinCTransform1", 0).toInt());
        ui->spinCTransform2->setValue(set->value(prefix+"spinCTransform2", 1).toInt());
        lastINIDir=set->value(prefix+"lastINIDir", "").toString();
    }
    if (f.isEmpty() && set) {
        delete set;
    }
}

/** \brief save widgets contents to INI file */
void MainWindow::storeSettings(const QString &fn, const QString&prefix)
{
    QString f=fn;
    QSettings* set=NULL;
    if (!f.isEmpty()) {
        set=new QSettings(f, QSettings::IniFormat);
    } else {
        set=ProgramOptions::getInstance()->getQSettings();
    }
    if (set) {
        set->setValue(prefix+"chkStartTrigger", ui->chkStartTrigger->isChecked());
        set->setValue(prefix+"chkStartTriggerRetrigger", ui->chkStartTriggerRetrigger->isChecked());
        set->setValue(prefix+"cmbStartTriggerEdge", ui->cmbStartTriggerEdge->currentIndex());
        set->setValue(prefix+"edtStartTriggerChannel", ui->edtStartTriggerChannel->text());
        set->setValue(prefix+"edtClockSource", ui->edtClockSource->text());
        set->setValue(prefix+"chkRefTrigger", ui->chkRefTrigger->isChecked());
        set->setValue(prefix+"cmbRefTriggerEdge", ui->cmbRefTriggerEdge->currentIndex());
        set->setValue(prefix+"edtRefTriggerChannel", ui->edtRefTriggerChannel->text());
        set->setValue(prefix+"spinSampleClock", ui->spinSampleClock->value());
        set->setValue(prefix+"spinUpdatePlotSamples", ui->spinUpdatePlotSamples->value());
        set->setValue(prefix+"spinSamples", ui->spinSamples->value());
        set->setValue(prefix+"spinPreSamples", ui->spinPreSamples->value());
        set->setValue(prefix+"spinPreviewSampleClock", ui->spinPreviewSampleClock->value());
        set->setValue(prefix+"spinUpdatePreviewSamples", ui->spinUpdatePreviewSamples->value());
        set->setValue(prefix+"spinMaxPreviewSamples", ui->spinMaxPreviewSamples->value());
        set->setValue(prefix+"spinPlotUpdate", ui->spinPlotUpdate->value());
        set->setValue(prefix+"spinCTransform1", ui->spinCTransform1->value());
        set->setValue(prefix+"spinCTransform2", ui->spinCTransform2->value());
        set->setValue(prefix+"cmbCTransform", ui->cmbCTransform->currentIndex());
        set->setValue(prefix+"chkAutoScaleY", ui->chkAutoScaleY->isChecked());
        set->setValue(prefix+"spinYMin", ui->spinYMin->value());
        set->setValue(prefix+"spinYMax", ui->spinYMax->value());
        set->setValue(prefix+"spinDisplayAvg", ui->spinDisplayAvg->value());
        set->setValue(prefix+"lastINIDir", lastINIDir);
        channels->storeSettings(*set, prefix+QString("NIChannels/"));
    }
    if (f.isEmpty() && set) {
        delete set;
    }
}

void MainWindow::loadINI()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load INI"),
                                                    lastINIDir,
                                                    tr("INI files (*.ini);;All Files (*.*)"));
    if (!fileName.isEmpty()) loadSettings(fileName);
}

void MainWindow::saveINI()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save INI"),
                                                    lastINIDir,
                                                    tr("INI files (*.ini);;All Files (*.*)"));
    if (!fileName.isEmpty()) storeSettings(fileName);
}


/** \brief start an acquisition */
void MainWindow::startAcquisition(bool start)
{
    ui->spinCTransform1->setMaximum(getChannels()->getChannelCount());
    ui->spinCTransform2->setMaximum(getChannels()->getChannelCount());
    // if a preview is running: stop the preview
    if (actStartPreview->isChecked()) {
        disconnect(actStartPreview, SIGNAL(toggled(bool)), this, SLOT(startPreview(bool)));
        actStartPreview->setChecked(false);
        actStartPreview->setIcon(QIcon(":/qfe_nidaqmxreader/images/preview.png"));
        actStartPreview->setText(tr("Start &Preview"));
        stopNI(NITaskHandle);
        connect(actStartPreview, SIGNAL(toggled(bool)), this, SLOT(startPreview(bool)));
    }

    // store settings to INI
    if (start) storeSettings(QString(), "General/");

    // disable settings tab, as the user may not alter the task settings during acquisition
    ui->tabSettings->setEnabled(!(start || actStartPreview->isChecked()));

    // make sure, plot is enabled
    ui->tabPlot->setEnabled(true);
    if (start) {
        actStartPreview->setChecked(false);
        actStartPreview->setEnabled(false);
        actStartAcquisition->setIcon(QIcon(":/qfe_nidaqmxreader/images/pause.png"));
        actStartAcquisition->setText(tr("Pause &Acquisition"));
        ui->tabWidget->setCurrentWidget(ui->tabPlot);
        if (ui->spinSamples->value()/ui->spinDisplayAvg->value()>300) {
            if (QMessageBox::warning(this, tr("start acquisition"), tr("You will plot more than 300 datapoints in acquisition mode. This may lead to instabilities!\nDou you want the plot-averaging to preset to a save value?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
                ui->spinDisplayAvg->setValue(ceil(double(ui->spinSamples->value())/300.0));
            }
        }
    } else {
        actStartPreview->setChecked(false);
        actStartPreview->setEnabled(true);
        actStartAcquisition->setIcon(QIcon(":/qfe_nidaqmxreader/images/play.png"));
        actStartAcquisition->setText(tr("Start &Acquisition"));
    }

    if (start) {
        stopNI(NITaskHandle);
        NITaskHandle=setupNI(this);
        ui->plotter->set_doDrawing(false);
        ui->plotter->getDatastore()->clear();
        ui->plotter->clearGraphs(true);
        plteData.clear();
        double dummy=0;
        ;
        ui->plotter->getDatastore()->addColumnForItem(ui->plotter->getDatastore()->addCopiedItem(&dummy, 1), 0, QString("time [seconds}"));
        for (int i=0; i<getChannels()->getChannelCount(); i++) {
            plteData.append(new JKQTPxyLineGraph(ui->plotter->get_plotter()));
            plteData.last()->set_xColumn(0);
            plteData.last()->set_yColumn(i+1);
            plteData.last()->set_title(channels->getChannel(i).name);
            plteData.last()->set_symbol(JKQTPnoSymbol);
            plteData.last()->set_lineWidth(1);
            plteData.last()->set_drawLine(true);
            ui->plotter->getDatastore()->addColumnForItem(ui->plotter->getDatastore()->addCopiedItem(&dummy, 1), 0, QString("channel %1 data").arg(i+1));
            ui->plotter->addGraph(plteData.last());
        }
        for (int i=0; i<getChannels()->getChannelCount(); i++) {
            ui->plotter->getDatastore()->addColumnForItem(ui->plotter->getDatastore()->addCopiedItem(&dummy, 1), 0, QString("channel %1 transfromed data (%2 + %3 * x)").arg(i+1).arg(getChannels()->getChannel(i).linTransformOffset).arg(getChannels()->getChannel(i).linTransformSlope));
        }
        ui->plotter->getDatastore()->addColumnForItem(ui->plotter->getDatastore()->addCopiedItem(&dummy, 1), 0, QString("time avgeraged [seconds]"));
        for (int i=0; i<getChannels()->getChannelCount(); i++) {
            ui->plotter->getDatastore()->addColumnForItem(ui->plotter->getDatastore()->addCopiedItem(&dummy, 1), 0, QString("transformed channel %1 avg. data").arg(i+1));
        }
        ui->plotter->getDatastore()->addColumnForItem(ui->plotter->getDatastore()->addCopiedItem(&dummy, 1), 0, QString("display channel transform: %1: %2--%3").arg(ui->cmbCTransform->currentText()).arg(ui->spinCTransform1->value()).arg(ui->spinCTransform2->value()));
        ui->plotter->getDatastore()->addColumnForItem(ui->plotter->getDatastore()->addCopiedItem(&dummy, 1), 0, QString("display channel transform averaged: %1: %2--%3").arg(ui->cmbCTransform->currentText()).arg(ui->spinCTransform1->value()).arg(ui->spinCTransform2->value()));

        ui->plotter->set_doDrawing(true);
        ui->plotter->update_plot();
        if (NITaskHandle!=0) {
            startNI(NITaskHandle);
        }
        ui->btnClearGraph->setEnabled(false);
    } else {
        stopNI(NITaskHandle);
        ui->btnClearGraph->setEnabled(true);
        actStartPreview->setChecked(false);
        actStartPreview->setEnabled(true);
        actStartPreview->setIcon(QIcon(":/qfe_nidaqmxreader/images/preview.png"));
        actStartPreview->setText(tr("Start &Preview"));
    }
}

void MainWindow::startPreview(bool start)
{
    ui->spinCTransform1->setMaximum(getChannels()->getChannelCount());
    ui->spinCTransform2->setMaximum(getChannels()->getChannelCount());
    if (actStartAcquisition->isChecked()) return;

    if (start) storeSettings(QString(), "qfe_nidaqmxreader/");
    ui->tabSettings->setEnabled(!(start || actStartAcquisition->isChecked()));
    ui->tabPlot->setEnabled(true);
    if (start) {
        actStartAcquisition->setChecked(false);
        actStartAcquisition->setEnabled(true);
        actStartPreview->setIcon(QIcon(":/qfe_nidaqmxreader/images/preview.png"));
        actStartPreview->setText(tr("Pause &Preview"));
        ui->tabWidget->setCurrentWidget(ui->tabPlot);
        if (ui->spinMaxPreviewSamples->value()/ui->spinDisplayAvg->value()>300) {
            if (QMessageBox::warning(this, tr("start preview"), tr("You will plot more than 300 datapoints in preview mode. This may lead to instabilities!\nDou you want the plot-averaging to preset to a save value?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
                ui->spinDisplayAvg->setValue(ceil(double(ui->spinMaxPreviewSamples->value())/300.0));
            }
        }
    } else {
        actStartPreview->setIcon(QIcon(":/qfe_nidaqmxreader/images/preview.png"));
        actStartPreview->setText(tr("Start &Preview"));
    }

    if (start) {
        stopNI(NITaskHandle);
        NITaskHandle=setupNIPreview(this);
        ui->plotter->set_doDrawing(false);
        ui->plotter->getDatastore()->clear();
        ui->plotter->clearGraphs(true);
        plteData.clear();
        double dummy=0;
        ;
        ui->plotter->getDatastore()->addColumnForItem(ui->plotter->getDatastore()->addCopiedItem(&dummy, 1), 0, QString("time [seconds]"));
        for (int i=0; i<getChannels()->getChannelCount(); i++) {
            plteData.append(new JKQTPxyLineGraph(ui->plotter->get_plotter()));
            plteData.last()->set_xColumn(0);
            plteData.last()->set_yColumn(i+1);
            plteData.last()->set_title(channels->getChannel(i).name);
            plteData.last()->set_symbol(JKQTPnoSymbol);
            plteData.last()->set_lineWidth(1);
            plteData.last()->set_drawLine(true);
            ui->plotter->getDatastore()->addColumnForItem(ui->plotter->getDatastore()->addCopiedItem(&dummy, 1), 0, QString("channel %1 data").arg(i+1));
            ui->plotter->addGraph(plteData.last());
        }
        for (int i=0; i<getChannels()->getChannelCount(); i++) {
            ui->plotter->getDatastore()->addColumnForItem(ui->plotter->getDatastore()->addCopiedItem(&dummy, 1), 0, QString("channel %1 transfromed data (%2 + %3 * x)").arg(i+1).arg(getChannels()->getChannel(i).linTransformOffset).arg(getChannels()->getChannel(i).linTransformSlope));
        }
        ui->plotter->getDatastore()->addColumnForItem(ui->plotter->getDatastore()->addCopiedItem(&dummy, 1), 0, QString("time avgeraged [seconds]"));
        for (int i=0; i<getChannels()->getChannelCount(); i++) {
            ui->plotter->getDatastore()->addColumnForItem(ui->plotter->getDatastore()->addCopiedItem(&dummy, 1), 0, QString("transformed channel %1 avg. data").arg(i+1));
        }
        ui->plotter->getDatastore()->addColumnForItem(ui->plotter->getDatastore()->addCopiedItem(&dummy, 1), 0, QString("display channel transform: %1: %2--%3").arg(ui->cmbCTransform->currentText()).arg(ui->spinCTransform1->value()).arg(ui->spinCTransform2->value()));
        ui->plotter->getDatastore()->addColumnForItem(ui->plotter->getDatastore()->addCopiedItem(&dummy, 1), 0, QString("display channel transform averaged: %1: %2--%3").arg(ui->cmbCTransform->currentText()).arg(ui->spinCTransform1->value()).arg(ui->spinCTransform2->value()));

        ui->plotter->set_doDrawing(true);
        ui->plotter->update_plot();
        if (NITaskHandle!=0) {
            startNI(NITaskHandle);
        }
        ui->btnClearGraph->setEnabled(true);
    } else {
        stopNI(NITaskHandle);
        ui->btnClearGraph->setEnabled(true);
    }
}

void MainWindow::on_btnClearGraph_clicked()
{
    ui->plotter->set_doDrawing(false);
    JKQTPdatastore* ds=ui->plotter->getDatastore();
    int maxInt=1;
    for (int i=0; i<plteData.size(); i++) {
        int col=plteData[i]->get_yColumn();
        QVector<double> dat;
        dat.resize(1);
        dat[0]=0;
        JKQTPdatastoreItem* it=ds->getItem(ds->getColumn(col).get_datastoreItem());
        if (it) {
            it->resizeColumns(dat.size());
            for (int t=0; t<dat.size(); t++) {
                it->set(0, t, dat[t]);
            }
        }
    }

    int xcol=0;
    JKQTPdatastoreItem* it=ds->getItem(ds->getColumn(xcol).get_datastoreItem());
    if (it) {
        it->resizeColumns(maxInt);
        for (int i=0; i<maxInt; i++) {
            it->set(0, i, double(i)/getSampleClock());
        }
    }

    ui->plotter->zoomToFit();
    ui->plotter->set_doDrawing(true);
    ui->plotter->update_plot();
    QApplication::processEvents();
}

void MainWindow::on_cmbCTransform_currentIndexChanged(int index)
{
    ui->spinCTransform1->setEnabled(index>0);
    ui->spinCTransform2->setEnabled(index>0);
}

NIChannelsModel *MainWindow::getChannels() const
{
    return channels;
}

bool MainWindow::getStartTrigger() const
{
    return ui->chkStartTrigger->isChecked();
}

bool MainWindow::getStartTriggerRetriggerable() const
{
    return ui->chkStartTriggerRetrigger->isChecked();
}

int MainWindow::getStartTriggerEdge() const
{
    return ui->cmbStartTriggerEdge->currentIndex();
}

QString MainWindow::getStartTriggerChannel() const
{
    return ui->edtStartTriggerChannel->text();
}

bool MainWindow::getRefTrigger() const
{
    return ui->chkRefTrigger->isChecked();
}


int MainWindow::getRefTriggerEdge() const
{
    return ui->cmbRefTriggerEdge->currentIndex();
}

QString MainWindow::getRefTriggerChannel() const
{
    return ui->edtRefTriggerChannel->text();
}


double MainWindow::getSampleClock() const
{
    return ui->spinSampleClock->value();
}

QString MainWindow::getClockSource() const
{
    return ui->edtClockSource->text();
}

double MainWindow::getPreviewSampleClock() const
{
    return ui->spinPreviewSampleClock->value();
}

int MainWindow::getUpdatePlotSamples() const
{
    return ui->spinUpdatePlotSamples->value();
}

int MainWindow::getUpdatePreviewPlotSamples() const
{
    return ui->spinUpdatePreviewSamples->value();
}

int MainWindow::getSamples() const
{
    return  ui->spinSamples->value();
}

int MainWindow::getPretriggerSamples() const
{
    return  ui->spinPreSamples->value();
}

QAction *MainWindow::getAcquisitionAction() const
{
    return actStartAcquisition;
}

void MainWindow::sendSetAcquisitionEnabled(bool en)
{
    emit sentSetAcquisitionEnabled(en);
}

void MainWindow::sendLogMessage(const QString &message)
{
    emit addedLogMessage(message);
}

void MainWindow::sendData(QList<QVector<double> > data)
{
    emit sentData(data);
}

void MainWindow::addLogMessage(const QString &message)
{
    QTextCursor c=ui->textBrowser->textCursor();
    c.movePosition(QTextCursor::End);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    c.insertHtml(message.toHtmlEscaped()+"<br>");
#else
    c.insertHtml(Qt::escape(message)+"<br>");
#endif
    ui->textBrowser->moveCursor(QTextCursor::End);
}

void MainWindow::setAcquisitionEnabled(bool en)
{
    actStartAcquisition->setChecked(en);
    if (!en) {
        actStartPreview->setEnabled(true);
        actStartPreview->setChecked(false);
        actStartPreview->setIcon(QIcon(":/qfe_nidaqmxreader/images/preview.png"));
        actStartPreview->setText(tr("Start &Preview"));
        actStartAcquisition->setIcon(QIcon(":/qfe_nidaqmxreader/images/play.png"));
        actStartAcquisition->setText(tr("Start &Acquisition"));
    }
}

void MainWindow::addData(const QList<QVector<double> > data)
{
    ui->plotter->set_doDrawing(false);
    JKQTPdatastore* ds=ui->plotter->getDatastore();
    int maxInt=0;
    double vmin=DBL_MAX;
    double vmax=-DBL_MAX;
    int avgCnt=ui->spinDisplayAvg->value();
    QVector<double> datCT1, datCT2;

    const int rawTimeColumn0=0;
    const int rawColumn0=rawTimeColumn0+1;
    const int transformedColumn0=1+plteData.size();
    const int averagedTimeColumn0=1+2*plteData.size();
    const int averagedColumn0=averagedTimeColumn0+1;
    const int averagedCTransformColumn=2+3*plteData.size();
    const int averagedCTransformColumnAvg=averagedCTransformColumn+1;

    for (int i=0; i<data.size(); i++) {
        if (i<plteData.size()) {
            QVector<double> dat=ds->getColumn(rawColumn0+i).copyData();
            if (dat.size()==1 && dat[0]==0) dat.clear();
            dat<<data[i];

            // ensure, that at most, the given number of samples is in the plot datastore, when in preview mode!
            if (actStartPreview->isChecked() && dat.size()>0 && dat.size()>ui->spinMaxPreviewSamples->value()) {
                dat.remove(0, dat.size()-ui->spinMaxPreviewSamples->value());
            }

            double oldvmin=vmin, oldvmax=vmax;
            maxInt=qMax(maxInt, dat.size());
            JKQTPdatastoreItem* it=ds->getItem(ds->getColumn(rawColumn0+i).get_datastoreItem());
            if (it) {
                it->resizeColumns(dat.size());
                for (int t=0; t<dat.size(); t++) {
                    const double v=dat[t];
                    it->set(0, t, v);
                    vmin=qMin(vmin, v);
                    vmax=qMax(vmax, v);
                }
                if (ui->cmbCTransform->currentIndex()>0) {
                    if (i==ui->spinCTransform1->value()) datCT1=dat;
                    else if (i==ui->spinCTransform2->value()) datCT2=dat;
                }
            }
            const double ltOffset=channels->getChannel(i).linTransformOffset;
            const double ltSlope=channels->getChannel(i).linTransformSlope;
            if (ltOffset!=0.0 || ltSlope!=1.0) {
                vmin=oldvmin;
                vmax=oldvmax;
                JKQTPdatastoreItem* itT=ds->getItem(ds->getColumn(transformedColumn0+i).get_datastoreItem());
                if (itT) {
                    itT->resizeColumns(dat.size());
                    for (int t=0; t<dat.size(); t++) {
                        const double v=ltOffset+ltSlope*dat[t];
                        itT->set(0, t, v);
                        vmin=qMin(vmin, v);
                        vmax=qMax(vmax, v);
                        dat[t]=v;
                    }
                }
                if (ui->cmbCTransform->currentIndex()>0) {
                    if (i==ui->spinCTransform1->value()) datCT1=dat;
                    else if (i==ui->spinCTransform2->value()) datCT2=dat;
                }
            }


            if (avgCnt>1) {
                JKQTPdatastoreItem* it=ds->getItem(ds->getColumn(averagedColumn0+i).get_datastoreItem());
                if (it) {
                    it->resizeColumns(dat.size()/avgCnt);
                    int ii=0;
                    for (int t=0; t<dat.size()/avgCnt; t++) {
                        double s=0, c=0;
                        for (int j=0; j<avgCnt; j++) {
                            if (ii<dat.size()){
                                c++;
                                s=s+dat[ii];
                                ii++;
                            }
                        }
                        const double v=s/c;
                        it->set(0, t, v);
                        vmin=qMin(vmin, v);
                        vmax=qMax(vmax, v);
                    }
                }
                plteData[i]->set_xColumn(averagedTimeColumn0);
                plteData[i]->set_yColumn(averagedColumn0+i);
                //qDebug()<<i<<": "<<plteData.size()+1<<plteData.size()+1+col;
            } else {
                if (ltOffset!=0.0 || ltSlope!=1.0) {
                    plteData[i]->set_xColumn(rawTimeColumn0);
                    plteData[i]->set_yColumn(transformedColumn0+i);
                } else {
                    plteData[i]->set_xColumn(rawTimeColumn0);
                    plteData[i]->set_yColumn(rawColumn0+i);
                }
                //qDebug()<<i<<": "<<0<<col;
            }
            if (ui->cmbCTransform->currentIndex()!=0) {
                if (i==ui->spinCTransform2->value()) {
                    plteData[i]->set_visible(false);
                } else if (i==ui->spinCTransform1->value()) {
                    if (avgCnt>1) {
                        plteData[i]->set_xColumn(averagedTimeColumn0);
                        plteData[i]->set_yColumn(averagedCTransformColumnAvg);
                    } else {
                        plteData[i]->set_xColumn(averagedTimeColumn0);
                        plteData[i]->set_yColumn(averagedCTransformColumn);
                    }
                }
            } else {
                plteData[i]->set_visible(true);
            }
            if (ui->cmbCTransform->currentIndex()>0) {
                if (i==ui->spinCTransform1->value() || i==ui->spinCTransform2->value()) {
                    vmin=oldvmin;
                    vmax=oldvmax;
                }
            }
        }
    }

    double clk=getSampleClock();
    if (actStartPreview->isChecked()) {
        clk=getPreviewSampleClock();
    }
    JKQTPdatastoreItem* it=ds->getItem(ds->getColumn(rawTimeColumn0).get_datastoreItem());
    if (it) {
        it->resizeColumns(maxInt);
        for (int i=0; i<maxInt; i++) {
            it->set(0, i, double(i)/clk);
        }
    }

    if (ui->cmbCTransform->currentIndex()>0 && ui->spinCTransform1->value()<getChannels()->getChannelCount() && ui->spinCTransform2->value()<getChannels()->getChannelCount() && datCT1.size()>0 && datCT2.size()>0) {
        int size=qMin(datCT1.size(), datCT2.size());
        double lastVMin=vmin;
        double lastVMax=vmax;
        {
            JKQTPdatastoreItem* it=ds->getItem(ds->getColumn(averagedCTransformColumn).get_datastoreItem());
            if (it) {
                it->resizeColumns(size);
                if (ui->cmbCTransform->currentIndex()==1) {
                    for (int i=0; i<size; i++) {
                        const double v=datCT1[i]/datCT2[i];
                        it->set(0, i, v);
                        vmin=qMin(vmin, v);
                        vmax=qMax(vmax, v);
                    }
                }
            }
        }
        if (avgCnt>1) {
            vmin=lastVMin;
            vmax=lastVMax;
            JKQTPdatastoreItem* it=ds->getItem(ds->getColumn(averagedCTransformColumnAvg).get_datastoreItem());
            if (it) {
                it->resizeColumns(size/avgCnt);
                int k=0;
                if (ui->cmbCTransform->currentIndex()==1) {
                    for (int i=0; i<size/avgCnt; i++) {
                        double c=0, s=0;
                        for (int j=0; j<avgCnt; j++) {
                            c=c+(datCT1[k]/datCT2[k]);
                            s++;
                            k++;
                        }
                        const double v=c/s;
                        it->set(0, i, v);
                        vmin=qMin(vmin, v);
                        vmax=qMax(vmax, v);
                    }
                }
            }
        }
    }

    if (avgCnt>1) {
        JKQTPdatastoreItem* it=ds->getItem(ds->getColumn(averagedTimeColumn0).get_datastoreItem());
        if (it) {
            it->resizeColumns(maxInt/avgCnt);
            for (int i=0; i<maxInt/avgCnt; i++) {
                it->set(0, i, double(i*avgCnt)/clk);
            }
        }
    }

    if (vmin>=0.5*DBL_MAX) vmin=-10;
    if (vmax<=-0.5*DBL_MAX) vmax=10;

    //qDebug()<<vmin<<vmax;

    if (ui->chkAutoScaleY->isChecked()) {
        ui->plotter->setXY(0, double(maxInt)/clk, vmin, vmax);
    } else {
        ui->plotter->setXY(0, double(maxInt)/clk, ui->spinYMin->value(), ui->spinYMax->value());
    }
    ui->plotter->set_doDrawing(true);
    if (lastPlotUpdate.elapsed()>MIN_REPLOT_DELAY) {
        ui->plotter->update_plot();
        lastPlotUpdate.restart();
    }
    QApplication::processEvents();
}

