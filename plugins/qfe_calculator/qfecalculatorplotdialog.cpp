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
    ui->plot->update_plot();
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


#define PARSE_COLOR_SPEC(spec, g) {\
    QRegExp rxRGB("rgb\\(\\s(\\d+)\\s\\,\\s(\\d+)\\s\\,\\s(\\d+)\\s\\)"); \
    if (rxRGB.indexIn(spec)==0) {\
        g->set_color(QColor(rxRGB.cap(1).toInt(),rxRGB.cap(2).toInt(),rxRGB.cap(3).toInt()));\
    } else if (spec.startsWith("r")) {\
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
    g->set_errorColor(g->get_color().darker());\
    g->set_fillColor(g->get_color().lighter());\
    }

#define PARSE_SPEC(spec, g) {\
    g->set_symbol(JKQTPcircle);\
    g->set_drawLine(false);\
    if (spec.contains("--")) {\
        g->set_drawLine(true);\
        g->set_style(Qt::DashLine);\
    } else if (spec.contains("-..")) {\
        g->set_drawLine(true);\
        g->set_style(Qt::DashDotDotLine);\
    } else if (spec.contains("-.")) {\
        g->set_drawLine(true);\
        g->set_style(Qt::DashDotLine);\
    } else if (spec.contains("-")) {\
        g->set_drawLine(true);\
        g->set_style(Qt::SolidLine);\
    } else if (spec.contains(":")) {\
        g->set_drawLine(true);\
        g->set_style(Qt::DotLine);\
    }\
    PARSE_COLOR_SPEC(spec,g)\
    if (spec.contains("o")) {\
        g->set_symbol(JKQTPcircle);\
    } else if (spec.contains("x")) {\
        g->set_symbol(JKQTPcross);\
    } else if (spec.contains("s")) {\
        g->set_symbol(JKQTPrect);\
    } else if (spec.contains("d")) {\
        g->set_symbol(JKQTPdiamond);\
    } else if (spec.contains("^")) {\
        g->set_symbol(JKQTPtriangle);\
    } else if (spec.contains("v")) {\
        g->set_symbol(JKQTPdownTriangle);\
    } else if (spec.contains("+")) {\
        g->set_symbol(JKQTPplus);\
    } else if (spec.contains("*")) {\
        g->set_symbol(JKQTPstar);\
    } else if (spec.contains("'")) {\
        g->set_symbol(JKQTPdot);\
    } else if (spec.size()>0){\
        g->set_symbol(JKQTPnoSymbol);\
    }\
    g->set_lineWidth(1);\
    }

#define PARSE_SPEC_ERROR(spec, es) \
    if (spec.contains("EB")) {\
        es=JKQTPerrorBars; \
    } else if (spec.contains("EL")) {\
        es=JKQTPerrorLines; \
    } else if (spec.contains("EP")) {\
        es=JKQTPerrorPolygons; \
    }\


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


        plt->addGraph(g);
    }

}

void QFECalculatorPlotDialog::addBarPlot(const QVector<double> &X, const QVector<double> &Y, const QString &spec, const QString &label)
{
    JKQtBasePlotter* plt=ui->plot->get_plotter();
    JKQTPdatastore* ds=plt->getDatastore();
    if (X.size()>0 && Y.size()>0) {
        QString ilabel=label;
        if (ilabel.isEmpty()) ilabel=QString("plot%1").arg(plt->getGraphCount()+1);
        JKQTPbarHorizontalGraph* g=new JKQTPbarHorizontalGraph(plt);
        if (!label.isEmpty()) g->set_title(label);
        g->set_xColumn(ds->addCopiedColumn(X.data(), X.size(), ilabel+": X"));
        g->set_yColumn(ds->addCopiedColumn(Y.data(), Y.size(), ilabel+": Y"));

        PARSE_COLOR_SPEC(spec, g)

        g->set_fillColor(g->get_color());
        g->set_color(g->get_fillColor().darker());


        plt->addGraph(g);
    }

}

