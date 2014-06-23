#include "qfoverlayplotdialog.h"
#include "ui_qfoverlayplotdialog.h"
#include "programoptions.h"
#include "qfpluginservices.h"
#include "qftools.h"

QFOverlayPlotDialog::QFOverlayPlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFOverlayPlotDialog)
{
    ui->setupUi(this);
    ui->plot->set_toolbarAlwaysOn(true);

    QSettings* set=ProgramOptions::getInstance()->getQSettings();
    if (set) {
        loadWidgetGeometry(*set, this, pos(), size(), "QFOverlayPlotDialog/windowsize");
    }

}

QFOverlayPlotDialog::~QFOverlayPlotDialog()
{
    QSettings* set=ProgramOptions::getInstance()->getQSettings();
    if (set) {
        saveWidgetGeometry(*set, this, "QFOverlayPlotDialog/windowsize");
    }

    delete ui;
}

QFPlotter *QFOverlayPlotDialog::getPlotter() const
{
    return ui->plot;
}

void QFOverlayPlotDialog::clearPlots()
{
    ui->plot->clearGraphs();
    ui->plot->getDatastore()->clear();
}

void QFOverlayPlotDialog::startAddingPlots()
{
    ui->plot->set_doDrawing(false);
}

void QFOverlayPlotDialog::addPlot(const QVector<double> &x, const QVector<double> &y, const QString &name, const QVector<double> &xerror, const QVector<double> &yerror)
{
    JKQtBasePlotter* plt=ui->plot->get_plotter();
    JKQTPdatastore* ds=plt->getDatastore();
    if (x.size()>0 && y.size()>0) {
        JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(plt);
        g->set_title(name);
        g->set_xColumn(ds->addCopiedColumn(x.data(), x.size(), name+": X"));
        g->set_yColumn(ds->addCopiedColumn(y.data(), y.size(), name+": Y"));
        g->set_symbol(JKQTPnoSymbol);
        g->set_drawLine(true);
        if (xerror.size()>0) {
            g->set_xErrorColumn(ds->addCopiedColumn(xerror.data(), xerror.size(), name+": XErr"));
            g->set_xErrorStyle(JKQTPerrorSimpleBars);
        }
        if (yerror.size()>0) {
            g->set_yErrorColumn(ds->addCopiedColumn(yerror.data(), yerror.size(), name+": YErr"));
            g->set_yErrorStyle(JKQTPerrorSimpleBars);
        }
        plt->addGraph(g);
    }
}

void QFOverlayPlotDialog::endAddingPlots()
{
    ui->plot->set_doDrawing(true);
    ui->plot->zoomToFit();
}

void QFOverlayPlotDialog::setLog(bool logX, bool logY)
{
    ui->plot->getXAxis()->set_logAxis(logX);
    ui->plot->getYAxis()->set_logAxis(logY);
}

void QFOverlayPlotDialog::setAxisLabel(const QString &labX, const QString &labY)
{
    ui->plot->getXAxis()->set_axisLabel(labX);
    ui->plot->getYAxis()->set_axisLabel(labY);
}

void QFOverlayPlotDialog::showHelp()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("qfoverlayplot.html");
}

void QFOverlayPlotDialog::closeClicked()
{
    accept();
    deleteLater();
}

void QFOverlayPlotDialog::updatePlot()
{
    startAddingPlots();
    ui->plot->get_plotter()->set_showKey(ui->chkKey->isChecked());
    for (size_t i=0; i<ui->plot->get_plotter()->getGraphCount(); i++) {
        JKQTPgraph* g=ui->plot->get_plotter()->getGraph(i);
        JKQTPxyGraphErrors* eg=dynamic_cast<JKQTPxyGraphErrors*>(g);
        if (eg) {
            if (ui->chkErrors->isChecked()) {
                eg->set_yErrorStyle(JKQTPerrorSimpleBars);
                eg->set_xErrorStyle(JKQTPerrorSimpleBars);
            } else {
                eg->set_yErrorStyle(JKQTPnoError);
                eg->set_xErrorStyle(JKQTPnoError);
            }
        }
    }
    endAddingPlots();
}
