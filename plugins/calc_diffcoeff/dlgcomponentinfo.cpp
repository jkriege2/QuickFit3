#include "dlgcomponentinfo.h"
#include "ui_dlgcomponentinfo.h"
#include "qfhtmldelegate.h"
#include "datatable2.h"
#include "qfediffusioncoefficientcalculator.h"
#include  <QTextDocument>
#include "qfversion.h"

DlgComponentInfo::DlgComponentInfo(QFEDiffusionCoefficientCalculator *plugin, int component, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgComponentInfo)
{
    this->plugin=plugin;
    ui->setupUi(this);
    model=new QFTableModel(this);
    ui->tableView->setModel(model);
    ui->tableView->setItemDelegate(new QFHTMLDelegate(ui->tableView));
    ui->comboBox->clear();
    ui->comboBox->addItems(plugin->getComponentNames());
    setComponent(component);
}

DlgComponentInfo::~DlgComponentInfo()
{
    delete ui;
}

void DlgComponentInfo::setComponent(int component) {
    if (ui->comboBox->currentIndex()!=component) ui->comboBox->setCurrentIndex(component);
    model->setReadonly(false);
    model->clear();
    model->appendColumn();
    model->appendColumn();
    model->setColumnTitle(0, tr("property"));
    model->setColumnTitle(1, tr("value"));

    int row=0;
    model->setCellCreate(row,0,tr("<b>name:</b>"));
    model->setCellCreate(row,1,QString("<b>%1</b>").arg(Qt::escape(plugin->getComponentName(component))));
    row++;
    model->setCellCreate(row,0,tr("<b>molar mass:</b>"));
    model->setCellCreate(row,1,tr("%1 g/mol").arg(plugin->getComponentMolarMass(component)));
    row++;
    QString comment=plugin->getComponentComment(component, true);
    QString c1=Qt::escape(plugin->getComponentComment(component, false));
    if (!comment.isEmpty() && !c1.isEmpty()) comment=comment+"<br><br>"+c1;

    if (!comment.isEmpty()) {
        model->setCellCreate(row,0,tr("<b>comment:</b>"));
        model->setCellCreate(row,1,comment);
        row++;
    }
    model->setCellCreate(row,0,tr("<b>model function:</b>"));
    model->setCellCreate(row,1,tr("%1").arg(plugin->getViscosityModelFunction(component, true)));
    row++;
    model->setCellCreate(row,0,tr("<b>model valid:</b>"));
    model->setCellCreate(row,1,tr("c = 0 ... %1 M").arg(plugin->getComponentCMax(component)));
    row++;
    model->setCellCreate(row,0,tr("<b>reference:</b>"));
    model->setCellCreate(row,1,Qt::escape(plugin->getComponentReference(component)));
    row++;
    model->setCellCreate(row,0,tr("<b>data file:</b>"));
    model->setCellCreate(row,1,tr("<i>%1</i>").arg(Qt::escape(plugin->getComponentDatafile(component))));
    row++;

    model->setReadonly(true);
    setWindowTitle(tr("Solute Info: %1").arg(plugin->getComponentName(component)));

    ui->tableView->resizeColumnsToContents();

    on_cmbPlot_currentIndexChanged(ui->cmbPlot->currentIndex());

}


void DlgComponentInfo::showHelp() {
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelp(plugin->getID()));
}

