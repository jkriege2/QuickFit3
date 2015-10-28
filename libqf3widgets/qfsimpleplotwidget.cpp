#include "qfsimpleplotwidget.h"
#include "ui_qfsimpleplotwidget.h"
#include "programoptions.h"
#include "qfpluginservices.h"
#include "qftools.h"
#include "qfmathparserxfunctionlinegraph.h"
#include "qffitfunctionplottools.h"



#define V_PARSE_COLOR_SPEC(spec, c) {\
    QRegExp rxRGB("rgb\\(\\s*(\\d+)\\s*\\,\\s*(\\d+)\\s*\\,\\s*(\\d+)\\s*\\)"); \
    QRegExp rxRGBA("rgba\\(\\s*(\\d+)\\s*\\,\\s*(\\d+)\\s*\\,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*\\)"); \
    QRegExp rxGray("gray\\(\\s*(\\d+)\\s*\\)"); \
    QRegExp rxGrayA("gray\\(\\s*(\\d+)\\s*\\,\\s*(\\d+)\\s*\\)"); \
    if (rxRGB.indexIn(spec)==0) {\
        c=QColor(rxRGB.cap(1).toInt(),rxRGB.cap(2).toInt(),rxRGB.cap(3).toInt());\
    } else if (rxRGBA.indexIn(spec)==0) {\
        c=QColor(rxRGBA.cap(1).toInt(),rxRGBA.cap(2).toInt(),rxRGBA.cap(3).toInt(),rxRGBA.cap(4).toInt());\
    } else if (rxGray.indexIn(spec)==0) {\
        c=QColor(rxGray.cap(1).toInt(),rxGray.cap(1).toInt(),rxGray.cap(1).toInt());\
    } else if (rxGrayA.indexIn(spec)==0) {\
        c=QColor(rxGrayA.cap(1).toInt(),rxGrayA.cap(1).toInt(),rxGrayA.cap(1).toInt(),rxGrayA.cap(2).toInt());\
    } else if (spec.startsWith("r")) {\
        c=QColor("red");\
    } else if (spec.startsWith("g")) {\
        c=QColor("green");\
    } else if (spec.startsWith("b")) {\
        c=QColor("blue");\
    } else if (spec.startsWith("y")) {\
        c=QColor("yellow");\
    } else if (spec.startsWith("m")) {\
        c=QColor("magenta");\
    } else if (spec.startsWith("k")) {\
        c=QColor("black");\
    } else if (spec.startsWith("c")) {\
        c=QColor("cyan");\
    }\
    }

#define V_PARSE_LINE_SPEC(spec, le, ls, lw) {\
    le=false;\
    if (spec.contains("--")) {\
        le=true;\
        ls=Qt::DashLine;\
    } else if (spec.contains("-..") || spec.contains("-:")) {\
        le=true;\
        ls=Qt::DashDotDotLine;\
    } else if (spec.contains("-.")) {\
        le=true;\
        ls=Qt::DashDotLine;\
    } else if (spec.contains("-")) {\
        le=true;\
        ls=Qt::SolidLine;\
    } else if (spec.contains(":")) {\
        le=true;\
        ls=Qt::DotLine;\
    }\
    lw=1;\
    QRegExp rxLW("lw\\(\\s*(\\d+\\.?\\d*)\\s*\\)"); \
    if (rxLW.indexIn(spec)>=0) {\
        lw=rxLW.cap(1).toDouble();\
    } else if (spec.contains("lw2")) {\
        lw=2;\
    } else if (spec.contains("lw3")) {\
        lw=3;\
    }\
    }


#define PARSE_COLOR_SPEC(spec, g) {\
    QColor c=g->get_color();\
    V_PARSE_COLOR_SPEC(spec, c)\
    g->set_color(c); \
    g->set_errorColor(g->get_color().darker());\
    g->set_fillColor(g->get_color().lighter());\
    }

