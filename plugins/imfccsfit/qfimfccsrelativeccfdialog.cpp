#include "qfimfccsrelativeccfdialog.h"
#include "ui_qfimfccsrelativeccfdialog.h"
#include "qfmatchrdrfunctor.h"
#include "qfimfccsfitevaluation_item.h"
#include "qfpluginservices.h"

QFImFCCSRelativeCCFDialog::QFImFCCSRelativeCCFDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFImFCCSRelativeCCFDialog)
{
    ui->setupUi(this);
    matchFunctor=new QFImFCCSMatchRDRFunctor();
    QFProject* project=QFPluginServices::getInstance()->getCurrentProject();
    QList<QPointer<QFRawDataRecord> > lst=matchFunctor->getFilteredList(project);
    ui->cmbACF->init(project, matchFunctor);
    ui->cmbCCF->init(project, matchFunctor);

    bool okACF=false;
    bool okCCF=false;
    for (int i=0; i<lst.size(); i++) {
        if (lst[i]) {
            if (!okACF && lst[i]->getRole().toUpper().startsWith("ACF")) {
                ui->cmbACF->setCurrentRDR(lst[i]);
                okACF=true;
            }
            if (!okCCF && lst[i]->getRole().toUpper().contains("FCCS")) {
                ui->cmbCCF->setCurrentRDR(lst[i]);
                okCCF=true;
            }
        }
        if (okACF&&okCCF) break;
    }

}

QFImFCCSRelativeCCFDialog::~QFImFCCSRelativeCCFDialog()
{
    delete ui;
    delete matchFunctor;
}

QFRawDataRecord *QFImFCCSRelativeCCFDialog::getACF() const
{
    return ui->cmbACF->currentRDR();
}

QFRawDataRecord *QFImFCCSRelativeCCFDialog::getCCF() const
{
    return ui->cmbCCF->currentRDR();
}

void QFImFCCSRelativeCCFDialog::on_cmbACF_currentIndexChanged(int index)
{
    QFRawDataRecord* acf=ui->cmbACF->currentRDR();
    QFProject* project=QFPluginServices::getInstance()->getCurrentProject();
    if (acf && project) {
        QString arole=acf->getRole().toUpper();
        bool ok=true;
        if (!(arole.startsWith("ACF") || arole.isEmpty() || acf->getName().toLower().contains("acf"))) {
            ok=QMessageBox::warning(this, windowTitle(), tr("the ACF file you selected seems to be no ACF file (role is '%1').\nClicking IGNORE, this function will still try to find a fitting CCF file").arg(arole), QMessageBox::Ok|QMessageBox::Ignore, QMessageBox::Ignore)==QMessageBox::Ignore;
        }
        if (ok) {
            QString adir=acf->getFolder();
            QString ainput=acf->getFileForType("input");
            QFRawDataRecord* guess1=NULL;
            QFRawDataRecord* guess2=NULL;
            QFRawDataRecord* guess3=NULL;
            QList<QPointer<QFRawDataRecord> > lst=matchFunctor->getFilteredList(project);
            for (int i=0; i<lst.size(); i++) {
                if (lst[i] && lst[i]!=acf) {
                    QString crole=lst[i]->getRole().toUpper();
                    QString cdir=lst[i]->getFolder();
                    QString cinput=lst[i]->getFileForType("input");
                    if ((crole.startsWith("FCCS")||crole.startsWith("DCCF")||lst[i]->getName().toLower().contains("fccs")||lst[i]->getName().toLower().contains("dccf"))) {
                        if (! (ainput.isEmpty() || cinput.isEmpty())) {
                            if (QFileInfo(cinput)==QFileInfo(ainput)) {
                                guess1=lst[i];
                            }
                        }
                        if (! (adir.isEmpty() || cdir.isEmpty())) {
                            if (adir==cdir) {
                                guess2=lst[i];
                            }
                        }
                    }
                }
            }
            if (guess1) ui->cmbCCF->setCurrentRDR(guess1);
            else if (guess2) ui->cmbCCF->setCurrentRDR(guess2);
        }
    }
}
