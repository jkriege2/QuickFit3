#include "ui_qfrdrtableplotsettingswidget.h"
#include "qfrdrtableplotsettingswidget.h"
#include "programoptions.h"
#include "qfrdrtable.h"
#include "qftools.h"
#include "qfdoubleedit.h"
#include "qfmathparserxfunctionlinegraph.h"
#include "qffitfunctionplottools.h"
#include "qffitfunctionmanager.h"
#include "qffitfunction.h"
#include "dlgcolorbarcoloring.h"


QFRDRTablePlotSettingsWidget::QFRDRTablePlotSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFRDRTablePlotSettingsWidget)
{



    updating=true;
    ui->setupUi(this);

    //ui->scrollArea->setWidget();


    //ui->formLayout_3->removeWidget(ui->widSaveCoordSettings);
    //ui->tabWidget->setCornerWidget(ui->widSaveCoordSettings);


    ui->edtAxisAspect->setRange(0,1e10);
    ui->edtAxisAspect->setCheckBounds(true, false);
    ui->edtDataAspect->setRange(0,1e10);
    ui->edtDataAspect->setCheckBounds(true, false);


    connect(ui->axisX, SIGNAL(autoscaleX()), this, SLOT(doAutoscaleX()));
    connect(ui->axisY, SIGNAL(autoscaleX()), this, SLOT(doAutoscaleY()));

    connect(ui->axisX, SIGNAL(updateGraph()), this, SLOT(doUpdateGraph()));
    connect(ui->axisX, SIGNAL(dataChanged()), this, SLOT(plotDataChanged()));
    connect(ui->axisY, SIGNAL(updateGraph()), this, SLOT(doUpdateGraph()));
    connect(ui->axisY, SIGNAL(dataChanged()), this, SLOT(plotDataChanged()));

    current=NULL;
    this->plot=-1;
    updating=false;


    if (ProgramOptions::getInstance() && ProgramOptions::getInstance()->getQSettings()) readSettings(*(ProgramOptions::getInstance()->getQSettings()), "table/QFRDRTablePlotSettingsWidget/");
}

QFRDRTablePlotSettingsWidget::~QFRDRTablePlotSettingsWidget()
{
    updating=true;
    if (ProgramOptions::getInstance() && ProgramOptions::getInstance()->getQSettings()) writeSettings(*(ProgramOptions::getInstance()->getQSettings()), "table/QFRDRTablePlotSettingsWidget/");
    delete ui;
}

void QFRDRTablePlotSettingsWidget::setRecord(QFRDRTable *record, int graph)
{
    current=record;
    this->plot=graph;
    updating=true;
    disconnectWidgets();
    if (record) {
        if (graph>=0 && graph<record->getPlotCount()) {
            setEnabled(true);
            QFRDRTable::PlotInfo g=record->getPlot(plot);
            ui->edtTitle->setText(g.title);

            ui->chkPlotAutosize->setChecked(g.graphAutosize);
            ui->spinWidth->setValue(g.graphWidth);
            ui->spinHeight->setValue(g.graphHeight);

            ui->axisX->loadPlotData(g.xAxis);
            ui->axisY->loadPlotData(g.yAxis);

            ui->chkGrid->setChecked(g.grid);
            ui->chkShowKey->setChecked(g.showKey);
            ui->cmbKeyPosition->setPosition(g.keyPosition);
            ui->spinKeyFontSize->setValue(g.keyFontSize);

            ui->cmbFontname->setCurrentFont(QFont(g.fontName));
            ui->spinAxisFontSize->setValue(g.axisFontSize);
            ui->spinAxisLabelFontSize->setValue(g.axisLabelFontSize);
            ui->spinTitleFontSize->setValue(g.labelFontSize);
            ui->chkKeepAxisAspect->setChecked(g.keepAxisAspectRatio);
            ui->chkKeepDataAspect->setChecked(g.keepDataAspectRatio);
            ui->edtAxisAspect->setValue(g.axisAspectRatio);
            ui->edtDataAspect->setValue(g.dataAspectRatio);

            ui->cmbGridColor->setCurrentColor(g.gridColor);
            ui->cmbBackgroundColor->setCurrentColor(g.backgroundColor);
            ui->cmbGridLinestyle->setCurrentLineStyle(g.gridStyle);
            ui->spinGridWidth->setValue(g.gridWidth);
            ui->sliderKeyTransparency->setValue(g.keyTransparency*255.0);
            ui->cmbKeyLayout->setCurrentIndex(g.keyLayout);

        } else {
            setEnabled(false);
        }
    }
    updating=false;
    connectWidgets();
    doUpdateGraph();
}

