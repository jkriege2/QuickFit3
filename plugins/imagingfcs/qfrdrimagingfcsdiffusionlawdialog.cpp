#include "qfrdrimagingfcsdiffusionlawdialog.h"
#include "ui_qfrdrimagingfcsdiffusionlawdialog.h"
#include "qfrdrimagingfcs_data.h"
#include "qfmathtools.h"
#include "qftools.h"
#include "statistics_tools.h"
#include "qfpluginservices.h"


QFRDRImagingFCSDiffusionLawDialog::QFRDRImagingFCSDiffusionLawDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFRDRImagingFCSDiffusionLawDialog)
{    
    dontWriteConfig=false;
    ui->setupUi(this);
    ui->btnToTable->setDefaultAction(ui->plotter->get_actCopyToTable());
    ui->plotter->set_toolbarAlwaysOn(true);
    ui->plotter->set_toolbarIconSize(24);
    currentRecord=NULL;
    ui->plotter->get_plotter()->set_keyFontSize(11);
    ui->edtScale1->setValue(1);
    ui->edtScale2->setValue(1);
    ui->edtScale3->setValue(1);
    setAttribute(Qt::WA_DeleteOnClose);
}

QFRDRImagingFCSDiffusionLawDialog::~QFRDRImagingFCSDiffusionLawDialog()
{
    delete ui;
}


void QFRDRImagingFCSDiffusionLawDialog::setSelection(const bool *data, int width, int height)
{
    this->selImg.clear();;
    for (int i=0; i<width*height; i++) {
        this->selImg<<data[i];
    }
    this->selImgWidth=width;
    this->selImgHeight=height;
    recalcPlot();
}

void QFRDRImagingFCSDiffusionLawDialog::setSelection(const QVector<bool> &data, int width, int height)
{
    this->selImg=data;
    this->selImgWidth=width;
    this->selImgHeight=height;
    recalcPlot();
}

void QFRDRImagingFCSDiffusionLawDialog::init(QFRDRImagingFCSData *current, const QString &evalgroup)
{
    currentRecord=current;
    QList<QFRawDataRecord*> recs=current->getProject()->getRDRGroupMembers(current->getGroup());
    QList<QFRDRImagingFCSData*> recsFCS;
    bool hasFCCS=false;
    bool hasDCCF=false;
    int ACFs=0, FCCSs=0, DCCFs=0;
    for (int i=0; i<recs.size(); i++) {
        QFRDRImagingFCSData* m1=qobject_cast<QFRDRImagingFCSData*>(recs[i]);
        if(!m1) {
            //recs.removeAt(i);
        } else {
            if (m1->isFCCS()) { hasFCCS=true; FCCSs++; }
            if (m1->isDCCF()) { hasDCCF=true; DCCFs++; }
            if (m1->isACF()) ACFs++;
            recsFCS<<m1;
        }
    }

    QList<QFRawDataRecord*> recsFolder=current->getProject()->getRDRsInFolder(current->getFolder(), true);
    QList<QFRDRImagingFCSData*> recsFCSFolder;
    bool hasFCCSFolder=false;
    bool hasDCCFFolder=false;
    int ACFsFolder=0, FCCSsFolder=0, DCCFsFolder=0;
    for (int i=0; i<recsFolder.size(); i++) {
        QFRDRImagingFCSData* m1=qobject_cast<QFRDRImagingFCSData*>(recsFolder[i]);
        if(!m1) {
            //recs.removeAt(i);
        } else {
            if (m1->isFCCS()) { hasFCCSFolder=true; FCCSsFolder++; }
            if (m1->isDCCF()) { hasDCCFFolder=true; DCCFsFolder++; }
            if (m1->isACF()) ACFsFolder++;
            recsFCSFolder<<m1;
        }
    }

    QList<QFRDRImagingFCSData*> r;
    QList<bool> proposedSel;
    bool hasSel=false;
    for (int i=0; i<recsFCSFolder.size(); i++) {
        if (!r.contains(recsFCSFolder[i])){
            r<<recsFCSFolder[i];
            if (ACFsFolder>2 && recsFCSFolder[i]->isACF()) {proposedSel<<true; hasSel=true; }
            else if (DCCFsFolder>2 && recsFCSFolder[i]->isDCCF()) {proposedSel<<true; hasSel=true; }
            else proposedSel<<false;
        }
    }
    for (int i=0; i<recsFCS.size(); i++) {
        if (!r.contains(recsFCS[i])) {
            r<<recsFCS[i];
            if (!hasSel) {
                if (ACFs>2 && recsFCS[i]->isACF()) {proposedSel<<true; hasSel=true; }
                else if (DCCFs>2 && recsFCS[i]->isDCCF()) {proposedSel<<true; hasSel=true; }
                else proposedSel<<false;
            } else proposedSel<<false;
        }
    }

    if (current) {
        if (current->propertyExists(evalgroup+"/FCSDiffLawSelection")) {
            QString sel=current->getQFProperty(evalgroup+"/FCSDiffLawSelection", "").toString();
            if (sel.size()>0) {
                proposedSel.clear();
                for (int i=0; i<sel.size(); i++) {
                    proposedSel<<(sel[i]=='1');
                }
            }
        }
    }
    dontWriteConfig=true;
    init(r, evalgroup, proposedSel);

    loadConfig();
    on_cmbMode_currentIndexChanged(ui->cmbMode->currentIndex());
    dontWriteConfig=false;
    loadConfig();
}

