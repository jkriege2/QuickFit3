#include "qfrdrimagingfcsdiffusionlawdialog.h"
#include "ui_qfrdrimagingfcsdiffusionlawdialog.h"
#include "qfrdrimagingfcs_data.h"
#include "qfmathtools.h"
#include "qftools.h"
#include "statistics_tools.h"
#include "qfpluginservices.h"


QFRDRImagingFCSDiffusionLawDialog::QFRDRImagingFCSDiffusionLawDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSDiffusionLawDialog)
{
    ui->setupUi(this);
    ui->btnToTable->setDefaultAction(ui->plotter->get_actCopyToTable());
    ui->plotter->set_toolbarAlwaysOn(true);
    ui->plotter->set_toolbarIconSize(24);
}

QFRDRImagingFCSDiffusionLawDialog::~QFRDRImagingFCSDiffusionLawDialog()
{
    delete ui;
}

void QFRDRImagingFCSDiffusionLawDialog::init(const QList<QFRDRImagingFCSData *> &recs, const QString &evalgroup)
{
    disconnect(ui->cmbParameter1, SIGNAL(currentIndexChanged(int)), this, SLOT(paramChanged()));
    disconnect(ui->cmbParameter2, SIGNAL(currentIndexChanged(int)), this, SLOT(paramChanged()));
    this->recs=recs;
    this->evalgroup=evalgroup;

    ui->lstRDRs->clear();
    rdrItems.clear();
    fitparams.clear();
    int p1=-1;
    int p2=-1;
    QList<QTriple<QString, QString, QString> > fp;
    for (int i=0; i<recs.size(); i++) {
        rdrItems.append(addCheckableQListWidgetItem(ui->lstRDRs, recs[i]->getName(), Qt::Checked));
        QList<QTriple<QString, QString, QString> > p=recs[i]->resultsCalcNamesEvalIDsAndLabels(QString(), QString(), evalgroup);
        if (fp.isEmpty()) fp=p;
        for (int j=0; j<p.size(); j++) {
            if (!fp.contains(p[j])) {
                fp.removeAll(p[j]);
            }
        }
    }


    bool hasAEff=false;
    for (int j=0; j<fp.size(); j++) {
        QString lab=fp[j].first;
        QString rn=fp[j].second;
        QString en=fp[j].third;
        if (!rn.startsWith("fix_") && !rn.endsWith("_islocal")) {

            fpProp prop;
            prop.label=lab;
            prop.resultname=rn;
            prop.evalname=en;

            if (rn.toLower().endsWith("efective_area")) { p1=j; hasAEff=true; prop.isAeff=true; }
            if (rn.toLower().endsWith("effective_area")) { p1=j; hasAEff=true; prop.isAeff=true; }
            if (rn.toLower().endsWith("focus_width") && p1<0 && !hasAEff) { p1=j; prop.isWxy=true; }
            if (rn.toLower().endsWith("pixel_size") && p1<0) { p1=j; prop.isPixSize=true; }
            if (rn.toLower().endsWith("pixel_width") && p1<0) { p1=j; prop.isPixSize=true; }
            if (rn.toLower().endsWith("focus_distance_x") && p1<0) { p1=j; prop.isPixSize=true; }
            if (rn.toLower().endsWith("focus_distance_y") && p1<0) { p1=j; prop.isPixSize=true; }
            if (rn.toLower().endsWith("focus_distance") && p1<0) { p1=j; prop.isPixSize=true; }

            if (rn.toLower().endsWith("msd_time_aeff1") && p2<0) { p2=j; prop.isTau=true; }
            if (rn.toLower().endsWith("msd_time_aeff2") && p2<0) { p2=j; prop.isTau=true; }
            if (rn.toLower().endsWith("msd_time_aeff3") && p2<0) { p2=j; prop.isTau=true; }
            if (rn.toLower().endsWith("diff_tau1")) { p2=j; prop.isTau=true; }
            if (rn.toLower().endsWith("diff_tau2") && p2<0) { p2=j; prop.isTau=true; }
            if (rn.toLower().endsWith("diff_coeff1")) { p2=j; prop.isD=true; }
            if (rn.toLower().endsWith("diff_coeff2") && p2<0) { p2=j; prop.isD=true; }

            fitparams.append(prop);
            ui->cmbParameter1->addItem(lab);
            ui->cmbParameter2->addItem(lab);
        }
    }

    ui->cmbParameter1->setCurrentIndex(p1);
    ui->cmbParameter2->setCurrentIndex(p2);
    connect(ui->cmbParameter1, SIGNAL(currentIndexChanged(int)), this, SLOT(paramChanged()));
    connect(ui->cmbParameter2, SIGNAL(currentIndexChanged(int)), this, SLOT(paramChanged()));
    paramChanged();
}

void QFRDRImagingFCSDiffusionLawDialog::setSelection(const bool *data, int width, int height)
{
    this->selImg=data;
    this->width=width;
    this->height=height;
}

void QFRDRImagingFCSDiffusionLawDialog::on_btnRecalc_clicked()
{
    recalcPlot();
}