bool QFRDRTablePlotSettingsWidget::getLogX() const
{
    return ui->axisX->getLogX();
}

bool QFRDRTablePlotSettingsWidget::getLogY() const
{
    return ui->axisY->getLogX();
}

void QFRDRTablePlotSettingsWidget::readSettings(QSettings &settings, const QString &prefix)
{
    ui->axisX->readSettings(settings, prefix+"/xaxis/");
    ui->axisY->readSettings(settings, prefix+"/yaxis/");
}

void QFRDRTablePlotSettingsWidget::writeSettings(QSettings &settings, const QString &prefix)
{
    ui->axisX->writeSettings(settings, prefix+"/xaxis/");
    ui->axisY->writeSettings(settings, prefix+"/yaxis/");
}



void QFRDRTablePlotSettingsWidget::setXRange(double xmin, double xmax)
{
    ui->axisX->setXRange(xmin, xmax);
}

void QFRDRTablePlotSettingsWidget::setYRange(double ymin, double ymax) {
    ui->axisY->setXRange(ymin, ymax);
}


void QFRDRTablePlotSettingsWidget::doAutoscaleX()
{
    emit autoscaleX();
}

void QFRDRTablePlotSettingsWidget::doAutoscaleY() {
    emit autoscaleY();
}

void QFRDRTablePlotSettingsWidget::graphDataChanged(int r) {
    //qDebug()<<"graphDataChanged    updating="<<updating;
    if (updating) return;
    if (current) {
        updating=true;
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;
        QFRDRTable::PlotInfo p=current->getPlot(this->plot);
        if (r>=0 && r<p.graphs.size()) {
            QFRDRTable::GraphInfo graph=p.graphs.at(r);

            if (ui->axisX->getlabel()=="x" && graph.xcolumn>=0) {
                QString txt=current->tableGetColumnTitle(graph.xcolumn);
                int idx=txt.indexOf(':');
                if (idx>=0) txt=txt.right(txt.size()-(idx+1));
                ui->axisX->setlabel(txt);
            }
            if (ui->axisY->getlabel()=="y" && graph.ycolumn>=0) {
                QString txt=current->tableGetColumnTitle(graph.ycolumn);
                int idx=txt.indexOf(':');
                if (idx>=0) txt=txt.right(txt.size()-(idx+1));
                ui->axisY->setlabel(txt);
            }
            if (ui->edtTitle->text().isEmpty() && graph.ycolumn>=0) {
                QString txt=current->tableGetColumnTitle(graph.ycolumn);
                int idx=txt.indexOf(':');
                if (idx>=0) txt=txt.right(txt.size()-(idx+1));
                ui->edtTitle->setText(txt);
            }

            p.graphs[r]=graph;
            current->setPlot(this->plot, p);

        }
        updating=false;
    }
    doUpdateGraph();
}

void QFRDRTablePlotSettingsWidget::plotDataChanged() {
    //qDebug()<<"plotDataChanged   updating="<<updating;
    if (updating) return;
    if (current) {
        if (this->plot<0 || this->plot>=current->getPlotCount()) return;

        QFRDRTable::PlotInfo p=current->getPlot(this->plot);
        p.title=ui->edtTitle->text();
        p.graphAutosize=ui->chkPlotAutosize->isChecked();
        p.graphWidth=ui->spinWidth->value();
        p.graphHeight=ui->spinHeight->value();
        p.grid=ui->chkGrid->isChecked();
        p.showKey=ui->chkShowKey->isChecked();

        p.fontName=ui->cmbFontname->currentFont().family();
        p.axisFontSize=ui->spinAxisFontSize->value();
        p.axisLabelFontSize=ui->spinAxisLabelFontSize->value();
        p.keyFontSize=ui->spinKeyFontSize->value();
        p.labelFontSize=ui->spinTitleFontSize->value();
        p.keyTransparency=double(ui->sliderKeyTransparency->value())/255.0;
        p.keyPosition=ui->cmbKeyPosition->getPosition();
        p.keyLayout=ui->cmbKeyLayout->getKeyLayout();
        p.dataAspectRatio=ui->edtDataAspect->value();
        p.axisAspectRatio=ui->edtAxisAspect->value();
        p.keepAxisAspectRatio=ui->chkKeepAxisAspect->isChecked();
        p.keepDataAspectRatio=ui->chkKeepDataAspect->isChecked();

        ui->axisX->storePlotData(p.xAxis);
        ui->axisY->storePlotData(p.yAxis);


        p.gridColor=ui->cmbGridColor->currentColor();
        p.backgroundColor=ui->cmbBackgroundColor->currentColor();
        p.gridStyle=ui->cmbGridLinestyle->currentLineStyle();
        p.gridWidth=ui->spinGridWidth->value();
        emit plotTitleChanged(this->plot, p.title);

        current->setPlot(this->plot, p);
        //QFRDRTable::GraphInfo graph=current->getPlot(this->plot).graphs.value(currentRow, QFRDRTable::GraphInfo());
    }
    doUpdateGraph();
}

