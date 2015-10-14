/*
Copyright (c) 2012-2015 by Sebastian Isbaner

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

#include "DialogGMMparams.h"
#include "ui_DialogGMMparams.h"

DialogGMMparams::DialogGMMparams(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGMMparams)
{
    ui->setupUi(this);


    ui->spinBoxDim->setDisabled(true);

    qDebug()<<"setup DialogGMMparams";
    ui->tableWidgetMean->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->tableWidgetMean->setSortingEnabled(false);
    ui->tableWidgetMean->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableWidgetMean->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetMean->setShowGrid(true);
    ui->tableWidgetSigma->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->tableWidgetSigma->setSortingEnabled(false);
    ui->tableWidgetSigma->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableWidgetSigma->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetSigma->setShowGrid(true);

    iniFile="GMMparams.ini";

    readFromIni(iniFile.toStdString());
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(updateTables(int)));
    fillList();

    connect(ui->spinBoxDim,SIGNAL(valueChanged(int)),this,SLOT(dimChanged(int)));
    connect(ui->spinBoxComp,SIGNAL(valueChanged(int)),this,SLOT(componentsChanged(int)));

    connect(ui->buttonBox,SIGNAL(accepted()), this, SLOT(saveAll()));




//    jkINIParser2 ini; // initialize to read the ini file
//    try
//    {
//        ini.set("nModels",(int64_t)GMMfitData::nModels);
//        for(int modelID=GMMfitData::LowFRET;modelID<GMMfitData::nModels;modelID++) {
//            models->init((GMMfitData::Model)modelID);
//            GMMfitModel model(*(models->gmm),models->getModelName((GMMfitData::Model)modelID));
//            stringstream stream;
//            stream<<"GMMfitModel"<<modelID;
//            model.writeToIni(ini,stream.str());
//        }
//        ini.writeFile("GMMparams.ini");
//    }
//    catch (jkINIParser2::error e)
//    { // error handling
//       AlexEvalLog::warning(QString::fromStdString(e.getMessage()));
//    }

}

DialogGMMparams::~DialogGMMparams()
{
    delete ui;
}

void DialogGMMparams::setDimensionName(QStringList &names)
{
    m_DimensionHeader=names;
    ui->tableWidgetMean->setVerticalHeaderLabels(m_DimensionHeader);
    ui->tableWidgetSigma->setVerticalHeaderLabels(m_DimensionHeader);
}


void DialogGMMparams::fillTable(GMMfit *gmm)
{
    qDebug()<<"fillTable";
    m_MeanHeader.clear();
    m_SigmaHeader.clear();

    ui->tableWidgetMean->clearContents();
    ui->tableWidgetSigma->clearContents();

    for( int i=0; i<gmm->getNumberOfComponents();++i) {
        m_MeanHeader<<"mu_"+QString::number(i+1);
    }
    ui->tableWidgetMean->setColumnCount(m_MeanHeader.size());
    ui->tableWidgetMean->setRowCount(gmm->getDim());
    ui->tableWidgetMean->setHorizontalHeaderLabels(m_MeanHeader);
    ui->tableWidgetMean->setVerticalHeaderLabels(m_DimensionHeader);
    for( int i=0; i<gmm->getDim();++i) {
        for( int j=0; j<gmm->getNumberOfComponents();++j) {
            ui->tableWidgetMean->setItem(i, j, new QTableWidgetItem(QString::number(gmm->getMu(j,i))));
        }
    }

    for( int j=0; j<gmm->getNumberOfComponents();++j) {
        for( int i=0; i<gmm->getDim();++i) {
            m_SigmaHeader<<"sigma_"+QString::number(j)+"_"+QString::number(i+1);
        }
    }

    ui->tableWidgetSigma->setColumnCount(m_SigmaHeader.size());
    ui->tableWidgetSigma->setRowCount(gmm->getDim());
    ui->tableWidgetSigma->setHorizontalHeaderLabels(m_SigmaHeader);
    ui->tableWidgetSigma->setVerticalHeaderLabels(m_DimensionHeader);
    for( int j=0; j<gmm->getNumberOfComponents();++j) {
        for( int i=0; i<gmm->getDim();++i) {
            for( int k=0; k<gmm->getDim();++k) {
                ui->tableWidgetSigma->setItem(i, j*gmm->getDim()+k, new QTableWidgetItem(QString::number(gmm->getGauss(j).getSigma()(i,k))));
            }
        }
    }



}

GMMfit * DialogGMMparams::getFromTable()
{
    int d=ui->spinBoxDim->value();
    int k=ui->spinBoxComp->value();
    GMMfit * gmm=new GMMfit(d,k);
    getFromTable(gmm);
    return gmm;
}

void DialogGMMparams::getFromTable(GMMfit *gmm)
{
    int d=ui->spinBoxDim->value();
    int k=ui->spinBoxComp->value();

    if(gmm->getDim()!=d || gmm->getNumberOfComponents()!=k) {
        qDebug()<<"error getFromTable";
    }

    VectorXd mu;
    MatrixXd sigma;
    mu=VectorXd::Zero(d);
    sigma=MatrixXd::Zero(d,d);

    for(int j=0;j<k;++j) {
        for(int i=0;i<d;++i) {
            mu(i)=ui->tableWidgetMean->item(i,j)->data(Qt::EditRole).toDouble();
        }
        gmm->getGauss(j).setMu(mu);

        for(int i=0;i<d;++i) {
            for(int l=0;l<d;++l) {
                sigma(i,l)=ui->tableWidgetSigma->item(i,j*d+l)->data(Qt::EditRole).toDouble();
            }
        }
        gmm->getGauss(j).setSigma(sigma);
    }


}

void DialogGMMparams::fillList()
{
    disconnect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(updateTables(int)));
    for(int i=ui->listWidget->count();i<models.size();++i) {
        ui->listWidget->addItem(models.at(i).getName());
    }
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(updateTables(int)));
}



GMMfitModel::GMMfitModel(GMMfit &gmm, QString name)
{
    this->gmm=&gmm;
    this->name=name;
}

GMMfitModel::GMMfitModel(GMMfit *gmm, QString name)
{
    this->gmm=gmm;
    this->name=name;
}

GMMfitModel::GMMfitModel()
{
    gmm=new GMMfit();
    name="default";
}

GMMfitModel::~GMMfitModel()
{
//    delete gmm;
}


QString GMMfitModel::toString() const
{
    return QString::fromStdString(gmm->toString());
}

QString GMMfitModel::resultToString(const QString sampleName) const
{
    QString str;
    QTextStream out(&str);
    for(int i=0;i<gmm->getNumberOfComponents();++i) {
        out<<name<<"\t"<<i<<"/"<<gmm->getNumberOfComponents()<<"\t"<<gmm->getPrior(i)<<"\t"<<gmm->getMu(i,0)<<"\t"
          <<gmm->getMu(i,1)<<"\t"<<gmm->getGauss(i).getSigma()(0,0)<<"\t"<<gmm->getGauss(i).getSigma()(1,1)<<"\t"
         <<gmm->getGauss(i).getSigma()(0,1)<<"\t"<<gmm->getN()<<"\t"<<sampleName<<"\n";
    }
    return str;
}

QString GMMfitModel::resultToStringHeader() const
{
    return QString("model name\tcomponent\tweight\tP\tS\tdeltaP\tdeltaS\tcorrPS\tN\tsample name\n");
}


void GMMfitModel::writeToIni(jkINIParser2 &ini, std::string group) const
{
    ini.enterGroup(group);
    ini.set("name",name.toStdString());
    ini.set("dimension",(int64_t)gmm->getDim());
    ini.set("components",(int64_t)gmm->getNumberOfComponents());
    for(int j=0;j<gmm->getNumberOfComponents();++j) {
        for(int i=0;i<gmm->getDim();++i) {
            stringstream stream;
            stream<<"mu"<<"_"<<j<<"_"<<i;
            ini.set(stream.str(),gmm->getGauss(j).getMu()(i));
        }

        for(int i=0;i<gmm->getDim();++i) {
            for(int l=0;l<gmm->getDim();++l) {
                stringstream stream;
                stream<<"sigma"<<"_"<<j<<"_"<<i<<"_"<<l;
                ini.set(stream.str(),gmm->getGauss(j).getSigma()(i,l));
            }
        }
    }
    ini.leaveGroup();

}

int GMMfitModel::readFromIni(jkINIParser2 &ini, string group)
{
    if(gmm!=NULL) delete gmm;
    ini.enterGroup(group);
    name=QString::fromStdString(ini.getAsString("name",""));
    int d=ini.getAsInt("dimension",0);
    int k=ini.getAsInt("components",0);
    gmm=new GMMfit(d,k);

    VectorXd mu;
    MatrixXd sigma;


    for(int j=0;j<k;++j) {
        mu=VectorXd::Zero(d);
        sigma=MatrixXd::Zero(d,d);

        for(int i=0;i<d;++i) {
            stringstream stream;
            stream<<"mu"<<"_"<<j<<"_"<<i;
            mu(i)=ini.getAsDouble(stream.str());
        }
        gmm->getGauss(j).setMu(mu);

         for(int i=0;i<d;++i) {
            for(int l=0;l<d;++l) {
                stringstream stream;
                stream<<"sigma"<<"_"<<j<<"_"<<i<<"_"<<l;
                sigma(i,l)=ini.getAsDouble(stream.str());
            }
         }
         gmm->getGauss(j).setSigma(sigma);
    }
    ini.leaveGroup();

    return EXIT_SUCCESS;
}



int DialogGMMparams::writeToIni(const std::string iniFilename) const
{
    jkINIParser2 ini; // initialize to read the ini file
    if (iniFilename.empty()) return EXIT_FAILURE;

    try
    {
        ini.set("nModels",(int64_t)models.size());
        for(int i=0;i<models.size();++i) {
            models[i].writeToIni(ini,("GMMfitModel"+QString::number(i)).toStdString());
        }

        ini.writeFile(iniFilename);
    }
    catch (jkINIParser2::error e)
    { // error handling
       qDebug()<<QString::fromStdString(e.getMessage());
       AlexEvalLog::warning(QString::fromStdString(e.getMessage()));
       return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int DialogGMMparams::readFromIni(const std::string iniFilename)
{
    jkINIParser2 ini; // initialize to read the ini file
    if (iniFilename.empty()) return EXIT_FAILURE;

    models.clear();
    try
    {
        ini.readFile(iniFilename); // read in an INI file
        int nModels=ini.getAsInt("nModels");
        for(int i=0;i<nModels;++i) {
            GMMfitModel model;
            model.readFromIni(ini,("GMMfitModel"+QString::number(i)).toStdString());
            models<<model;
        }

        qDebug()<<QString::fromStdString(models.first().getGMM()->toString());
    }
    catch (jkINIParser2::error e)
    { // error handling
        qDebug()<<QString::fromStdString(e.getMessage());
       AlexEvalLog::warning(QString::fromStdString(e.getMessage()));
       return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void DialogGMMparams::saveAll()
{
    qDebug()<<"saveAll"<<iniFile;
    if(writeToIni(iniFile.toStdString())==EXIT_SUCCESS) qDebug()<<"all models saved in"<<iniFile;
}

void DialogGMMparams::dimChanged(int /*dim*/)
{
    ui->spinBoxDim->setDisabled(true);
}