void QFRDRImagingFCSDiffusionLawDialog::paramChanged()
{
    disconnect(ui->cmbMode, SIGNAL(currentIndexChanged(int)), this, SLOT(recalcPlot()));

    int p1=ui->cmbParameter1->currentIndex();
    int p2=ui->cmbParameter2->currentIndex();

    if (p1>=0 && p2>=0) {
        ui->cmbMode->setCurrentIndex(0);
        /*  simple (tau vs. Aeff)</string>
            D vs. Aeff
            D vs. Wx
            tauD vs. Wxy
         */
        if (fitparams[p1].isAeff && fitparams[p2].isTau) {
            ui->cmbMode->setCurrentIndex(0);
        } else if (fitparams[p1].isAeff && fitparams[p2].isD) {
            ui->cmbMode->setCurrentIndex(1);
        } else if ((fitparams[p1].isWxy || fitparams[p1].isPixSize)  && fitparams[p2].isD) {
            ui->cmbMode->setCurrentIndex(2);
        } else if ((fitparams[p1].isWxy ||fitparams[p1].isPixSize)  && fitparams[p2].isTau) {
            ui->cmbMode->setCurrentIndex(3);
        }
    }
    connect(ui->cmbMode, SIGNAL(currentIndexChanged(int)), this, SLOT(recalcPlot()));
    recalcPlot();
}