#define PARSE_SYMBOL_SPEC(spec, g) {\
    g->set_symbol(JKQTPcircle);\
    if (spec.contains("o")) {\
        g->set_symbol(JKQTPcircle);\
        if (spec.contains("of")) g->set_symbol(JKQTPfilledCircle);\
    } else if (spec.contains("x")) {\
        g->set_symbol(JKQTPcross);\
    } else if (spec.contains("s")) {\
        g->set_symbol(JKQTPrect);\
        if (spec.contains("sf")) g->set_symbol(JKQTPfilledRect);\
    } else if (spec.contains("d")) {\
        g->set_symbol(JKQTPdiamond);\
        if (spec.contains("df")) g->set_symbol(JKQTPfilledDiamond);\
    } else if (spec.contains("^")) {\
        g->set_symbol(JKQTPtriangle);\
        if (spec.contains("^f")) g->set_symbol(JKQTPfilledTriangle);\
    } else if (spec.contains("v")) {\
        g->set_symbol(JKQTPdownTriangle);\
        if (spec.contains("vf")) g->set_symbol(JKQTPfilledDownTriangle);\
    } else if (spec.contains("+")) {\
        g->set_symbol(JKQTPplus);\
    } else if (spec.contains("*")) {\
        g->set_symbol(JKQTPstar);\
        if (spec.contains("*f")) g->set_symbol(JKQTPfilledStar);\
    } else if (spec.contains("'")) {\
        g->set_symbol(JKQTPdot);\
    } else if (spec.size()>0){\
        g->set_symbol(JKQTPnoSymbol);\
    }\
     QRegExp rxSS("SS\\(\\s*(\\d+\\.?\\d*)\\s*\\)"); \
    if (rxSS.indexIn(spec)>=0) {\
        g->set_symbolSize(rxSS.cap(1).toDouble());\
    }\
    }

#define PARSE_SPEC_NOSYMBOL(spec, g) {\
    bool le=g->get_drawLine();\
    double lw=g->get_lineWidth(); \
    Qt::PenStyle ls=g->get_style();\
    V_PARSE_LINE_SPEC(spec,le,ls,lw)\
    g->set_drawLine(le);\
    g->set_style(ls);\
    g->set_lineWidth(lw);\
    PARSE_COLOR_SPEC(spec,g)\
    }
#define PARSE_SPEC(spec, g) \
    PARSE_SPEC_NOSYMBOL(spec, g)\
    PARSE_SYMBOL_SPEC(spec,g)

#define PARSE_SPEC_ERROR(spec, es) \
    if (spec.contains("EB")) {\
        es=JKQTPerrorBars; \
    } else if (spec.contains("EL")) {\
        es=JKQTPerrorLines; \
    } else if (spec.contains("EP")) {\
        es=JKQTPerrorPolygons; \
    } else if (spec.contains("ES")) {\
        es=JKQTPerrorSimpleBars; \
    } else if (spec.contains("EE")) {\
        es=JKQTPerrorEllipses; \
    }\








QFSimplePlotWidget::QFSimplePlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFSimplePlotWidget)
{
    ui->setupUi(this);
    ui->plot->set_toolbarAlwaysOn(true);

    QSettings* set=ProgramOptions::getInstance()->getQSettings();
    if (set) {
        loadWidgetGeometry(*set, this, pos(), size(), "QFSimplePlotWidget/windowsize");
    }

}

QFSimplePlotWidget::~QFSimplePlotWidget()
{
    QSettings* set=ProgramOptions::getInstance()->getQSettings();
    if (set) {
        saveWidgetGeometry(*set, this, "QFSimplePlotWidget/windowsize");
    }

    delete ui;
}

void QFSimplePlotWidget::showHelp()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("qf3_simpleplotwidget.html");
}


void QFSimplePlotWidget::updatePlot()
{
    startAddingPlots();
    endAddingPlots();
}

void QFSimplePlotWidget::on_chkLogX_toggled(bool checked)
{
    setLogX(checked);
}

void QFSimplePlotWidget::on_chkLogY_toggled(bool checked)
{
    setLogY(checked);
}