void DialogGMMparams::updateTables(int index)
{
    qDebug()<<"updateTables";
    if(index>=models.size()) return;
    fillTable(models[index].getGMM());
    ui->lineEditName->setText(models[index].getName());

    ui->spinBoxDim->setValue(models[index].getGMM()->getDim());
    ui->spinBoxComp->setValue(models[index].getGMM()->getNumberOfComponents());
}

void DialogGMMparams::on_pushButtonSave_clicked()
{
    qDebug()<<"on_pushButtonSave_clicked";
    getFromTable(models[ui->listWidget->currentRow()].getGMM());
    models[ui->listWidget->currentRow()].setName(ui->lineEditName->text());
    ui->listWidget->currentItem()->setText(ui->lineEditName->text());
}


void DialogGMMparams::on_pushButtonModel_clicked()
{
    GMMfit* m=getFromTable();
    GMMfitModel model(*m,ui->lineEditName->text());
    models.append(model);
    fillList();

    disconnect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(updateTables(int)));
    ui->listWidget->setCurrentRow(models.size()-1);
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(updateTables(int)));
}

void DialogGMMparams::on_pushButtonRemove_clicked()
{
    disconnect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(updateTables(int)));
    int item=ui->listWidget->currentRow();
    models.removeAt(item);
    ui->listWidget->clear();
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(updateTables(int)));
    fillList();
}


void DialogGMMparams::componentsChanged(int k)
{
    GMMfit *newGMM=new GMMfit(ui->spinBoxDim->value(),k);

    VectorXd mu;
    MatrixXd sigma;

    int d=ui->spinBoxDim->value();
    int item=ui->listWidget->currentRow();

    int kOld=models[item].getGMM()->getNumberOfComponents();
    for(int j=0;j<qMin(k,kOld);++j) {
        mu=VectorXd::Zero(d);
        sigma=MatrixXd::Zero(d,d);

        mu=models[item].getGMM()->getGauss(j).getMu();
        newGMM->getGauss(j).setMu(mu);

        sigma=models[item].getGMM()->getGauss(j).getSigma();
        newGMM->getGauss(j).setSigma(sigma);
    }

    for(int j=kOld;j<k;++j) {
        newGMM->getGauss(j).setMu(mu);

        newGMM->getGauss(j).setSigma(sigma);
    }

    models[item].setGMM(newGMM);
    fillTable(newGMM);
}
