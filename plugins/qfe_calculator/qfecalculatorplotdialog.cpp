#include "qfecalculatorplotdialog.h"
#include "ui_qfecalculatorplotdialog.h"
#include "programoptions.h"
#include "qfpluginservices.h"
#include "qftools.h"

QFECalculatorPlotDialog::QFECalculatorPlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFECalculatorPlotDialog)
{
    ui->setupUi(this);
    ui->plot->set_toolbarAlwaysOn(true);

    QSettings* set=ProgramOptions::getInstance()->getQSettings();
    if (set) {
        loadWidgetGeometry(*set, this, pos(), size(), "QFECalculatorPlotDialog/windowsize");
    }

}

QFECalculatorPlotDialog::~QFECalculatorPlotDialog()
{
    QSettings* set=ProgramOptions::getInstance()->getQSettings();
    if (set) {
        saveWidgetGeometry(*set, this, "QFECalculatorPlotDialog/windowsize");
    }

    delete ui;
}

void QFECalculatorPlotDialog::showHelp()
{
    QFPluginServices::getInstance()->displayPluginHelpWindow("qfe_calculator", "plot.html");
}

void QFECalculatorPlotDialog::closeClicked()
{
    accept();
    deleteLater();
}

void QFECalculatorPlotDialog::updatePlot()
{
    startAddingPlots();
    endAddingPlots();
}

void QFECalculatorPlotDialog::on_chkLogX_toggled(bool checked)
{
    setLogX(checked);
}

void QFECalculatorPlotDialog::on_chkLogY_toggled(bool checked)
{
    setLogY(checked);
}


QFPlotter *QFECalculatorPlotDialog::getPlotter() const
{
    return ui->plot;
}

void QFECalculatorPlotDialog::clearPlots()
{
    ui->plot->clearGraphs();
    ui->plot->getDatastore()->clear();
}

void QFECalculatorPlotDialog::startAddingPlots()
{
    ui->plot->set_doDrawing(false);
}


void QFECalculatorPlotDialog::endAddingPlots()
{
    ui->plot->set_doDrawing(true);
    ui->plot->zoomToFit();
}

void QFECalculatorPlotDialog::setLog(bool logX, bool logY)
{
    ui->plot->getXAxis()->set_logAxis(logX);
    ui->plot->getYAxis()->set_logAxis(logY);
}

void QFECalculatorPlotDialog::setAxisLabel(const QString &labX, const QString &labY)
{
    ui->plot->getXAxis()->set_axisLabel(labX);
    ui->plot->getYAxis()->set_axisLabel(labY);
}

void QFECalculatorPlotDialog::setLogX(bool log)
{
    ui->plot->getXAxis()->set_logAxis(log);
    ui->plot->zoomToFit();
}

void QFECalculatorPlotDialog::setLogY(bool log)
{
    ui->plot->getYAxis()->set_logAxis(log);
    ui->plot->zoomToFit();
}

void QFECalculatorPlotDialog::setXAxisLabel(const QString &label)
{
    ui->plot->getXAxis()->set_axisLabel(label);
}

void QFECalculatorPlotDialog::setYAxisLabel(const QString &label)
{
    ui->plot->getYAxis()->set_axisLabel(label);
}


#define PARSE_SPEC(spec, g) {\
    g->set_symbol(JKQTPcircle);\
    g->set_drawLine(false);\
    if (spec.contains("--")) {\
        g->set_drawLine(true);\
        g->set_style(Qt::DashLine);\
    } else if (spec.contains("-")) {\
        g->set_drawLine(true);\
        g->set_style(Qt::SolidLine);\
    } else if (spec.contains(":")) {\
        g->set_drawLine(true);\
        g->set_style(Qt::DotLine);\
    }\
    if (spec.startsWith("r")) {\
        g->set_color(QColor("red"));\
    } else if (spec.startsWith("g")) {\
        g->set_color(QColor("green"));\
    } else if (spec.startsWith("b")) {\
        g->set_color(QColor("blue"));\
    } else if (spec.startsWith("y")) {\
        g->set_color(QColor("yellow"));\
    } else if (spec.startsWith("m")) {\
        g->set_color(QColor("magenta"));\
    } else if (spec.startsWith("k")) {\
        g->set_color(QColor("black"));\
    } else if (spec.startsWith("c")) {\
        g->set_color(QColor("cyan"));\
    }\
    if (spec.contains("o")) {\
        g->set_symbol(JKQTPcircle);\
    } else if (spec.contains("x")) {\
        g->set_symbol(JKQTPcross);\
    } else if (spec.contains("+")) {\
        g->set_symbol(JKQTPplus);\
    } else if (spec.contains("*")) {\
        g->set_symbol(JKQTPstar);\
    } else if (spec.contains("'")) {\
        g->set_symbol(JKQTPdot);\
    } else if (spec.size()>0){\
        g->set_symbol(JKQTPnoSymbol);\
    }\
    g->set_errorColor(g->get_color().darker());\
    g->set_fillColor(g->get_color().lighter());\
    g->set_lineWidth(1);\
    }

void QFECalculatorPlotDialog::addPlot(const QVector<double> &X, const QVector<double> &Y, const QString &spec, const QString& label)
{
    JKQtBasePlotter* plt=ui->plot->get_plotter();
    JKQTPdatastore* ds=plt->getDatastore();
    if (X.size()>0 && Y.size()>0) {
        QString ilabel=label;
        if (ilabel.isEmpty()) ilabel=QString("plot%1").arg(plt->getGraphCount()+1);
        JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(plt);
        if (!label.isEmpty()) g->set_title(label);
        g->set_xColumn(ds->addCopiedColumn(X.data(), X.size(), ilabel+": X"));
        g->set_yColumn(ds->addCopiedColumn(Y.data(), Y.size(), ilabel+": Y"));

        PARSE_SPEC(spec, g)

        /*if (Xerrors.size()>0) {
            g->set_xErrorColumn(ds->addCopiedColumn(Xerrors.data(), Xerrors.size(), ilabel+": XErr"));
            g->set_xErrorStyle(ui->cmbErrorStyleX->getErrorStyle());
        }
        if (Yerrors.size()>0) {
            g->set_yErrorColumn(ds->addCopiedColumn(Yerrors.data(), Yerrors.size(), ilabel+": YErr"));
            g->set_yErrorStyle(ui->cmbErrorStyleY->getErrorStyle());
        }*/
        plt->addGraph(g);
    }

}
