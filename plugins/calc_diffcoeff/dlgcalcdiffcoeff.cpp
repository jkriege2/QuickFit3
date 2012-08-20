#include "dlgcalcdiffcoeff.h"
#include "ui_dlgcalcdiffcoeff.h"
#include "qfediffusioncoefficientcalculator.h"
#include "programoptions.h"

DlgCalcDiffCoeff::DlgCalcDiffCoeff(QFEDiffusionCoefficientCalculator *plg, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgCalcDiffCoeff)
{
    updating=false;
    this->plugin=plg;
    tab=new QFTableModel(this);
    ui->setupUi(this);
    c_temp=-1;
    c_D=-1;
    c_visc=-1;
    c_density=-1;
    ui->tableView->setModel(tab);
    ui->plotter->get_plotter()->getXAxis()->set_axisLabel(tr("temperature [°C]"));
    ui->edtGivenD->setValue(20);
    ui->edtGivenD->setRange(0,1e10);
    ui->edtGivenDVisc->setRange(0,1e10);
    ui->edtGivenDVisc->setValue(1);
    ui->spinGivenDT->setValue(20);
    ui->plotter->getYAxis()->set_labelDigits(4);
    ui->plotter->getYAxis()->set_minTicks(5);
    ui->plotter->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    QList<int> s;
    s<<ui->splitter->width()/2<<ui->splitter->width()/2;
    ui->splitter->setSizes(s);
    readSamples();
    readSettings();
    updateD();
    QTimer::singleShot(10000, this, SLOT(writeSettings()));
}

DlgCalcDiffCoeff::~DlgCalcDiffCoeff()
{
    delete ui;
}

void DlgCalcDiffCoeff::updateD() {
    if (updating) return;
    // calculate solution properties
    tab->setReadonly(false);
    tab->clear();
    tab->appendColumn();
    tab->appendColumn();
    tab->appendColumn();
    tab->appendColumn();
    tab->setColumnTitle(0, tr("temperature [°C]"));
    tab->setColumnTitle(1, tr("D [µm²/s]"));
    tab->setColumnTitle(2, tr("viscosity [mPa s]"));
    tab->setColumnTitle(3, tr("density [kg/l]"));
    temp.clear();
    D.clear();
    visc.clear();
    density.clear();
    for (double T=ui->spinSolutionTemperature->value(); T<=ui->spinSolutionTemperatureEnd->value(); T=T+ui->spinSolutionTemperatureDelta->value()) {
        temp.append(T);
        visc.append(plugin->getSolutionViscosity(ui->cmbSolutionName->currentIndex(), 273.15+T)*1000.0);
        density.append(plugin->getSolutionDensity(ui->cmbSolutionName->currentIndex(), 273.15+T));
        if (ui->tabWidget->currentIndex()==0) {
            D.append(plugin->getSphereDCoeff(ui->cmbSolutionName->currentIndex(), ui->spinSPhereDiameter->value()*1e-9, 273.15+T)*1e12);
            ui->labSphereD->setText(tr("D<sub>20,W</sub> = %1 &mu;m<sup>2</sup>/s").arg(plugin->getSphereDCoeff(0, ui->spinSPhereDiameter->value()*1e-9, 293.15)*1e12));
        } else if (ui->tabWidget->currentIndex()==1) {
            D.append(plugin->getDCoeff_from_D(ui->cmbSolutionName->currentIndex(), ui->edtGivenD->value()/1e12, ui->edtGivenDVisc->value()/1000.0, ui->spinGivenDT->value()+273.15, 273.15+T)*1e12);
            ui->labGivenDD20W->setText(tr("D<sub>20,W</sub> = %1 &mu;m<sup>2</sup>/s").arg(plugin->getDCoeff_from_D(0, ui->edtGivenD->value()/1e12, ui->edtGivenDVisc->value()/1000.0, ui->spinGivenDT->value()+273.15, 293.15)*1e12));
        } else if (ui->tabWidget->currentIndex()==2) {
            D.append(plugin->getDCoeff_from_D20W(ui->cmbSolutionName->currentIndex(), ui->spinGivenD20W->value()/1e12, 273.15+T)*1e12);
        } else  {
            D.append(0);
        }
        tab->appendRow();
        tab->setCell(tab->rowCount()-1, 0, temp.last());
        tab->setCell(tab->rowCount()-1, 1, D.last());
        tab->setCell(tab->rowCount()-1, 2, visc.last());
        tab->setCell(tab->rowCount()-1, 3, density.last());
    }
    tab->setReadonly(true);
    updatePlot();
}

