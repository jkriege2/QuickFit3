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

#include "qfespectraviewerfretdialog.h"
#include "ui_qfespectraviewerfretdialog.h"
#include "programoptions.h"

QFESpectraViewerFRETDialog::QFESpectraViewerFRETDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFESpectraViewerFRETDialog)
{
    ui->setupUi(this);
    model.setReadonly(false);
    ui->tableView->setModel(&model);
    ui->spinKappa2->setValue(ProgramOptions::getConfigValue("QFESpectraViewerFRETDialog/kappa2", 2.0/3.0).toDouble());
    ui->spinN->setValue(ProgramOptions::getConfigValue("QFESpectraViewerFRETDialog/n", 1.333).toDouble());
}

QFESpectraViewerFRETDialog::~QFESpectraViewerFRETDialog()
{
    ProgramOptions::setConfigValue("QFESpectraViewerFRETDialog/kappa2", ui->spinKappa2->value());
    ProgramOptions::setConfigValue("QFESpectraViewerFRETDialog/n", ui->spinN->value());
    delete ui;
}


void QFESpectraViewerFRETDialog::init(const QList<QFESpectraViewerPlotItem> &plotItems, SpectrumManager *manager)
{
    this->manager=manager;
    fluorophores.clear();
    for (int i=0; i<plotItems.size(); i++) {
        if (plotItems[i].type==qfesFluorohpore && manager->fluorophoreExists(plotItems[i].name)) {
            fluorophores<<manager->getFluorophoreData(plotItems[i].name);
        }
    }


    updateFRET();
}

void QFESpectraViewerFRETDialog::updateFRET()
{
    model.disableSignals();
    model.setReadonly(false);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));


    model.clear();

    for (int i=0; i<fluorophores.size(); i++) {
        int c=0;
        model.setCellCreate(i+2, 0, fluorophores[i].name);
        model.setColumnTitleCreate(0, tr("donor fluorophore"));
        model.setCellCreate(0, 0, tr("QE [%]"));
        model.setColumnTitleCreate(1, tr("QE [%]"));
        model.setCellCreate(1, 0, tr("extinction [1/(M cm)]"));
        model.setColumnTitleCreate(2, tr("extinction [1/(M cm)]"));
        c++;
        model.setCellCreate(i+2, c, fluorophores[i].fluorescence_efficiency*100.0);
        model.setCellCreate(0, i+3, fluorophores[i].fluorescence_efficiency*100.0);
        //model.setColumnTitleCreate(1, tr("QE [%]"));
        c++;
        model.setCellCreate(i+2, c, fluorophores[i].extiction_coefficient);
        model.setCellCreate(1, i+3, fluorophores[i].extiction_coefficient);
        //model.setColumnTitleCreate(2, tr("extinction [1/(M cm)]"));
        c++;
        for (int j=0; j<fluorophores.size(); j++) {
            double kappa2=ui->spinKappa2->value();
            double n=ui->spinN->value();
            double Q=fluorophores[i].fluorescence_efficiency;

            SpectrumManager::Spectrum* sDonor=manager->getSpectrum(fluorophores[i].spectrum_fl);
            SpectrumManager::Spectrum* sAcceptor=manager->getSpectrum(fluorophores[j].spectrum_abs);
            double lmin=qMin(sDonor->getWavelengthMin(), sAcceptor->getWavelengthMin());
            double lmax=qMin(sDonor->getWavelengthMax(), sAcceptor->getWavelengthMax());

            //qDebug()<<"donor: "<<fluorophores[i].name<<"      acceptor: "<<fluorophores[j].name;
            //qDebug()<<"lrange: "<<lmin<<lmax;

            double J=sDonor->getMulSpectrumIntegral(sAcceptor, lmin, lmax, 4.0, 1.0)*fluorophores[j].extiction_coefficient;
            double Jn=sDonor->getSpectrumIntegral(lmin, lmax)*sAcceptor->getSpectrumMax();
            //qDebug()<<"kappa2="<<kappa2<<"  n="<<n<<"  Q="<<Q;
            //qDebug()<<"acceptor epsilon="<<fluorophores[j].extiction_coefficient;
            //qDebug()<<"Jint="<<sDonor->getMulSpectrumIntegral(sAcceptor, lmin, lmax, 4.0, 1.0);
            //qDebug()<<"J="<<J;
            //qDebug()<<"JN="<<Jn;
            //qDebug()<<"JJ="<<J/Jn;
            double fret=0.211*pow(J/Jn*Q/pow(n, 4)*kappa2, 1.0/6.0);
                    //pow(9000.0*Q*M_LN10*kappa2*J/Jn/128.0/pow(M_PI, 5)/pow(n, 4)/QF_NAVOGADRO, 1.0/6.0)*10.0;
            qDebug()<<"R0="<<fret<<" Angstrom";
            model.setCellCreate(i+2, c, fret);
            model.setColumnTitleCreate(c, fluorophores[j].name);
            c++;
        }
    }

    model.setReadonly(true);
    model.enableSignals(true);
    ui->tableView->setModel(&model);
    ui->tableView->resizeColumnsToContents();
    QApplication::restoreOverrideCursor();
}