void QFRDRImagingFCSDiffusionLawDialog::saveConfig()
{
    if (dontWriteConfig) return;
    if (currentRecord) {
        QString selString;

        for (int i=0; i<rdrItems.size(); i++) {
            selString+=QString((rdrItems[i]->checkState()==Qt::Checked)?"1":"0");
        }

        currentRecord->setQFProperty(evalgroup+"/FCSDiffLawSelection", selString, false, false);
        currentRecord->setQFProperty(evalgroup+"/param1", ui->cmbParameter1->currentData(), false, false);
        currentRecord->setQFProperty(evalgroup+"/param2", ui->cmbParameter2->currentData(), false, false);
        currentRecord->setQFProperty(evalgroup+"/param3", ui->cmbParameter3->currentData(), false, false);
        currentRecord->setQFProperty(evalgroup+"/mode", ui->cmbMode->currentIndex(), false, false);
        currentRecord->setQFProperty(evalgroup+"/useSelection", ui->chkSelection->isChecked(), false, false);

        currentRecord->setQFProperty(evalgroup+"/chkLogX", ui->chkLogX->isChecked(), false, false);
        currentRecord->setQFProperty(evalgroup+"/chkLogY", ui->chkLogY->isChecked(), false, false);
        currentRecord->setQFProperty(evalgroup+"/chkFitLin", ui->chkFitLin->isChecked(), false, false);
        currentRecord->setQFProperty(evalgroup+"/chkFitQuadratic", ui->chkFitQuadratic->isChecked(), false, false);
        currentRecord->setQFProperty(evalgroup+"/chkFitPowerlaw", ui->chkFitPowerlaw->isChecked(), false, false);
        currentRecord->setQFProperty(evalgroup+"/chkFitWeighted", ui->chkFitWeighted->isChecked(), false, false);
        currentRecord->setQFProperty(evalgroup+"/chkShowErrors", ui->chkShowErrors->isChecked(), false, false);

        currentRecord->setQFProperty(evalgroup+"/chkDefaultScale1", ui->chkDefaultScale1->isChecked(), false, false);
        currentRecord->setQFProperty(evalgroup+"/chkDefaultScale2", ui->chkDefaultScale2->isChecked(), false, false);
        currentRecord->setQFProperty(evalgroup+"/edtScale1", ui->edtScale1->value(), false, false);
        currentRecord->setQFProperty(evalgroup+"/edtScale2", ui->edtScale2->value(), false, false);
        currentRecord->setQFProperty(evalgroup+"/chkDefaultScale3", ui->chkDefaultScale3->isChecked(), false, false);
        currentRecord->setQFProperty(evalgroup+"/edtScale3", ui->edtScale3->value(), false, false);
    }
}

void QFRDRImagingFCSDiffusionLawDialog::loadConfig()
{
    if (currentRecord) {
        QString selString=currentRecord->getQFProperty(evalgroup+"/FCSDiffLawSelection", "").toString();
        if (selString.size()>0) {
            for (int i=0; i<ui->lstRDRs->count(); i++) {
                if (ui->lstRDRs->item(i)) {
                    if (i<selString.size()) {
                        if (selString[i]=='1') ui->lstRDRs->item(i)->setCheckState(Qt::Checked);
                        else ui->lstRDRs->item(i)->setCheckState(Qt::Unchecked);
                    } else {
                        ui->lstRDRs->item(i)->setCheckState(Qt::Unchecked);
                    }
                }

            }
        }
        loadParams();

        ui->chkLogX->setChecked(currentRecord->getQFProperty(evalgroup+"/chkLogX", ui->chkLogX->isChecked()).toBool());
        ui->chkLogY->setChecked(currentRecord->getQFProperty(evalgroup+"/chkLogY", ui->chkLogY->isChecked()).toBool());
        ui->chkFitLin->setChecked(currentRecord->getQFProperty(evalgroup+"/chkFitLin", ui->chkFitLin->isChecked()).toBool());
        ui->chkFitQuadratic->setChecked(currentRecord->getQFProperty(evalgroup+"/chkFitQuadratic", ui->chkFitQuadratic->isChecked()).toBool());
        ui->chkFitPowerlaw->setChecked(currentRecord->getQFProperty(evalgroup+"/chkFitPowerlaw", ui->chkFitPowerlaw->isChecked()).toBool());
        ui->chkFitWeighted->setChecked(currentRecord->getQFProperty(evalgroup+"/chkFitWeighted", ui->chkFitWeighted->isChecked()).toBool());
        ui->chkDefaultScale1->setChecked(currentRecord->getQFProperty(evalgroup+"/chkDefaultScale1", ui->chkDefaultScale1->isChecked()).toBool());
        ui->chkDefaultScale2->setChecked(currentRecord->getQFProperty(evalgroup+"/chkDefaultScale2", ui->chkDefaultScale2->isChecked()).toBool());
        ui->chkDefaultScale3->setChecked(currentRecord->getQFProperty(evalgroup+"/chkDefaultScale3", ui->chkDefaultScale3->isChecked()).toBool());
        ui->chkShowErrors->setChecked(currentRecord->getQFProperty(evalgroup+"/chkShowErrors", ui->chkShowErrors->isChecked()).toBool());
        ui->edtScale1->setValue(currentRecord->getQFProperty(evalgroup+"/edtScale1", ui->edtScale1->value()).toDouble());
        ui->edtScale2->setValue(currentRecord->getQFProperty(evalgroup+"/edtScale2", ui->edtScale2->value()).toDouble());
        ui->edtScale3->setValue(currentRecord->getQFProperty(evalgroup+"/edtScale3", ui->edtScale3->value()).toDouble());
        ui->cmbMode->setCurrentIndex(currentRecord->getQFProperty(evalgroup+"/mode", ui->cmbMode->currentIndex()).toInt());
        ui->chkSelection->setChecked(currentRecord->getQFProperty(evalgroup+"/useSelection", ui->chkSelection->isChecked()).toBool());
    }
}