void QFECalculatorPlotDialog::addErrorPlot(const QVector<double> &X, const QVector<double> &Y, const QVector<double> &Xerrors, const QVector<double> &Yerrors, const QString &spec, const QString &label, JKQTPerrorPlotstyle errorstyle)
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

        JKQTPerrorPlotstyle es=errorstyle;
        PARSE_SPEC(spec, g)
        PARSE_SPEC_ERROR(spec, es)
        QColor c=g->get_color().darker();
        g->set_errorColor(c);
        c.setAlphaF(0.5);
        g->set_errorFillColor(c);

        if (Xerrors.size()>0) {
            g->set_xErrorColumn(ds->addCopiedColumn(Xerrors.data(), Xerrors.size(), ilabel+": XErr"));
            g->set_xErrorStyle(es);
        }
        if (Yerrors.size()>0) {
            g->set_yErrorColumn(ds->addCopiedColumn(Yerrors.data(), Yerrors.size(), ilabel+": YErr"));
            g->set_yErrorStyle(es);
        }
        plt->addGraph(g);
    }
}

void QFECalculatorPlotDialog::addScatterPlot(const QVector<double> &X, const QVector<double> &Y, const QVector<double> &S, const QString &spec, const QString &label)
{
    JKQtBasePlotter* plt=ui->plot->get_plotter();
    JKQTPdatastore* ds=plt->getDatastore();
    if (X.size()>0 && Y.size()>0) {
        QString ilabel=label;
        if (ilabel.isEmpty()) ilabel=QString("plot%1").arg(plt->getGraphCount()+1);
        JKQTPxyParametrizedErrorScatterGraph* g=new JKQTPxyParametrizedErrorScatterGraph(plt);
        if (!label.isEmpty()) g->set_title(label);
        g->set_xColumn(ds->addCopiedColumn(X.data(), X.size(), ilabel+": X"));
        g->set_yColumn(ds->addCopiedColumn(Y.data(), Y.size(), ilabel+": Y"));
        g->set_sizeColumn(ds->addCopiedColumn(S.data(), S.size(), ilabel+": S"));

        PARSE_SPEC(spec, g)


        plt->addGraph(g);
    }
}

void QFECalculatorPlotDialog::addColorScatterPlot(const QVector<double> &X, const QVector<double> &Y, const QVector<double> &S, const QString &spec, const QString &label, const QString &colorscale)
{
    JKQtBasePlotter* plt=ui->plot->get_plotter();
    JKQTPdatastore* ds=plt->getDatastore();
    if (X.size()>0 && Y.size()>0) {
        QString ilabel=label;
        if (ilabel.isEmpty()) ilabel=QString("plot%1").arg(plt->getGraphCount()+1);
        JKQTPxyParametrizedErrorScatterGraph* g=new JKQTPxyParametrizedErrorScatterGraph(plt);
        if (!label.isEmpty()) g->set_title(label);
        g->set_xColumn(ds->addCopiedColumn(X.data(), X.size(), ilabel+": X"));
        g->set_yColumn(ds->addCopiedColumn(Y.data(), Y.size(), ilabel+": Y"));
        g->set_colorColumn(ds->addCopiedColumn(S.data(), S.size(), ilabel+": S"));

        PARSE_SPEC(spec, g)
        JKQTPMathImageColorPalette pal=JKQTPMathImageMATLAB;
        QStringList pals=JKQTPimagePlot_getPredefinedPalettes();
        for (int i=0; i<pals.size(); i++) {
            if (colorscale.toLower().trimmed().simplified()==pals[i].toLower().simplified().trimmed()) {
                pal=(JKQTPMathImageColorPalette)i;
            }
        }
        g->set_palette(pal);

        plt->addGraph(g);
    }
}

void QFECalculatorPlotDialog::fitRanges()
{
    ui->plot->zoomToFit();
}