void QFRDRImagingFCSDiffusionLawDialog::recalcPlot()
{
    QVector<double> X, Xerr, Y, Yerr, W;
    int p1=ui->cmbParameter1->currentIndex();
    int p2=ui->cmbParameter2->currentIndex();
    QString unit1="";
    QString unit2="";

    for (int i=0; i<rdrItems.size(); i++) {
        if (rdrItems[i]->checkState()==Qt::Checked && recs[i]) {
            bool ok=false;
            QVector<double> vp1=recs[i]->resultsGetAsDoubleList(fitparams[p1].evalname, fitparams[p1].resultname, &ok);
            if (unit1.isEmpty()) unit1=recs[i]->resultsGet(fitparams[p1].evalname, fitparams[p1].resultname).unit;
            const bool* mask1=recs[i]->maskGet();
            QVector<double> vp2=recs[i]->resultsGetAsDoubleList(fitparams[p2].evalname, fitparams[p2].resultname, &ok);
            if (unit2.isEmpty()) unit2=recs[i]->resultsGet(fitparams[p2].evalname, fitparams[p2].resultname).unit;
            const bool* mask2=recs[i]->maskGet();
            int w=recs[i]->getImageFromRunsWidth();
            int h=recs[i]->getImageFromRunsHeight();
            QVector<bool> sel;
            if (selImg && ui->chkSelection->isChecked() && width*height>0) {
                if (w*h==width*height) {
                    for (int i=0; i<width*height; i++) {
                        sel<<selImg[i];
                    }
                } else if (w*h!=width*height) {
                    for (int i=0; i<w*h; i++) {
                        int x=i%w;
                        int y=i/w;
                        int x2=round(double(x)/double(w)*double(width));
                        int y2=round(double(y)/double(h)*double(height));
                        int i2=qBound(0, y2*width+x2, width*height);
                        sel<<selImg[i2];
                    }
                }
            }
            if (sel.size()<=0) {
                for (int i=0; i<w*h; i++) {
                    sel<<true;
                }
            }
            if (ok && vp1.size()==vp2.size() && vp1.size()==w*h && vp1.size()>0) {
                QVector<double> d1, d2;
                for (int i=0; i<w*h; i++) {
                    if (sel.value(i, false) && (!mask1 || !mask1[i]) && i<vp1.size()) {
                        d1<<vp1[i];
                    }
                    if (sel.value(i, false) && (!mask2 || !mask2[i]) && i<vp2.size()) {
                        d2<<vp2[i];
                    }
                }

                if (d1.size()>0 && d2.size()>0) {
                    /*  simple (tau vs. Aeff)</string>
                    Aeff/D vs. Aeff
                    Wxy^2/D vs. Wxy^2
                    tauD vs. Wxy^2
                 */
                    if (ui->cmbMode->currentIndex()==0) {

                    } else if (ui->cmbMode->currentIndex()==1) {
                        for (int i=0; i<d1.size(); i++) {
                            d1[i]=d1[i]/d2[i];
                        }
                    } else if (ui->cmbMode->currentIndex()==2) {
                        for (int i=0; i<d1.size(); i++) {
                            d1[i]=qfSqr(d1[i])/d2[i];
                        }
                        for (int i=0; i<d2.size(); i++) {
                            d1[i]=qfSqr(d1[i]);
                        }
                    } else if (ui->cmbMode->currentIndex()==3) {
                        for (int i=0; i<d2.size(); i++) {
                            d1[i]=qfSqr(d1[i]);
                        }
                    }
                    double stdv=0;
                    X<<qfstatisticsAverageStd(stdv, d1);
                    Xerr<<stdv;
                    stdv=0;
                    Y<<qfstatisticsAverageStd(stdv, d2);
                    Yerr<<stdv;
                    W<<(1.0/stdv);
                }

            }
            bool draw=ui->plotter->get_doDrawing();
            ui->plotter->set_doDrawing(false);
            ui->plotter->clearGraphs();
            ui->plotter->getDatastore()->clear();
            int cX=ui->plotter->getDatastore()->addCopiedColumn(X, QString("mean(%1)").arg(ui->cmbParameter1->currentText()));
            int cXerr=ui->plotter->getDatastore()->addCopiedColumn(Xerr, QString("std(%1)").arg(ui->cmbParameter1->currentText()));
            int cY=ui->plotter->getDatastore()->addCopiedColumn(Y, QString("mean(%1)").arg(ui->cmbParameter2->currentText()));
            int cYerr=ui->plotter->getDatastore()->addCopiedColumn(Yerr, QString("std(%1)").arg(ui->cmbParameter2->currentText()));

            ui->plotter->getXAxis()->set_logAxis(false);
            ui->plotter->getYAxis()->set_logAxis(false);

            if (ui->cmbMode->currentIndex()==0) {
                ui->plotter->getXAxis()->set_axisLabel(QString("\\verb{%1} [%2]").arg(ui->cmbParameter1->currentText()).arg(unit1));
                ui->plotter->getYAxis()->set_axisLabel(QString("\\verb{%1} [%2]").arg(ui->cmbParameter2->currentText()).arg(unit2));
            } else if (ui->cmbMode->currentIndex()==1) {
                ui->plotter->getXAxis()->set_axisLabel(QString("\\verb{%1} [%2]").arg(ui->cmbParameter1->currentText()).arg(unit1));
                ui->plotter->getYAxis()->set_axisLabel(QString("\\frac{\\verb{%1}}{\\verb{%2}} [(%3)/(%4)]").arg(ui->cmbParameter1->currentText()).arg(ui->cmbParameter2->currentText()).arg(unit1).arg(unit2));
            } else if (ui->cmbMode->currentIndex()==2) {
                ui->plotter->getXAxis()->set_axisLabel(QString("\\left(\\verb{%1}\\right)^2  [(%2)^2]").arg(ui->cmbParameter1->currentText()).arg(unit1));
                ui->plotter->getYAxis()->set_axisLabel(QString("\\frac{\\left(\\verb{%1}\\right)^2}{\\verb{%2}}  [(%3)^2/(%4)]").arg(ui->cmbParameter1->currentText()).arg(ui->cmbParameter2->currentText()).arg(unit1).arg(unit2));
            } else if (ui->cmbMode->currentIndex()==3) {
                ui->plotter->getXAxis()->set_axisLabel(QString("\\left(\\verb{%1}\\right)^2 [(%2)^2]").arg(ui->cmbParameter1->currentText()).arg(unit1));
                ui->plotter->getYAxis()->set_axisLabel(QString("\\verb{%1} [%2]").arg(ui->cmbParameter2->currentText()).arg(unit2));
            }

            ui->plotter->get_plotter()->set_keyPosition(JKQTPkeyInsideTopLeft);

            JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(ui->plotter->get_plotter());
            g->set_title(tr("data"));
            g->set_color(QColor("red"));
            g->set_errorColor(g->get_color().darker());
            g->set_drawLine(false);
            g->set_symbol(JKQTPplus);
            g->set_xErrorStyle(JKQTPerrorSimpleBars);
            g->set_yErrorStyle(JKQTPerrorSimpleBars);
            g->set_xColumn(cX);
            g->set_xErrorColumn(cXerr);
            g->set_yColumn(cY);
            g->set_yErrorColumn(cYerr);
            ui->plotter->addGraph(g);

            {
                double a=0, b=0;
                statisticsLinearRegression(X.data(), Y.data(), X.size(), a, b);
                JKQTPxFunctionLineGraph* gfit=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
                gfit->set_title(tr("fit: $f(x) = %1 + %2 \\cdot x$").arg(doubleToLatexQString(a, 4, false)).arg(doubleToLatexQString(b, 4, false)));
                gfit->set_params(constructQVectorFromItems(a, b));
                gfit->setSpecialFunction(JKQTPxFunctionLineGraph::Line);
                gfit->set_color(QColor("blue"));
                ui->plotter->addGraph(gfit);
            }
            {
                double a=0, b=0;
                statisticsLinearWeightedRegression(X.data(), Y.data(), W.data(), X.size(), a, b);
                JKQTPxFunctionLineGraph* gfit=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
                gfit->set_title(tr("weighted fit: $f(x) = %1 + %2 \\cdot x$").arg(doubleToLatexQString(a, 4, false)).arg(doubleToLatexQString(b, 4, false)));
                gfit->set_params(constructQVectorFromItems(a, b));
                gfit->setSpecialFunction(JKQTPxFunctionLineGraph::Line);
                gfit->set_color(QColor("darkgreen"));
                ui->plotter->addGraph(gfit);
            }


            ui->plotter->zoomToFit();
            ui->plotter->set_doDrawing(draw);
            ui->plotter->update_plot();
        }
    }
}

void QFRDRImagingFCSDiffusionLawDialog::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayPluginHelpWindow("imaging_fcs", "tutorial_difflaw.html");
}