void QFRDRTablePlotSettingsWidget::doUpdateGraph() {
    if (!updating) emit updateGraph();
}




void QFRDRTablePlotSettingsWidget::connectWidgets()
{
    //qDebug()<<"connectWidgets";

    connect(ui->edtTitle, SIGNAL(editingFinished()), this, SLOT(plotDataChanged()));
    connect(ui->chkGrid, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->chkShowKey, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->cmbFontname, SIGNAL(currentIndexChanged(QString)), this, SLOT(plotDataChanged()));
    connect(ui->spinAxisFontSize, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->spinAxisLabelFontSize, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->spinKeyFontSize, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->spinTitleFontSize, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->sliderKeyTransparency, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbKeyLayout, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbKeyPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->edtAxisAspect, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->edtDataAspect, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->chkKeepAxisAspect, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->chkKeepDataAspect, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));

    connect(ui->cmbGridColor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbBackgroundColor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbGridLinestyle, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->spinGridWidth, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));

    connect(ui->chkPlotAutosize, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->spinHeight, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->spinWidth, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->spinHeight, SIGNAL(editingFinished()), this, SLOT(plotDataChanged()));
    connect(ui->spinWidth, SIGNAL(editingFinished()), this, SLOT(plotDataChanged()));
    ui->axisX->connectWidgets();
    ui->axisY->connectWidgets();
}

void QFRDRTablePlotSettingsWidget::disconnectWidgets()
{
    //qDebug()<<"disconnectWidgets";
    ui->axisX->disconnectWidgets();
    ui->axisY->disconnectWidgets();

    disconnect(ui->edtTitle, SIGNAL(editingFinished()), this, SLOT(plotDataChanged()));
    disconnect(ui->chkGrid, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkShowKey, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbFontname, SIGNAL(currentIndexChanged(QString)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinAxisFontSize, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinAxisLabelFontSize, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinKeyFontSize, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinTitleFontSize, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->sliderKeyTransparency, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbKeyLayout, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbKeyPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->edtAxisAspect, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->edtDataAspect, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkKeepAxisAspect, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkKeepDataAspect, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbGridColor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbBackgroundColor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbGridLinestyle, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinGridWidth, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));

    disconnect(ui->chkPlotAutosize, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinHeight, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinWidth, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinHeight, SIGNAL(editingFinished()), this, SLOT(plotDataChanged()));
    disconnect(ui->spinWidth, SIGNAL(editingFinished()), this, SLOT(plotDataChanged()));

}