void QFSimplePlotWidget::on_chkGrid_toggled(bool checked)
{
    setGrid(checked);
}

void QFSimplePlotWidget::emitLogX(bool log)
{
    emit logXtoggled(log);
}

void QFSimplePlotWidget::emitLogY(bool log)
{
    emit logYtoggled(log);
}


QFPlotter *QFSimplePlotWidget::getPlotter() const
{
    return ui->plot;
}

void QFSimplePlotWidget::clearPlots()
{
    ui->plot->clearGraphs();
    ui->plot->getDatastore()->clear();
}

void QFSimplePlotWidget::startAddingPlots()
{
    ui->plot->set_doDrawing(false);
}


void QFSimplePlotWidget::endAddingPlots()
{
    ui->plot->set_doDrawing(true);
    ui->plot->update_plot();
}

void QFSimplePlotWidget::setLog(bool logX, bool logY)
{
    ui->plot->getXAxis()->set_logAxis(logX);
    ui->plot->getYAxis()->set_logAxis(logY);
    ui->chkLogX->setChecked(logX);
    ui->chkLogY->setChecked(logY);
    emit logXtoggled(logX);
    emit logYtoggled(logY);
}

void QFSimplePlotWidget::setXRange(double rmin, double rmax)
{
    ui->plot->getXAxis()->setRange(rmin, rmax);
}

void QFSimplePlotWidget::setYRange(double rmin, double rmax)
{
    ui->plot->getYAxis()->setRange(rmin, rmax);
}

void QFSimplePlotWidget::setAxisLabel(const QString &labX, const QString &labY)
{
    ui->plot->getXAxis()->set_axisLabel(labX);
    ui->plot->getYAxis()->set_axisLabel(labY);
}

void QFSimplePlotWidget::setLogX(bool log)
{
    ui->plot->getXAxis()->set_logAxis(log);
    ui->plot->zoomToFit();
    ui->chkLogX->setChecked(log);
    emit logXtoggled(log);
}

void QFSimplePlotWidget::setLogY(bool log)
{
    ui->plot->getYAxis()->set_logAxis(log);
    ui->plot->zoomToFit();
    ui->chkLogY->setChecked(log);
    emit logYtoggled(log);
}

void QFSimplePlotWidget::setGrid(bool enabled, const QString& style)
{
    if (!style.isEmpty()) {
        Qt::PenStyle ls=ui->plot->getXAxis()->get_gridStyle();
        double lw=ui->plot->getXAxis()->get_gridWidth();
        QColor lc=ui->plot->getXAxis()->get_gridColor();
        bool le=true;
        V_PARSE_LINE_SPEC(style,le,ls,lw)
        if (!le || ls==Qt::NoPen) ls=Qt::SolidLine;
        ui->plot->getXAxis()->set_gridStyle(ls);
        ui->plot->getXAxis()->set_gridWidth(lw);
        ui->plot->getXAxis()->set_gridColor(lc);
        ui->plot->getYAxis()->set_gridStyle(ls);
        ui->plot->getYAxis()->set_gridWidth(lw);
        ui->plot->getYAxis()->set_gridColor(lc);
    }
    ui->plot->setGrid(enabled);
    ui->plot->update_plot();
    ui->chkGrid->setChecked(enabled);
}

void QFSimplePlotWidget::setXAxisLabel(const QString &label)
{
    ui->plot->getXAxis()->set_axisLabel(label);
}

void QFSimplePlotWidget::setYAxisLabel(const QString &label)
{
    ui->plot->getYAxis()->set_axisLabel(label);
}




void QFSimplePlotWidget::addPlot(const QVector<double> &X, const QVector<double> &Y, const QString &spec, const QString& label)
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

