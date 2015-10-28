#include "qfetcspcimporterlifetimeview.h"
#include "ui_qfetcspcimporterlifetimeview.h"
#include "qfaction.h"
#include "qfpluginservices.h"
QFETCSPCImporterLifetimeView::QFETCSPCImporterLifetimeView(QWidget *parent) :
    QFDialog(parent),
    ui(new Ui::QFETCSPCImporterLifetimeView)
{
    plteRange=NULL;
    ui->setupUi(this);
    plteRange=new JKQTPverticalRange(ui->plotter->get_plotter());
    plteRange->set_rangeMin(0);
    plteRange->set_rangeMax(0);
    plteRange->set_visible(false);
    plteRange->set_unlimitedSizeMin(true);
    plteRange->set_unlimitedSizeMax(true);
    plteRange->set_fillRange(true);
    plteRange->set_plotCenterLine(false);
    plteRange->set_invertedRange(true);
    QColor c=QColor("grey");
    c.setAlphaF(0.5);
    plteRange->set_color(c);

    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
    connect(ui->plotter, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
}

QFETCSPCImporterLifetimeView::~QFETCSPCImporterLifetimeView()
{
    delete ui;
}

void QFETCSPCImporterLifetimeView::initArrialtimes(const QList<QVector<int16_t> > &arrivaltimes, double mtOffset, double mtFactor)
{
    for (int i=0; i<actsCopyRange.keys().size(); i++) {
        ui->plotter->removeAction(actsCopyRange.keys().at(i));
        delete actsCopyRange.keys().at(i);
    }
    actsCopyRange.clear();
    this->arrivaltimes=arrivaltimes;
    this->mtOffset=mtOffset;
    this->mtFactor=mtFactor;
    int maxC=0;
    for (int c=0; c<arrivaltimes.size(); c++) {
        for (int i=0; i<arrivaltimes[c].size(); i++) {
            if (arrivaltimes[c].at(i)>maxC) maxC=arrivaltimes[c].at(i);
        }
    }

    ui->plotter->set_doDrawing(false);
    ui->plotter->deleteGraph(plteRange, false);
    ui->plotter->clearGraphs(true);
    ui->plotter->addGraph(plteRange);
    JKQTPdatastore* ds=ui->plotter->getDatastore();
    graphs.clear();
    ds->clear();


    QVector<double> channels;
    QVector<double> microtime;
    for (int i=0; i<maxC; i++) {
        channels<<i;
        microtime<<(mtOffset+double(i)*mtFactor);
    }
    ds->addCopiedColumn(channels.data(), channels.size(), tr("microtime channel"));
    ds->addCopiedColumn(microtime.data(), microtime.size(), tr("microtime [ns]"));
    for (int c=0; c<arrivaltimes.size(); c++) {
        if (arrivaltimes[c].size()>0) {
            QAction* act=new QFActionWithNoMenuRole(tr("use lifetime range for channel %1").arg(c+1), this);
            connect(act, SIGNAL(triggered()), this, SLOT(sendRange()));
            actsCopyRange.insert(act, c);
            ui->plotter->addAction(act);
            QVector<double> hist;
            for (int i=0; i<maxC; i++) {
                hist<<0;
            }
            for (int i=0; i<arrivaltimes[c].size(); i++) {
                int16_t mtch=arrivaltimes[c].at(i);
                if (mtch>=0 && mtch<maxC) {
                    hist[mtch]++;
                }
            }
            for (int i=0; i<hist.size(); i++) {
                hist[i]=hist[i]/double(arrivaltimes[c].size());
            }
            size_t pc=ds->addCopiedColumn(hist.data(), hist.size(), tr("channel %1 histogram").arg(c+1));
            JKQTPxyLineGraph* g=new JKQTPxyLineGraph(ui->plotter->get_plotter());
            g->set_title(tr("channel %1").arg(c+1));
            g->set_drawLine(true);
            g->set_symbol(JKQTPnoSymbol);
            g->set_lineWidth(0.75);
            g->set_xColumn(1);
            g->set_yColumn(pc);
            ui->plotter->addGraph(g);
            graphs<<g;
        }
    }

    ui->plotter->getXAxis()->set_axisLabel(tr("TCSPC mircotime [ns]"));
    ui->plotter->getYAxis()->set_axisLabel(tr("norm. photon frequency"));
    ui->plotter->set_doDrawing(true);
    updatePlot();
}

void QFETCSPCImporterLifetimeView::setDescription(const QString &text)
{
    ui->labDescription->setText(text);
}

void QFETCSPCImporterLifetimeView::updatePlot()
{
    ui->plotter->set_doDrawing(false);
    ui->plotter->getXAxis()->set_logAxis(ui->chkLogX->isChecked());
    ui->plotter->getYAxis()->set_logAxis(ui->chkLogY->isChecked());
    ui->plotter->set_doDrawing(true);
    ui->plotter->zoomToFit();
    ui->plotter->update_plot();
}

void QFETCSPCImporterLifetimeView::showHelp()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("tcspcimporter")+"livetimeview.html");
}

void QFETCSPCImporterLifetimeView::on_btnSelectRange_toggled(bool enabled)
{
    if (enabled) {
        ui->plotter->set_mouseActionMode(JKQtPlotter::RectangleEvents);
        if (plteRange) plteRange->set_visible(true);
        for (int i=0; i<actsCopyRange.keys().size(); i++) {
            actsCopyRange.keys().at(i)->setVisible(true);
        }
    } else {
        ui->plotter->set_mouseActionMode(JKQtPlotter::ZoomRectangle);
        if (plteRange) plteRange->set_visible(false);
        for (int i=0; i<actsCopyRange.keys().size(); i++) {
            actsCopyRange.keys().at(i)->setVisible(false);
        }
    }
    ui->plotter->update_plot();
}

void QFETCSPCImporterLifetimeView::userRectangleFinished(double x, double /*y*/, double width, double /*height*/, Qt::KeyboardModifiers /*modifiers*/)
{
    if (!plteRange) return;
    plteRange->set_rangeMin(qMin(x,x+width));
    plteRange->set_rangeMax(qMax(x,x+width));
    ui->plotter->update_plot();
}

void QFETCSPCImporterLifetimeView::sendRange()
{
    if (!plteRange) return;
    QAction* act=qobject_cast<QAction*>(sender());
    if (act && actsCopyRange.contains(act)) {
        emit setLifetimeFilter(actsCopyRange[act], plteRange->get_rangeMin(), plteRange->get_rangeMax());
    }
}