void QFRDRImagingFCSDiffusionLawDialog::loadParams()
{
    if (currentRecord) {
        ui->cmbParameter1->setCurrentData(currentRecord->getQFProperty(evalgroup+"/param1", ui->cmbParameter1->currentData()));
        ui->cmbParameter2->setCurrentData(currentRecord->getQFProperty(evalgroup+"/param2", ui->cmbParameter2->currentData()));
        ui->cmbParameter3->setCurrentData(currentRecord->getQFProperty(evalgroup+"/param3", ui->cmbParameter3->currentData()));
    }

}

void QFRDRImagingFCSDiffusionLawDialog::init(const QList<QFRDRImagingFCSData *> &recs, const QString &evalgroup, const QList<bool> &proposedRecordSelection)
{
    disconnect(ui->cmbParameter1, SIGNAL(currentIndexChanged(int)), this, SLOT(paramChanged()));
    disconnect(ui->cmbParameter2, SIGNAL(currentIndexChanged(int)), this, SLOT(paramChanged()));
    disconnect(ui->cmbParameter3, SIGNAL(currentIndexChanged(int)), this, SLOT(paramChanged()));
    this->recs=recs;
    this->evalgroup=evalgroup;

    ui->lstRDRs->clear();
    rdrItems.clear();
    fitparams.clear();
    int p1=-1;
    int p2=-1;
    int p3=-1;
    QList<QTriple<QString, QString, QString> > fp;
    for (int i=0; i<recs.size(); i++) {
        rdrItems.append(addCheckableQListWidgetItem(ui->lstRDRs, recs[i]->getName(), proposedRecordSelection.value(i, true)?(Qt::Checked):(Qt::Unchecked)));
        QList<QTriple<QString, QString, QString> > p=recs[i]->resultsCalcNamesEvalIDsAndLabels(QString(), QString(), evalgroup);
        if (fp.isEmpty()) fp=p;
        for (int j=0; j<p.size(); j++) {
            if (!fp.contains(p[j])) {
                fp.removeAll(p[j]);
            }
        }
    }


    ui->cmbParameter1->clear();
    ui->cmbParameter2->clear();
    ui->cmbParameter3->clear();
    bool hasAEff=false;
    int cj=0;
    int cAEff=-1;
    int cWxy=-1;
    int cPixSize=-1;
    int cD=-1;
    int cTauD2=-1;
    int cTauD=-1;
    int cPixDistance=-1;
    for (int j=0; j<fp.size(); j++) {
        QString lab=fp[j].first;
        QString rn=fp[j].second;
        QString en=fp[j].third;
        if (!rn.startsWith("fix_") && !rn.endsWith("_islocal") && !rn.endsWith("_fix")) {

            fpProp prop;
            prop.label=lab;
            prop.resultname=rn;
            prop.evalname=en;

            if (rn.toLower().endsWith("efective_area")) { cAEff=cj; hasAEff=true; prop.isAeff=true; }
            if (rn.toLower().endsWith("effective_area")) { cAEff=cj; hasAEff=true; prop.isAeff=true; }
            if (rn.toLower().endsWith("focus_width")) { cWxy=cj; prop.isWxy=true; }
            if (rn.toLower().endsWith("pixel_size")) { cPixSize=cj; prop.isPixSize=true; }
            if (rn.toLower().endsWith("pixel_width") && cPixSize<0) { cPixSize=cj; prop.isPixSize=true; }
            if (rn.toLower().endsWith("pixel_height") && cPixSize<0) { cPixSize=cj; prop.isPixSize=true; }
            if (rn.toLower().endsWith("focus_distance_x") && cPixDistance<0) { cPixDistance=cj; prop.isPixDistance=true; }
            if (rn.toLower().endsWith("focus_distance_y") && cPixDistance<0) { cPixDistance=cj; prop.isPixDistance=true; }
            if (rn.toLower().endsWith("focus_distance") && cPixDistance<0) { cPixDistance=cj; prop.isPixDistance=true; }

            if (rn.toLower().endsWith("msd_time_aeff1") ) { cTauD2=cj; prop.isTau=true; }
            if (rn.toLower().endsWith("msd_time_aeff2") && cTauD2<0) { cTauD2=cj; prop.isTau=true; }
            if (rn.toLower().endsWith("msd_time_aeff3") && cTauD2<0) { cTauD2=cj; prop.isTau=true; }
            if (rn.toLower().endsWith("diff_tau1")) { cTauD=cj; prop.isTau=true; }
            if (rn.toLower().endsWith("diff_tau2") && cTauD<0) { cTauD=cj; prop.isTau=true; }
            if (rn.toLower().endsWith("diff_coeff1")) { cD=cj; prop.isD=true; }
            if (rn.toLower().endsWith("diff_coeff2") && cD<0) { cD=cj; prop.isD=true; }

            fitparams.append(prop);
            ui->cmbParameter1->addItem(lab, en+"/"+rn);
            ui->cmbParameter2->addItem(lab, en+"/"+rn);
            ui->cmbParameter3->addItem(lab, en+"/"+rn);
            cj++;
        }
    }

    if (cPixDistance>=0) {
        p1=cPixDistance;
        p2=cTauD;
        if (cTauD<0) {
            if (cTauD2>=0) p2=cTauD2;
            else {
                p2=cD;
                p3=cAEff;
                if (cAEff<0) p3=cWxy;
            }
        }
    } else {
        p1=cAEff;
        if (cAEff<0) {
            p1=cWxy;
        }
        p2=cTauD;
        if (cTauD<0) {
            if (cTauD2>=0) p2=cTauD2;
            else {
                p2=cD;
                p3=cAEff;
                if (cAEff<0) p3=cWxy;
            }
        }
    }

    ui->cmbParameter1->setCurrentIndex(p1);
    ui->cmbParameter2->setCurrentIndex(p2);
    ui->cmbParameter3->setCurrentIndex(p3);
    on_cmbMode_currentIndexChanged(ui->cmbMode->currentIndex());
    connect(ui->cmbParameter1, SIGNAL(currentIndexChanged(int)), this, SLOT(paramChanged()));
    connect(ui->cmbParameter2, SIGNAL(currentIndexChanged(int)), this, SLOT(paramChanged()));
    connect(ui->cmbParameter3, SIGNAL(currentIndexChanged(int)), this, SLOT(paramChanged()));
    paramChanged();
}