void DlgCalcDiffCoeff::updateGivenD() {
    QString filename=QFPluginServices::getInstance()->getAssetsDirectory()+"/plugins/"+plugin->getID()+"/samples.ini";
    QSettings set(filename, QSettings::IniFormat);

    updating=true;

    QString name=ui->cmbGivenDName->currentText();
    //ui->cmbGivenDName->clear();
    // search for the name
    QStringList groups=set.childGroups();
    for (int i=0; i<groups.size(); i++) {
        QString n=set.value(groups[i]+"/name", "").toString();
        if (n==name) {
            ui->spinGivenDT->setValue(set.value(groups[i]+"/temperature", 20).toDouble());
            ui->edtGivenD->setValue(set.value(groups[i]+"/D", 10).toDouble());
            ui->edtGivenDVisc->setValue(set.value(groups[i]+"/viscosity", 1).toDouble());
            //ui->cmbGivenDName->setToolTip(set.value(groups[i]+"/reference", "").toString());
            QVariant ref=set.value(groups[i]+"/reference", "");
            if (ref.type()==QVariant::StringList) ref=ref.toStringList().join(", ");
            ui->edtReference->setText(ref.toString());
            break;
        }
    }

    updating=false;
    updateD();
}

void DlgCalcDiffCoeff::on_btnSaveGivenD_clicked() {
    QString filename=QFPluginServices::getInstance()->getAssetsDirectory()+"/plugins/"+plugin->getID()+"/samples.ini";

    QSettings set(filename, QSettings::IniFormat);

    if (set.isWritable()) {

        QString name=ui->cmbGivenDName->currentText();

        // search for the name
        QStringList groups=set.childGroups();
        int idx_max=0;
        for (int i=0; i<groups.size(); i++) {
            QRegExp rx("sample(\\d+)", Qt::CaseInsensitive);
            if (rx.indexIn(groups[i])>-1) {
                idx_max=rx.cap(1).toInt();
            }
        }

        idx_max++;
        set.setValue(QString("sample%1/name").arg(idx_max), name);
        set.setValue(QString("sample%1/D").arg(idx_max), ui->edtGivenD->value());
        set.setValue(QString("sample%1/viscosity").arg(idx_max), ui->edtGivenDVisc->value());
        set.setValue(QString("sample%1/temperature").arg(idx_max), ui->spinGivenDT->value());
        set.setValue(QString("sample%1/reference").arg(idx_max), ui->edtReference->text());
        ui->cmbGivenDName->addItem(name);
    }

}

void DlgCalcDiffCoeff::updatePlot()
{
    if (updating) return;
    ui->plotter->set_doDrawing(false);
    ui->plotter->clearGraphs();
    ui->plotter->getDatastore()->clear();
    c_temp=ui->plotter->getDatastore()->addColumn(temp.data(), temp.size(), "temperature [°C]");
    c_D=ui->plotter->getDatastore()->addColumn(D.data(), D.size(), "D [µm²/s]");
    c_visc=ui->plotter->getDatastore()->addColumn(visc.data(), visc.size(), "viscosity [mPa s]");
    c_density=ui->plotter->getDatastore()->addColumn(density.data(), density.size(), "density [kg/l]");
    ui->plotter->get_plotter()->getYAxis()->set_axisLabel(tr(""));

    if (ui->cmbPlot->currentIndex()==0) {
        JKQTPxyLineGraph* g=new JKQTPxyLineGraph(ui->plotter->get_plotter());
        g->set_drawLine(true);
        g->set_xColumn(c_temp);
        g->set_yColumn(c_D);
        ui->plotter->addGraph(g);
        ui->plotter->get_plotter()->getYAxis()->set_axisLabel(tr("diffusion coefficient [µm²/s]"));
    } else if (ui->cmbPlot->currentIndex()==1) {
        JKQTPxyLineGraph* g=new JKQTPxyLineGraph(ui->plotter->get_plotter());
        g->set_drawLine(true);
        g->set_xColumn(c_temp);
        g->set_yColumn(c_density);
        ui->plotter->addGraph(g);
        ui->plotter->get_plotter()->getYAxis()->set_axisLabel(tr("solution density [kg/l]"));
    } else if (ui->cmbPlot->currentIndex()==2) {
        JKQTPxyLineGraph* g=new JKQTPxyLineGraph(ui->plotter->get_plotter());
        g->set_drawLine(true);
        g->set_xColumn(c_temp);
        g->set_yColumn(c_visc);
        ui->plotter->addGraph(g);
        ui->plotter->get_plotter()->getYAxis()->set_axisLabel(tr("solution viscosity [mPa{\\cdot}s]"));
    }

    ui->plotter->zoomToFit();
    ui->plotter->set_doDrawing(true);
    ui->plotter->update_plot();
}