void QFSimplePlotWidget::addFunctionPlot(QFFitFunction* function, const QString &spec, const QString &label, const QVector<double> &parameters)
{
    JKQtBasePlotter* plt=ui->plot->get_plotter();
    JKQTPdatastore* ds=plt->getDatastore();
    if (function) {
        QString ilabel=label;
        if (ilabel.isEmpty()) ilabel=QString("plot%1").arg(plt->getGraphCount()+1);
        JKQTPxQFFitFunctionLineGraph* g=new JKQTPxQFFitFunctionLineGraph(plt);
        if (!label.isEmpty()) g->set_title(label);
        g->set_fitFunction(function);
        g->set_params(parameters);

        PARSE_SPEC_NOSYMBOL(spec, g)


        plt->addGraph(g);
    }

}

void QFSimplePlotWidget::addBarPlot(const QVector<double> &X, const QVector<double> &Y, const QString &spec, const QString &label)
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

        double cnt=0;
        for (size_t i=0; i<plt->getGraphCount(); i++) {
            if (dynamic_cast<JKQTPbarHorizontalGraph*>(plt->getGraph(i))) {
                cnt++;
            }
        }
        double s=0.0;
        double w=0.9;
        if (cnt>1) {
            double w=1.0/cnt*0.7;
            double d=0.8/(cnt);
            double h=0.1+d/2.0;
            for (size_t i=0; i<plt->getGraphCount(); i++) {
                JKQTPbarHorizontalGraph* b=dynamic_cast<JKQTPbarHorizontalGraph*>(plt->getGraph(i));
                if (b) {
                    b->set_width(w);
                    b->set_shift(h-0.5);
                    h=h+d;
                }
            }
        } else {
            for (size_t i=0; i<plt->getGraphCount(); i++) {
                JKQTPbarHorizontalGraph* b=dynamic_cast<JKQTPbarHorizontalGraph*>(plt->getGraph(i));
                if (b) {
                    b->set_width(w);
                    b->set_shift(s);
                }
            }
        }
    }

}

void QFSimplePlotWidget::addErrorPlot(const QVector<double> &X, const QVector<double> &Y, const QVector<double> &Xerrors, const QVector<double> &Yerrors, const QString &spec, const QString &label, JKQTPerrorPlotstyle errorstyle)
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

void QFSimplePlotWidget::addScatterPlot(const QVector<double> &X, const QVector<double> &Y, const QVector<double> &S, const QString &spec, const QString &label)
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

void QFSimplePlotWidget::addColorScatterPlot(const QVector<double> &X, const QVector<double> &Y, const QVector<double> &S, const QString &spec, const QString &label, const QString &colorscale)
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

void QFSimplePlotWidget::fitRanges()
{
    ui->plot->zoomToFit();
}

void QFSimplePlotWidget::fitXRange()
{
    ui->plot->zoomToFit(true, false);
}

void QFSimplePlotWidget::fitYRange()
{
    ui->plot->zoomToFit(false, true);
}

void QFSimplePlotWidget::setAxesEditable(bool editable)
{
    ui->chkLogX->setEnabled(editable);
    ui->chkLogY->setEnabled(editable);
}


QFSimplePlotDialog::QFSimplePlotDialog(QWidget *parent, Qt::WindowFlags f):
    QFDialog(parent, f|Qt::WindowStaysOnTopHint)
{
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle(tr("QuickFit: simple plot"));
    setAttribute(Qt::WA_DeleteOnClose, true);
    QVBoxLayout* vbl=new QVBoxLayout();
    plot=new QFSimplePlotWidget(this);
    vbl->addWidget(plot);
    QHBoxLayout* hbl=new QHBoxLayout();
    vbl->addLayout(hbl);
    hbl->addStretch();
    QPushButton* btn=new QPushButton(QIcon(":/lib/exit.png"), tr("&Close"), this);
    connect(btn, SIGNAL(clicked()), this, SLOT(close()));
    hbl->addWidget(btn);
    setLayout(vbl);
    resize(640,480);
    ProgramOptions::getConfigWindowGeometry(this, "QFSimplePlotDialog/geometry");
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

QFSimplePlotDialog::~QFSimplePlotDialog()
{
    ProgramOptions::setConfigWindowGeometry(this, "QFSimplePlotDialog/geometry");
}