void QFRDRImagingFCSDiffusionLawDialog::on_btnRecalc_clicked()
{
    recalcPlot();
}

void QFRDRImagingFCSDiffusionLawDialog::on_btnClose_clicked()
{
    saveConfig();
    close();
}

void QFRDRImagingFCSDiffusionLawDialog::paramChanged()
{
    disconnect(ui->cmbMode, SIGNAL(currentIndexChanged(int)), this, SLOT(recalcPlot()));

    int p1=ui->cmbParameter1->currentIndex();
    int p2=ui->cmbParameter2->currentIndex();
    int p3=ui->cmbParameter3->currentIndex();

    on_cmbMode_currentIndexChanged(ui->cmbMode->currentIndex());

    if (p1>=0 && p2>=0) {
        ui->cmbMode->setCurrentIndex(0);
        /*
         */
        if (fitparams[p1].isAeff && fitparams[p2].isTau) {
            ui->cmbMode->setCurrentIndex(0);
        } else if (fitparams[p1].isAeff && fitparams[p2].isD) {
            ui->cmbMode->setCurrentIndex(1);
        } else if ((fitparams[p1].isWxy || fitparams[p1].isPixSize)  && fitparams[p2].isD) {
            ui->cmbMode->setCurrentIndex(2);
        } else if ((fitparams[p1].isWxy ||fitparams[p1].isPixSize)  && fitparams[p2].isTau) {
            ui->cmbMode->setCurrentIndex(3);
        } else if ((fitparams[p1].isPixDistance)  && fitparams[p2].isTau) {
            ui->cmbMode->setCurrentIndex(7);
        } else if ((fitparams[p1].isPixDistance)  && fitparams[p2].isD) {
            ui->cmbMode->setCurrentIndex(6);
        }
    }
    connect(ui->cmbMode, SIGNAL(currentIndexChanged(int)), this, SLOT(recalcPlot()));
    recalcPlot();
}