void QFRDRTablePlotSettingsWidget::on_btnSaveSystem_clicked() {

    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/table/graph_templates/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFRDRTablePlotSettingsWidget/lasttemplatedir", ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/table/graph_templates/").toString();
    QString filename=qfGetSaveFileName(this, tr("save graph settings template ..."), dir, tr("graph settings template  (*.gst)"))    ;
    if (!filename.isEmpty()) {
        bool ok=true;
        if (QFile::exists(filename)) {
            ok=false;
            if (QMessageBox::question(this, tr("save graph settings template ..."), tr("The file\n  '%1'\nalready exists. Overwrite?").arg(filename), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {
                ok=true;
            }
        }
        if (ok) {
            QSettings set(filename, QSettings::IniFormat);

            ui->axisX->saveToSettings(set, "x");
            ui->axisY->saveToSettings(set, "y");

            set.setValue("title", ui->edtTitle->text());
            set.setValue("autosize",ui->chkPlotAutosize->isChecked());
            set.setValue("plotwidth",ui->spinWidth->value());
            set.setValue("plotheight",ui->spinHeight->value());
            set.setValue("grid",ui->chkGrid->isChecked());
            set.setValue("showkey",ui->chkShowKey->isChecked());
            set.setValue("fontname", ui->cmbFontname->currentText());
            set.setValue("axisfontsize", ui->spinAxisFontSize->value());
            set.setValue("axislabelfontsize", ui->spinAxisLabelFontSize->value());
            set.setValue("keyfontsize", ui->spinKeyFontSize->value());
            set.setValue("titlefontsize", ui->spinTitleFontSize->value());
            set.setValue("keytransparency", ui->sliderKeyTransparency->value());
            set.setValue("keylayout", ui->cmbKeyLayout->currentIndex());
            set.setValue("keyposition", ui->cmbKeyPosition->currentIndex());
            set.setValue("axisaspect", ui->edtAxisAspect->value());
            set.setValue("dataaspect", ui->edtDataAspect->value());
            set.setValue("keepaxisaspect", ui->chkKeepAxisAspect->isChecked());
            set.setValue("keepdataaspect", ui->chkKeepDataAspect->isChecked());
            set.setValue("backgroundColor", QColor2String(ui->cmbBackgroundColor->currentColor()));
            set.setValue("gridColor", QColor2String(ui->cmbGridColor->currentColor()));
            set.setValue("gridStyle", QPenStyle2String(ui->cmbGridLinestyle->currentLineStyle()));
            set.setValue("gridWidth", ui->spinGridWidth->value());
        }
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRTablePlotSettingsWidget/lasttemplatedir", dir);
}





void QFRDRTablePlotSettingsWidget::on_btnLoadSystem_clicked() {
    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/table/graph_templates/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFRDRTablePlotSettingsWidget/lasttemplatedir", ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/table/graph_templates/").toString();
    QString filename=qfGetOpenFileName(this, tr("open graph settings template ..."), dir, tr("graph settings template (*.gst)"))    ;
    if (!filename.isEmpty()) {
        QSettings set(filename, QSettings::IniFormat);

        ui->axisX->loadFromSettings(set, "x");
        ui->axisY->loadFromSettings(set, "y");

        ui->chkPlotAutosize->setChecked(set.value("autosize",ui->chkPlotAutosize->isChecked()).toBool());
        ui->spinWidth->setValue(set.value("plotwidth", ui->spinWidth->value()).toInt());
        ui->spinHeight->setValue(set.value("plotheight", ui->spinHeight->value()).toInt());

        ui->edtTitle->setText(set.value("title", ui->edtTitle->text()).toString());
        ui->chkGrid->setChecked(set.value("grid",ui->chkGrid->isChecked()).toBool());
        ui->chkShowKey->setChecked(set.value("showkey",ui->chkShowKey->isChecked()).toBool());
        ui->cmbFontname->setCurrentFont(QFont(set.value("fontname", ui->cmbFontname->currentFont().family()).toString()));
        ui->spinAxisFontSize->setValue(set.value("axisfontsize", ui->spinAxisFontSize->value()).toDouble());
        ui->spinAxisLabelFontSize->setValue(set.value("axislabelfontsize", ui->spinAxisLabelFontSize->value()).toDouble());
        ui->spinKeyFontSize->setValue(set.value("keyfontsize", ui->spinKeyFontSize->value()).toDouble());
        ui->spinTitleFontSize->setValue(set.value("titlefontsize", ui->spinTitleFontSize->value()).toDouble());
        ui->sliderKeyTransparency->setValue(set.value("keytransparency", ui->sliderKeyTransparency->value()).toDouble());
        ui->cmbKeyLayout->setCurrentIndex(set.value("keylayout", ui->cmbKeyLayout->currentIndex()).toInt());
        ui->cmbKeyPosition->setCurrentIndex(set.value("keyposition", ui->cmbKeyPosition->currentIndex()).toInt());
        ui->edtAxisAspect->setValue(set.value("axisaspect", ui->edtAxisAspect->value()).toDouble());
        ui->edtDataAspect->setValue(set.value("dataaspect", ui->edtDataAspect->value()).toDouble());
        ui->chkKeepAxisAspect->setChecked(set.value("keepaxisaspect", ui->chkKeepAxisAspect->isChecked()).toBool());
        ui->chkKeepDataAspect->setChecked(set.value("keepdataaspect", ui->chkKeepDataAspect->isChecked()).toBool());
        ui->cmbGridColor->setCurrentColor(QColor(set.value("gridColor", QColor2String(ui->cmbGridColor->currentColor())).toString()));
        ui->cmbBackgroundColor->setCurrentColor(QColor(set.value("backgroundColor", QColor2String(ui->cmbBackgroundColor->currentColor())).toString()));
        ui->cmbGridLinestyle->setCurrentLineStyle(String2QPenStyle(set.value("gridStyle", QPenStyle2String(ui->cmbGridLinestyle->currentLineStyle())).toString()));
        ui->spinGridWidth->setValue(set.value("gridWidth", ui->spinGridWidth->value()).toDouble());
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRTablePlotSettingsWidget/lasttemplatedir", dir);
}