void DlgCalcDiffCoeff::readSettings() {
    ProgramOptions* opt=ProgramOptions::getInstance();
    if (opt) {
        QSettings* set=opt->getQSettings();
        if (set) {
            loadWidgetGeometry(*set, this, plugin->getID()+"/widget/");
            loadSplitter(*set, ui->splitter, plugin->getID()+"/splitter/");
            ui->cmbPlot->setCurrentIndex(set->value(plugin->getID()+"/plot", 0).toInt());
            ui->tabWidget->setCurrentIndex(set->value(plugin->getID()+"/sample", 0).toInt());
            ui->spinSolutionTemperature->setValue(set->value(plugin->getID()+"/temperature", 10).toDouble());
            ui->spinSolutionTemperatureEnd->setValue(set->value(plugin->getID()+"/temperature_end", 40).toDouble());
            ui->spinSolutionTemperatureDelta->setValue(set->value(plugin->getID()+"/temperature_delta", 0.5).toDouble());
            ui->spinSPhereDiameter->setValue(set->value(plugin->getID()+"/diameter", 100).toDouble());
            //ui->edtGivenD->setValue(set->value(plugin->getID()+"/givenD/D", 100).toDouble());
            //ui->edtGivenDVisc->setValue(set->value(plugin->getID()+"/givenD/visc", 1).toDouble());
            //ui->spinGivenDT->setValue(set->value(plugin->getID()+"/givenD/Temp", 20).toDouble());
            //ui->spinGivenD20W->setValue(set->value(plugin->getID()+"/givenD20W", 100).toDouble());

            ui->cmbSolutionName->setCurrentIndex(set->value(plugin->getID()+"/solution", 0).toInt());
            ui->cmbGivenDName->setCurrentIndex(set->value(plugin->getID()+"/givenD/name", 0).toInt());

            updateD();
            updateGivenD();
        }
    }
}

void DlgCalcDiffCoeff::writeSettings() {
    ProgramOptions* opt=ProgramOptions::getInstance();
    if (opt) {
        QSettings* set=opt->getQSettings();
        if (set) {
            saveWidgetGeometry(*set, this, plugin->getID()+"/widget/");
            saveSplitter(*set, ui->splitter, plugin->getID()+"/splitter/");
            set->setValue(plugin->getID()+"/plot", ui->cmbPlot->currentIndex());
            set->setValue(plugin->getID()+"/solution", ui->cmbSolutionName->currentIndex());
            set->setValue(plugin->getID()+"/sample", ui->tabWidget->currentIndex());
            set->setValue(plugin->getID()+"/temperature", ui->spinSolutionTemperature->value());
            set->setValue(plugin->getID()+"/temperature_end", ui->spinSolutionTemperatureEnd->value());
            set->setValue(plugin->getID()+"/temperature_delta", ui->spinSolutionTemperatureDelta->value());
            set->setValue(plugin->getID()+"/diameter", ui->spinSPhereDiameter->value());
            set->setValue(plugin->getID()+"/givenD/D", ui->edtGivenD->value());
            set->setValue(plugin->getID()+"/givenD/visc", ui->edtGivenDVisc->value());
            set->setValue(plugin->getID()+"/givenD/Temp", ui->spinGivenDT->value());
            set->setValue(plugin->getID()+"/givenD/name", ui->cmbGivenDName->currentIndex());

            set->setValue(plugin->getID()+"/givenD20W", ui->spinGivenD20W->value());


        }
    }
}

void DlgCalcDiffCoeff::readSamples() {
    QString filename=QFPluginServices::getInstance()->getAssetsDirectory()+"/plugins/"+plugin->getID()+"/samples.ini";
    //qDebug()<<"reading samples.ini: "<<filename;
    QSettings set(filename, QSettings::IniFormat);

    updating=true;


    ui->cmbGivenDName->clear();
    // search for the name
    QStringList groups=set.childGroups();
//    qDebug()<<"reading samples.ini: groups: "<<groups;
    for (int i=0; i<groups.size(); i++) {
        set.beginGroup(groups[i]);
        QString n=set.value("name", "").toString();
        //qDebug()<<i<<n;
        ui->cmbGivenDName->addItem(n);
        set.endGroup();
    }

    updating=false;
    updateD();
}

void DlgCalcDiffCoeff::updateReport(const QModelIndex &index) {
    QVariant d=ui->tableView->model()->data(index);
    switch (index.column()) {
        case 0:
            plugin->setReportVal("temperature", d.toDouble());
            break;
        case 1:
            plugin->setReportVal("diff_coeff", d.toDouble());
            break;
        case 2:
            plugin->setReportVal("viscosity", d.toDouble());
            break;
        case 3:
            plugin->setReportVal("density", d.toDouble());
            break;
    }

}