void QFRDRImagingFCSDiffusionLawDialog::recalcPlot()
{
    QVector<double> X, Xerr, Y, Yerr, W, Nitems;
    int p1=ui->cmbParameter1->currentIndex();
    int p2=ui->cmbParameter2->currentIndex();
    int p3=ui->cmbParameter3->currentIndex();
    QString unit1="";
    QString unit2="";
    QString unit3="";

    QString selString;
    bool majorLabel=false;
    QVector<double> d1, d2, d3;

    for (int i=0; i<rdrItems.size(); i++) {
        selString+=QString((rdrItems[i]->checkState()==Qt::Checked)?"1":"0");
        if (rdrItems[i]->checkState()==Qt::Checked && recs[i]) {
            bool ok=false;
            QVector<double> vp1=recs[i]->resultsGetAsDoubleList(fitparams[p1].evalname, fitparams[p1].resultname, &ok);
            if (unit1.isEmpty()) unit1=recs[i]->resultsGet(fitparams[p1].evalname, fitparams[p1].resultname).unit;
            const bool* mask1=recs[i]->maskGet();
            QVector<double> vp2=recs[i]->resultsGetAsDoubleList(fitparams[p2].evalname, fitparams[p2].resultname, &ok);
            if (unit2.isEmpty()) unit2=recs[i]->resultsGet(fitparams[p2].evalname, fitparams[p2].resultname).unit;
            QVector<double> vp3=recs[i]->resultsGetAsDoubleList(fitparams[p3].evalname, fitparams[p3].resultname, &ok);
            if (unit3.isEmpty()) unit3=recs[i]->resultsGet(fitparams[p3].evalname, fitparams[p3].resultname).unit;
            const bool* mask2=recs[i]->maskGet();
            int w=recs[i]->getImageFromRunsWidth();
            int h=recs[i]->getImageFromRunsHeight();
            //qDebug()<<vp1.size()<<unit1<<vp2.size()<<unit2<<vp3.size()<<unit3<<w<<h;
            QVector<bool> sel;
            if (selImg.size()==selImgWidth*selImgHeight && ui->chkSelection->isChecked() && selImgWidth*selImgHeight>0) {
                if (w*h==selImgWidth*selImgHeight) {
                    for (int i=0; i<selImgWidth*selImgHeight; i++) {
                        sel<<selImg[i];
                    }
                } else  {
                    sel=resizeBoolVecImage(selImg, selImgWidth, selImgHeight, w, h, NULL);
                    if (sel.size()<w*h) {
                        for (int i=0; i<w*h; i++) {
                            sel<<false;
                        }
                    }
                }
            }
            if (sel.size()<=0) {
                for (int i=0; i<w*h; i++) {
                    sel<<true;
                }
            }

            if (ui->chkDefaultScale1->isChecked()) {
                double scale=1;
                QString unit=unit1;
                if (unit.toLower().contains("nano")||unit.toLower()=="nm") {
                    scale=1e-3;
                }
                if (unit.toLower()=="usec") {
                    scale=1e-6;
                }
                if (scale!=1) unit1=QString("%1\\cdot{%2}").arg(doubleToLatexQString(scale,3)).arg(unit1);
                ui->edtScale1->setValue(scale);
            }
            if (ui->chkDefaultScale2->isChecked()) {
                double scale=1;
                QString unit=unit2;
                if (unit.toLower().contains("nano")||unit.toLower()=="nm") {
                    scale=1e-3;
                }
                if (unit.toLower()=="usec") {
                    scale=1e-6;
                }
                if (scale!=1) unit2=QString("%1\\cdot{%2}").arg(doubleToLatexQString(scale,3)).arg(unit2);
                ui->edtScale2->setValue(scale);
            }
            if (ui->chkDefaultScale3->isChecked()) {
                double scale=1;
                QString unit=unit3;
                if (unit.toLower().contains("nano")||unit.toLower()=="nm") {
                    scale=1e-3;
                }
                if (unit.toLower()=="usec") {
                    scale=1e-6;
                }
                if (scale!=1) unit3=QString("%1\\cdot{%2}").arg(doubleToLatexQString(scale,3)).arg(unit3);
                ui->edtScale3->setValue(scale);
            }

            if (ok && vp1.size()==vp2.size() && vp1.size()==w*h && vp1.size()>0) {
                for (int i=0; i<w*h; i++) {
                    if (sel.value(i, false) && (!mask1 || !mask1[i]) && i<vp1.size()) {
                        d1<<(vp1[i]*ui->edtScale1->value());
                    }
                    if (sel.value(i, false) && (!mask2 || !mask2[i]) && i<vp2.size()) {
                        d2<<(vp2[i]*ui->edtScale2->value());
                    }
                    if (sel.value(i, false) && ((!mask1 || !mask1[i])||(!mask2 || !mask2[i])) && i<vp3.size()) {
                        d3<<(vp3[i]*ui->edtScale3->value());
                    }
                }
                //qDebug()<<"###"<<d1.size()<<d2.size()<<d3.size()<<d1.value(0, 0)<<d2.value(0, 0)<<d3.value(0, 0);

                if (d1.size()>0 && d2.size()>0) {
                    /*
                      Y   vs.  X
                0   simple P2 vs. P1 (e.g. tau vs. Aeff)
                1   (P1/P2) vs. P1 (e.g. Aeff/D vs. Aeff)
                2   (P1²/P2) vs. P1² (e.g. Wxy²/D vs. Wxy²)
                3   P2 vs. P1² (e.g. tauD vs. Wxy²)

                4   P3/P2 vs. P1² (e.g. Aeff/D vs. dx²)
                5   P3²/P2 vs. P1² (e.g. Wxy²/D vs. dx²)
                6   P1² vs. P1²/P2(e.g. dx² vs. Wxy²/D)
                7   P1² vs. P2 (e.g. dx² vs. tauD)

                 */
                    QVector<double> px, py;
                    if (ui->cmbMode->currentIndex()==0) {
                        px=d1;
                        py=d2;
                    } else if (ui->cmbMode->currentIndex()==1) {
                        px=d1;
                        for (int i=0; i<d1.size(); i++) {
                            py<<d1[i]/d2[i];
                        }
                    } else if (ui->cmbMode->currentIndex()==2) {
                        for (int i=0; i<d1.size(); i++) {
                            px<<qfSqr(d1[i]);
                            py<<qfSqr(d1[i])/d2[i];
                        }
                    } else if (ui->cmbMode->currentIndex()==3) {
                        for (int i=0; i<d1.size(); i++) {
                            px<<qfSqr(d1[i]);
                        }
                        py=d2;
                    } else if (ui->cmbMode->currentIndex()==4) {
                        for (int i=0; i<d1.size(); i++) {
                            px<<qfSqr(d1[i]);
                            py<<d3.value(i, 0)/d2[i];
                        }
                    } else if (ui->cmbMode->currentIndex()==5) {
                        for (int i=0; i<d1.size(); i++) {
                            px<<qfSqr(d1[i]);
                            py<<qfSqr(d3.value(i, 1))/d2[i];
                        }

                    } else if (ui->cmbMode->currentIndex()==6) {
                        for (int i=0; i<d1.size(); i++) {
                            px<<qfSqr(d1[i])/d2[i];
                            py<<qfSqr(d1[i]);
                        }
                    } else if (ui->cmbMode->currentIndex()==7) {
                        for (int i=0; i<d1.size(); i++) {
                            px<<d2[i];
                            py<<qfSqr(d1[i]);
                        }
                    }
                    double stdvx=0, stdvy=0, vx=0, vy=0;
                    vx=qfstatisticsAverageStd(stdvx, px);
                    vy=qfstatisticsAverageStd(stdvy, py);

                    if ( (vx!=0 || vy!=0) ){
                        X<<vx;
                        Y<<vy;
                        Xerr<<stdvx;
                        Yerr<<stdvy;
                        Nitems<<d1.size();
                        if (stdvy!=0) W<<(1.0/stdvy);
                        else W<<(1.0/double(rdrItems.size()));
                    }
                }

            }
            bool draw=ui->plotter->get_doDrawing();
            ui->plotter->set_doDrawing(false);
            ui->plotter->clearGraphs();
            ui->plotter->getDatastore()->clear();
            int cX=ui->plotter->getDatastore()->addCopiedColumn(X, QString("mean(X)"));
            int cXerr=ui->plotter->getDatastore()->addCopiedColumn(Xerr, QString("std(X)"));
            int cY=ui->plotter->getDatastore()->addCopiedColumn(Y, QString("mean(Y)"));
            int cYerr=ui->plotter->getDatastore()->addCopiedColumn(Yerr, QString("std(Y)"));
            int cNItems=ui->plotter->getDatastore()->addCopiedColumn(Nitems, tr("# pixels"));
            cNItems=ui->plotter->getDatastore()->addCopiedColumn(d1, ui->cmbParameter1->currentText());
            cNItems=ui->plotter->getDatastore()->addCopiedColumn(d2, ui->cmbParameter2->currentText());
            cNItems=ui->plotter->getDatastore()->addCopiedColumn(d3, ui->cmbParameter3->currentText());

            ui->plotter->getXAxis()->set_logAxis(ui->chkLogX->isChecked());
            ui->plotter->getYAxis()->set_logAxis(ui->chkLogY->isChecked());

            /*
                0   simple P2 vs. P1 (e.g. tau vs. Aeff)
                1   (P1/P2) vs. P1 (e.g. Aeff/D vs. Aeff)
                2   (P1²/P2) vs. P1² (e.g. Wxy²/D vs. Wxy²)
                3   P2 vs. P1² (e.g. tauD vs. Wxy²)
                4   P3/P2 vs. P1² (e.g. Aeff/D vs. dx²)
                5   P3²/P2 vs. P1² (e.g. Wxy²/D vs. dx²)
                6   P1² vs. P1²/P2(e.g. dx² vs. Wxy²/D)
                7   P1² vs. P2 (e.g. dx² vs. tauD)

             */
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
            } else if (ui->cmbMode->currentIndex()==4) {
                ui->plotter->getXAxis()->set_axisLabel(QString("\\left(\\verb{%1}\\right)^2  [(%2)^2]").arg(ui->cmbParameter1->currentText()).arg(unit1));
                ui->plotter->getYAxis()->set_axisLabel(QString("\\frac{\\verb{%3}}{\\verb{%2}}  [(%6)/(%5)]").arg(ui->cmbParameter1->currentText()).arg(ui->cmbParameter2->currentText()).arg(ui->cmbParameter3->currentText()).arg(unit1).arg(unit2).arg(unit3));
            } else if (ui->cmbMode->currentIndex()==5) {
                ui->plotter->getXAxis()->set_axisLabel(QString("\\left(\\verb{%1}\\right)^2  [(%2)^2]").arg(ui->cmbParameter1->currentText()).arg(unit1));
                ui->plotter->getYAxis()->set_axisLabel(QString("\\frac{\left(\\verb{%3}\\right)^2}{\\verb{%2}}  [(%6)^2/(%5)]").arg(ui->cmbParameter1->currentText()).arg(ui->cmbParameter2->currentText()).arg(ui->cmbParameter3->currentText()).arg(unit1).arg(unit2).arg(unit3));
            } else if (ui->cmbMode->currentIndex()==6) {
                ui->plotter->getYAxis()->set_axisLabel(QString("\\left(\\verb{%1}\\right)^2  [(%2)^2]").arg(ui->cmbParameter1->currentText()).arg(unit1));
                ui->plotter->getXAxis()->set_axisLabel(QString("\\frac{\left(\\verb{%1}\\right)^2}{\\verb{%2}}  [(%4)^2/(%5)]").arg(ui->cmbParameter1->currentText()).arg(ui->cmbParameter2->currentText()).arg(ui->cmbParameter3->currentText()).arg(unit1).arg(unit2).arg(unit3));
            } else if (ui->cmbMode->currentIndex()==7) {
                ui->plotter->getYAxis()->set_axisLabel(QString("\\left(\\verb{%1}\\right)^2 [(%2)^2]").arg(ui->cmbParameter1->currentText()).arg(unit1));
                ui->plotter->getXAxis()->set_axisLabel(QString("\\verb{%1} [%2]").arg(ui->cmbParameter2->currentText()).arg(unit2));
             }

            ui->plotter->get_plotter()->set_keyPosition(JKQTPkeyInsideTopLeft);

            JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(ui->plotter->get_plotter());
            g->set_title(tr("data"));
            g->set_color(QColor("red"));
            g->set_errorColor(g->get_color().darker());
            g->set_drawLine(false);
            g->set_symbol(JKQTPplus);

            g->set_xColumn(cX);
            g->set_xErrorColumn(cXerr);
            g->set_yColumn(cY);
            g->set_yErrorColumn(cYerr);
            if (ui->chkShowErrors->isChecked()) {
                g->set_xErrorStyle(JKQTPerrorSimpleBars);
                g->set_yErrorStyle(JKQTPerrorSimpleBars);
            } else {
                g->set_xErrorStyle(JKQTPnoError);
                g->set_yErrorStyle(JKQTPnoError);
            }
            ui->plotter->addGraph(g);

            ui->plainTextEdit->clear();

            if (ui->chkFitLin->isChecked()) {
                {
                    double a=0, b=0;
                    statisticsLinearRegression(X.data(), Y.data(), X.size(), a, b);
                    JKQTPxFunctionLineGraph* gfit=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
                    gfit->set_title(tr("fit: f(x) = %1 %2 \\cdot x = \\frac{x}{%3}%4").arg(doubleToLatexQString(a, 4, false)).arg(doubleToLatexQStringAlwaysSign(b, 4, false))
                                                                                      .arg(doubleToLatexQString(1.0/b, 4, false)).arg(doubleToLatexQStringAlwaysSign(a, 4, false)) );
                    gfit->set_params(constructQVectorFromItems(a, b));
                    gfit->setSpecialFunction(JKQTPxFunctionLineGraph::Line);
                    gfit->set_color(QColor("blue"));
                    ui->plotter->addGraph(gfit);
                    ui->plainTextEdit->setPlainText(ui->plainTextEdit->toPlainText()+tr("linear fit:\n   a=%1, b=%2; tau0=%1, D=%3  [ f(x)=tau0+Aeff/D ]\n\n").arg(a).arg(b).arg(1/b));
                }
                if (ui->chkFitWeighted->isChecked()) {
                    double a=0, b=0;
                    statisticsLinearWeightedRegression(X.data(), Y.data(), W.data(), X.size(), a, b);
                    JKQTPxFunctionLineGraph* gfit=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
                    gfit->set_title(tr("fit: f(x) = %1 %2 \\cdot x = \\frac{x}{%3}%4").arg(doubleToLatexQString(a, 4, false)).arg(doubleToLatexQStringAlwaysSign(b, 4, false))
                                                                                      .arg(doubleToLatexQStringAlwaysSign(1.0/b, 4, false)).arg(doubleToLatexQStringAlwaysSign(a, 4, false)) );
                    gfit->set_params(constructQVectorFromItems(a, b));
                    gfit->setSpecialFunction(JKQTPxFunctionLineGraph::Line);
                    gfit->set_color(QColor("blue"));
                    gfit->set_style(Qt::DashLine);
                    ui->plotter->addGraph(gfit);
                    ui->plainTextEdit->setPlainText(ui->plainTextEdit->toPlainText()+tr("linear fit:\n   a=%1, b=%2; tau0=%1, D=%3  [ f(x)=tau0+Aeff/D ]\n\n").arg(a).arg(b).arg(1/b));
                }
            }

            if (ui->chkFitQuadratic->isChecked()) {
                {
                    QVector<double> a;
                    a<<0<<0<<0;
                    statisticsPolyFit(X.data(), Y.data(), X.size(), 2, a.data());
                    JKQTPxFunctionLineGraph* gfit=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
                    gfit->set_title(tr("fit: f(x) = %1 %2 \\cdot x %3 \\cdot x^2").arg(doubleToLatexQString(a[0], 4, false)).arg(doubleToLatexQStringAlwaysSign(a[1], 4, false)).arg(doubleToLatexQStringAlwaysSign(a[2], 4, false)));
                    gfit->set_params(a);
                    gfit->setSpecialFunction(JKQTPxFunctionLineGraph::Polynomial);
                    gfit->set_color(QColor("darkgreen"));
                    ui->plotter->addGraph(gfit);
                    ui->plainTextEdit->setPlainText(ui->plainTextEdit->toPlainText()+tr("quadratic fit:\n   a=%1, b=%2, c=%3\n\n").arg(a[0]).arg(a[1]).arg(a[2]));
                }
            }
            if (ui->chkFitPowerlaw->isChecked()) {
                {
                    double a=0, b=0;
                    statisticsPowerLawRegression(X.data(), Y.data(), X.size(), a, b);
                    JKQTPxFunctionLineGraph* gfit=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
                    gfit->set_title(tr("fit: f(x) = %1 \\cdot x^{ %2}").arg(doubleToLatexQString(a, 4, false)).arg(doubleToLatexQString(b, 4, false)));
                    gfit->set_params(constructQVectorFromItems(a, b));
                    gfit->setSpecialFunction(JKQTPxFunctionLineGraph::PowerLaw);
                    gfit->set_color(QColor("darkorange"));
                    ui->plotter->addGraph(gfit);
                    ui->plainTextEdit->setPlainText(ui->plainTextEdit->toPlainText()+tr("powerlaw fit:\n   a=%1, power=%2\n\n").arg(a).arg(b));
                }

                if (ui->chkFitWeighted->isChecked()) {
                    double a=0, b=0;
                    statisticsLinearWeightedRegression(X.data(), Y.data(), W.data(), X.size(), a, b);
                    JKQTPxFunctionLineGraph* gfit=new JKQTPxFunctionLineGraph(ui->plotter->get_plotter());
                    gfit->set_title(tr("weighted fit: f(x) = %1 \\cdot x^{ %2}").arg(doubleToLatexQString(a, 4, false)).arg(doubleToLatexQString(b, 4, false)));
                    gfit->set_params(constructQVectorFromItems(a, b));
                    gfit->setSpecialFunction(JKQTPxFunctionLineGraph::PowerLaw);
                    gfit->set_color(QColor("darkorange"));
                    gfit->set_style(Qt::DashLine);
                    ui->plotter->addGraph(gfit);
                    ui->plainTextEdit->setPlainText(ui->plainTextEdit->toPlainText()+tr("weighted powerlaw fit:\n   a=%1, power=%2\n\n").arg(a).arg(b));
                }
            }


            ui->plotter->zoomToFit();
            ui->plotter->set_doDrawing(draw);
            ui->plotter->update_plot();
        }
    }

    saveConfig();
}