void DlgComponentInfo::on_cmbPlot_currentIndexChanged(int index)
{
    ui->plotter->set_doDrawing(false);
    ui->plotter->getDatastore()->clear();
    ui->plotter->clearGraphs(true);
    ui->plotter->get_plotter()->set_keyPosition(JKQTPkeyInsideLeft);

    double cMin=0;
    double cMax=plugin->getComponentCMax(ui->comboBox->currentIndex());
    QString filename=plugin->getComponentDatafile(ui->comboBox->currentIndex());
    if (QFile::exists(filename)) {
        datatable2 tab;
        QVector<double> dc, deta, drho, dn;
        tab.load_csv(filename.toStdString(), ',', '#');
        if (tab.get_line_count()>0 && tab.get_column_count()>=4) {
            for (int r=0; r<tab.get_line_count(); r++) {
                double c=tab.get(0,r);
                dc.append(c*1000.0);
                deta.append(tab.get(3,r));
                drho.append(tab.get(1,r));
                dn.append(tab.get(2,r));
                if (c<cMin) cMin=c;
                if (c>cMax) cMax=c;
            }
            int c_c=ui->plotter->getDatastore()->addCopiedColumn(dc.data(), dc.size(), tr("c_datafile [nM]"));
            int c_eta=ui->plotter->getDatastore()->addCopiedColumn(deta.data(), deta.size(), tr("viscosity_datafile [mPa*s]"));
            int c_rho=ui->plotter->getDatastore()->addCopiedColumn(drho.data(), drho.size(), tr("density [g/cm^3]"));
            int c_n=ui->plotter->getDatastore()->addCopiedColumn(dn.data(), dn.size(), tr("refractive index "));

            if (index==0) {
                ui->plotter->get_plotter()->addGraph(c_c, c_eta, tr("reference data"), JKQTPpoints, QColor("blue"), JKQTPcross);
                ui->plotter->getYAxis()->set_axisLabel(tr("viscosity $\\eta_{20}$ @ 20°C [mPa\\cdot s]"));
            } else if (index==1) {
                ui->plotter->get_plotter()->addGraph(c_c, c_rho, tr("reference data"), JKQTPpoints, QColor("blue"), JKQTPcross);
                ui->plotter->getYAxis()->set_axisLabel(tr("density $\\rho_{20}$ @ 20°C [g/cm^3]"));
            } else if (index==2) {
                ui->plotter->get_plotter()->addGraph(c_c, c_n, tr("reference data"), JKQTPpoints, QColor("blue"), JKQTPcross);
                ui->plotter->getYAxis()->set_axisLabel(tr("refractive index $n_{20}$ @ 20°C"));
            }
        }
    }

    if (index==0) {
        QVector<double> dc, deta;
        for (double c=cMin; c<cMax; c=c+(cMax-cMin)/2000.0) {
            dc.append(c*1000.0);
            deta.append(plugin->evaluateComponentViscosity20degC(ui->comboBox->currentIndex(), c)*1000.0);
        }
        int c_c=ui->plotter->getDatastore()->addCopiedColumn(dc.data(), dc.size(), tr("c [nM]"));
        int c_eta=ui->plotter->getDatastore()->addCopiedColumn(deta.data(), deta.size(), tr("viscosity [mPa*s]"));

        ui->plotter->get_plotter()->addGraph(c_c, c_eta, tr("fit model"), JKQTPlines, QColor("red"), JKQTPnoSymbol);
        ui->plotter->getXAxis()->set_axisLabel(tr("solute concentration $c$ [nM]"));
    }
    ui->plotter->set_doDrawing(true);
    ui->plotter->zoomToFit();
}

void DlgComponentInfo::on_btnMailData_clicked() {
    int component=ui->comboBox->currentIndex();
    QString datafilename=QFileInfo(plugin->getComponentDatafile(component)).fileName();
    QFile f(plugin->getComponentDatafile(component));
    QString filecontents;
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        filecontents=f.readAll();
        f.close();
    }

    QString inisection=QString("[component%1]\nname=%2\nmolar_mass=%3\nreference=\"%4\"\ndatafile=%5\ncomment=\"%6\"\ncomment_html=\"%7\"\nmodel=%8\nmodel_expression=\"%9\"\n").arg(component).arg(plugin->getComponentName(component)).arg(plugin->getComponentMolarMass(component)).arg(plugin->getComponentReference(component)).arg(datafilename).arg(plugin->getComponentComment(component, false)).arg(plugin->getComponentComment(component, true)).arg(plugin->getComponentModelID(component)).arg(plugin->getViscosityModelFunction(component, false));
    QVector<double> params=plugin->getComponentModelParamaters(component);
    for (int i=0; i<params.size(); i++) {
        inisection+=QString("p%1=%2\n").arg(i+1).arg(params[i]);
    }
    QString mailcontents=tr("Dear authors,\nfind attatched my component data for inclusion in the next QuickFit release,\n\nBest,\n\n\nDATA:\n---------------------------------------------------------------------\n%1\n---------------------------------------------------------------------\n%3:\n%2\n---------------------------------------------------------------------\n").arg(inisection).arg(filecontents).arg(datafilename);

    QUrl url=QUrl(QByteArray("mailto:")+qfInfoEmail().toLocal8Bit()+"?subject="+QUrl::toPercentEncoding("QuickFit3/calc_diffcoeff: new component data")+
                  "&body="+QUrl::toPercentEncoding(mailcontents));
    QDesktopServices::openUrl(url);
}
