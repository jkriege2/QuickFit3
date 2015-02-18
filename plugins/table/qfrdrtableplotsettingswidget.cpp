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

            ui->axisX->setRecord(record, graph);
            ui->axisX->loadPlotData(g.xAxis);
            ui->axisY->setRecord(record, graph);
            ui->axisY->loadPlotData(g.yAxis);

            ui->grpMajor->setChecked(g.grid);
            ui->grpMinor->setChecked(g.gridMinor);
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


            ui->spinKeyBoxWidth->setValue(g.keyBoxLineWidth);
            ui->cmbKeyBackground->setCurrentColor(g.keyBackgroundColor);
            ui->cmbKeyBoxColor->setCurrentColor(g.keyLineColor);
            ui->chkKeyBox->setChecked(g.keyBox);

            ui->cmbBackgroundColor->setCurrentColor(g.backgroundColor);
            ui->cmbGridColor->setCurrentColor(g.gridColor);
            ui->cmbGridLinestyle->setCurrentLineStyle(g.gridStyle);
            ui->spinGridWidth->setValue(g.gridWidth);
            ui->cmbGridColorMinor->setCurrentColor(g.gridColorMinor);
            ui->cmbGridLinestyleMinor->setCurrentLineStyle(g.gridStyleMinor);
            ui->spinGridWidthMinor->setValue(g.gridWidthMinor);
            ui->sliderKeyTransparency->setValue(g.keyTransparency*255.0);

            ui->chkGridMajorX->setChecked(g.gridMajorX);
            ui->chkGridMajorY->setChecked(g.gridMajorY);
            ui->chkGridMinorX->setChecked(g.gridMinorX);
            ui->chkGridMinorY->setChecked(g.gridMinorY);


            ui->cmbKeyLayout->setCurrentIndex(g.keyLayout);

            ui->chkShowTitle->setChecked(g.showTitle);
            ui->spinKeyXMargin->setValue(g.keyXMargin);
            ui->spinKeyYMargin->setValue(g.keyYMargin);
            ui->spinKeyXOffset->setValue(g.keyXOffset);
            ui->spinKeyYOffset->setValue(g.keyYOffset);
            ui->spinKeyXSeparation->setValue(g.keyXSeparation);
            ui->spinKeyYSeparation->setValue(g.keyYSeparation);
            ui->spinKeyLineLength->setValue(g.key_line_length);

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
        p.grid=ui->grpMajor->isChecked();
        p.gridMinor=ui->grpMinor->isChecked();
        p.showKey=ui->chkShowKey->isChecked();

        p.gridMajorX=ui->chkGridMajorX->isChecked();
        p.gridMajorY=ui->chkGridMajorY->isChecked();
        p.gridMinorX=ui->chkGridMinorX->isChecked();
        p.gridMinorY=ui->chkGridMinorY->isChecked();

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
        p.keyBoxLineWidth=ui->spinKeyBoxWidth->value();
        p.keyBackgroundColor=ui->cmbKeyBackground->currentColor();
        p.keyLineColor=ui->cmbKeyBoxColor->currentColor();
        p.keyBox=ui->chkKeyBox->isChecked();


        p.showTitle=ui->chkShowTitle->isChecked();
        p.keyXMargin=ui->spinKeyXMargin->value();
        p.keyYMargin=ui->spinKeyYMargin->value();
        p.keyXOffset=ui->spinKeyXOffset->value();
        p.keyYOffset=ui->spinKeyYOffset->value();
        p.keyXSeparation=ui->spinKeyXSeparation->value();
        p.keyYSeparation=ui->spinKeyYSeparation->value();
        p.key_line_length=ui->spinKeyLineLength->value();



        ui->axisX->storePlotData(p.xAxis);
        ui->axisY->storePlotData(p.yAxis);


        p.backgroundColor=ui->cmbBackgroundColor->currentColor();
        p.gridColor=ui->cmbGridColor->currentColor();
        p.gridStyle=ui->cmbGridLinestyle->currentLineStyle();
        p.gridWidth=ui->spinGridWidth->value();
        p.gridColorMinor=ui->cmbGridColorMinor->currentColor();
        p.gridStyleMinor=ui->cmbGridLinestyleMinor->currentLineStyle();
        p.gridWidthMinor=ui->spinGridWidthMinor->value();
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
    connect(ui->grpMajor, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->grpMinor, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
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

    connect(ui->cmbBackgroundColor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbGridColor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbGridLinestyle, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->spinGridWidth, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->cmbGridColorMinor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbGridLinestyleMinor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->spinGridWidthMinor, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));


    connect(ui->chkGridMajorX, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->chkGridMajorY, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->chkGridMinorX, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    connect(ui->chkGridMinorY, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));


    connect(ui->cmbKeyBackground, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->cmbKeyBoxColor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    connect(ui->spinKeyBoxWidth, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->chkKeyBox, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));


    connect(ui->spinKeyXMargin, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->spinKeyYMargin, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->spinKeyXOffset, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->spinKeyYOffset, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->spinKeyXSeparation, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->spinKeyYSeparation, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->spinKeyLineLength, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    connect(ui->chkShowTitle, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));



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

    disconnect(ui->chkGridMajorX, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkGridMajorY, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkGridMinorX, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkGridMinorY, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));

    disconnect(ui->edtTitle, SIGNAL(editingFinished()), this, SLOT(plotDataChanged()));
    disconnect(ui->grpMajor, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkShowKey, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->grpMinor, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
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
    disconnect(ui->cmbBackgroundColor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbGridColor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbGridLinestyle, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinGridWidth, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbGridColorMinor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbGridLinestyleMinor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinGridWidthMinor, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbKeyBackground, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->cmbKeyBoxColor, SIGNAL(currentIndexChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinKeyBoxWidth, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkKeyBox, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinKeyXMargin, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinKeyYMargin, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinKeyXOffset, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinKeyYOffset, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinKeyXSeparation, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinKeyYSeparation, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinKeyLineLength, SIGNAL(valueChanged(double)), this, SLOT(plotDataChanged()));
    disconnect(ui->chkShowTitle, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));

    disconnect(ui->chkPlotAutosize, SIGNAL(toggled(bool)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinHeight, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinWidth, SIGNAL(valueChanged(int)), this, SLOT(plotDataChanged()));
    disconnect(ui->spinHeight, SIGNAL(editingFinished()), this, SLOT(plotDataChanged()));
    disconnect(ui->spinWidth, SIGNAL(editingFinished()), this, SLOT(plotDataChanged()));

}

void QFRDRTablePlotSettingsWidget::rereadCurrentData()
{
    setRecord(current, plot);
}


void QFRDRTablePlotSettingsWidget::on_btnSaveSystem_clicked() {
    if (!current) return;
    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/table/graph_templates/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFRDRTablePlotSettingsWidget/lasttemplatedir", ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/table/graph_templates/").toString();
    QString filename=qfGetSaveFileName(this, tr("save plot settings template ..."), dir, tr("plot settings template  (*.gxt)"))    ;
    if (!filename.isEmpty()) {
        bool ok=true;
        if (QFile::exists(filename)) {
            ok=false;
            if (QMessageBox::question(this, tr("save plot settings template ..."), tr("The file\n  '%1'\nalready exists. Overwrite?").arg(filename), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {
                ok=true;
            }
        }
        if (ok) {
            QFile f(filename);
            if (f.open(QFile::WriteOnly)) {
                QXmlStreamWriter stream(&f);
                stream.setAutoFormatting(true);
                stream.writeStartDocument();
                stream.writeStartElement("graph_templates");
                bool saveGraphs=QMessageBox::question(this, tr("save plot settings template ..."), tr("Should the graphs also be saved to the template?\n   [Yes]: Saves the graphs in this plot\n   [No]: Saves only the settings of the coordinate system").arg(filename), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes;
                current->writePlotInfo(stream, current->getPlot(plot), saveGraphs);
                stream.writeEndElement();
                stream.writeEndDocument();
                f.close();
            }



        }
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRTablePlotSettingsWidget/lasttemplatedir", dir);
}





void QFRDRTablePlotSettingsWidget::on_btnLoadSystem_clicked() {
    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/table/graph_templates/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFRDRTablePlotSettingsWidget/lasttemplatedir", ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/table/graph_templates/").toString();
    QString filename=qfGetOpenFileName(this, tr("open plot settings template ..."), dir, tr("plot settings template (*.gxt *.gst)"))    ;
    if (!filename.isEmpty()) {
        if (filename.toLower().endsWith(".gst")) {
            QSettings set(filename, QSettings::IniFormat);

            ui->axisX->loadFromSettings(set, "x");
            ui->axisY->loadFromSettings(set, "y");

            ui->chkPlotAutosize->setChecked(set.value("autosize",ui->chkPlotAutosize->isChecked()).toBool());
            ui->spinWidth->setValue(set.value("plotwidth", ui->spinWidth->value()).toInt());
            ui->spinHeight->setValue(set.value("plotheight", ui->spinHeight->value()).toInt());

            ui->edtTitle->setText(set.value("title", ui->edtTitle->text()).toString());
            ui->grpMajor->setChecked(set.value("grid",ui->grpMajor->isChecked()).toBool());
            ui->grpMinor->setChecked(set.value("gridMinor",ui->grpMinor->isChecked()).toBool());
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
            ui->cmbBackgroundColor->setCurrentColor(QColor(set.value("backgroundColor", QColor2String(ui->cmbBackgroundColor->currentColor())).toString()));
            ui->cmbGridColor->setCurrentColor(QColor(set.value("gridColor", QColor2String(ui->cmbGridColor->currentColor())).toString()));
            ui->cmbGridLinestyle->setCurrentLineStyle(String2QPenStyle(set.value("gridStyle", QPenStyle2String(ui->cmbGridLinestyle->currentLineStyle())).toString()));
            ui->spinGridWidth->setValue(set.value("gridWidth", ui->spinGridWidth->value()).toDouble());
            ui->cmbGridColorMinor->setCurrentColor(QColor(set.value("gridColorMinor", QColor2String(ui->cmbGridColorMinor->currentColor())).toString()));
            ui->cmbGridLinestyleMinor->setCurrentLineStyle(String2QPenStyle(set.value("gridStyleMinor", QPenStyle2String(ui->cmbGridLinestyleMinor->currentLineStyle())).toString()));
            ui->spinGridWidthMinor->setValue(set.value("gridWidthMinor", ui->spinGridWidthMinor->value()).toDouble());


            ui->chkKeyBox->setChecked(set.value("keyBox", ui->chkKeyBox->isChecked()).toBool());
            ui->cmbKeyBoxColor->setCurrentColor(QColor(set.value("keyBoxColor", QColor2String(ui->cmbKeyBoxColor->currentColor())).toString()));
            ui->cmbKeyBackground->setCurrentColor(QColor(set.value("keyBackgroundColor", QColor2String(ui->cmbKeyBackground->currentColor())).toString()));
            ui->spinKeyBoxWidth->setValue(set.value("keyBoxWidth", ui->spinKeyBoxWidth->value()).toDouble());
            updateGraph();
        } else {
            QDomDocument doc("mydocument");
             QFile file(filename);
             if (!file.open(QIODevice::ReadOnly))
                 return;
             if (!doc.setContent(&file)) {
                 file.close();
                 return;
             }
             file.close();

             // print out the element names of all elements that are direct children
             // of the outermost element.
             QDomElement docElem = doc.documentElement();
             if (docElem.tagName().toLower()=="graph_templates") {
                 QDomElement te=docElem.firstChildElement("plot");
                 while(!te.isNull()) {
                     QFRDRTable::PlotInfo pi=current->getPlot(plot);
                     current->readPlotInfo(pi, te);
                     current->setPlot(plot, pi);
                     te = te.nextSiblingElement("plot");
                 }
             }
             rereadCurrentData();
             emit plotSettingsChanged();
        }

    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRTablePlotSettingsWidget/lasttemplatedir", dir);
}


void QFRDRTablePlotSettingsWidget::on_btnCopySystem_clicked()
{
    QString s;
    {
        QXmlStreamWriter stream(&s);
        stream.setAutoFormatting(true);
        stream.writeStartDocument();
        stream.writeStartElement("graph_templates");
        bool saveGraphs=false;
        current->writePlotInfo(stream, current->getPlot(plot), saveGraphs);
        stream.writeEndElement();
        stream.writeEndDocument();
    }
    QClipboard *clipboard = QApplication::clipboard();
    QMimeData* mime=new QMimeData();
    mime->setData("quickfit3/qfrdrtable/graph_system", s.toUtf8());
    clipboard->setMimeData(mime);
}

void QFRDRTablePlotSettingsWidget::on_btnPasteSystem_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData* mime=clipboard->mimeData();
    if (mime && mime->hasFormat("quickfit3/qfrdrtable/graph_system")) {
        QString s=QString::fromUtf8(mime->data("quickfit3/qfrdrtable/graph_system"));
        QDomDocument doc("mydocument");
        doc.setContent(s);
        QDomElement docElem = doc.documentElement();
        if (docElem.tagName().toLower()=="graph_templates") {
            QDomElement te=docElem.firstChildElement("plot");
            if(!te.isNull()) {
                QFRDRTable::PlotInfo pi=current->getPlot(plot);
                current->readPlotInfo(pi, te);
                current->setPlot(plot, pi);
                te = te.nextSiblingElement("plot");
            }
        }
    }
    rereadCurrentData();
    emit plotSettingsChanged();
}

void QFRDRTablePlotSettingsWidget::on_btnAutoscaleXY_clicked()
{
    doAutoscaleX();
    doAutoscaleY();
}