void QFRDRImagingFCSDiffusionLawDialog::on_btnTutorial_clicked()
{
    QFPluginServices::getInstance()->displayPluginHelpWindow("imaging_fcs", "difflaw.html");
}

void QFRDRImagingFCSDiffusionLawDialog::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayPluginHelpWindow("imaging_fcs", "tutorial_difflaw.html");
}

void QFRDRImagingFCSDiffusionLawDialog::on_btnSwap_clicked()
{
    int i1=ui->cmbParameter1->currentIndex();
    int i2=ui->cmbParameter2->currentIndex();
    disconnect(ui->cmbParameter2, SIGNAL(currentIndexChanged(int)), this, SLOT(paramChanged()));
    ui->cmbParameter2->setCurrentIndex(i1);
    connect(ui->cmbParameter2, SIGNAL(currentIndexChanged(int)), this, SLOT(paramChanged()));
    ui->cmbParameter1->setCurrentIndex(i2);

}

void QFRDRImagingFCSDiffusionLawDialog::on_edtScale1_editingFinished()
{
    if (!ui->chkDefaultScale1->isChecked()) {
        recalcPlot();
    }
}

void QFRDRImagingFCSDiffusionLawDialog::on_edtScale2_editingFinished()
{
    if (!ui->chkDefaultScale2->isChecked()) {
        recalcPlot();
    }
}

void QFRDRImagingFCSDiffusionLawDialog::on_cmbMode_currentIndexChanged(int index)
{

    /*
        0   simple P2 vs. P1 (e.g. tau vs. Aeff)
        1   (P1/P2) vs. P1 (e.g. Aeff/D vs. Aeff)
        2   (P1²/P2) vs. P1² (e.g. Wxy²/D vs. Wxy²)
        3   P2 vs. P1² (e.g. tauD vs. Wxy²)
        4   P3/P2 vs. P1² (e.g. Aeff/D vs. dx²)
        5   P3²/P2 vs. P1² (e.g. Wxy²/D vs. dx²)
        6   P1² vs. P1²/P2(e.g. dx² vs. Wxy²/D)
        7   P1² vs. P2 (e.g. dx² vs. tauD)

     */
    bool has3=(index==4)||(index==5);
    ui->edtScale3->setVisible(has3);
    ui->chkDefaultScale3->setVisible(has3);
    ui->cmbParameter3->setVisible(has3);
    ui->lab31->setVisible(has3);
    ui->lab32->setVisible(has3);
}
