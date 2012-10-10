#include "dlgcomponentinfo.h"
#include "ui_dlgcomponentinfo.h"
#include "qfhtmldelegate.h"
#include "datatable2.h"
#include "qfediffusioncoefficientcalculator.h"
#include  <QTextDocument>

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
    model->setCellCreate(row,0,tr("<b>model function:</b>"));
    model->setCellCreate(row,1,tr("%1").arg(plugin->getViscosityModelFunction(component, true)));
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

    ui->plotter->set_doDrawing(false);
    ui->plotter->getDatastore()->clear();
    ui->plotter->clearGraphs(true);
    ui->plotter->get_plotter()->set_keyPosition(JKQTPkeyInsideLeft);

    double cMin=0;
    double cMax=1;
    QString filename=plugin->getComponentDatafile(component);
    if (QFile::exists(filename)) {
        datatable2 tab;
        QVector<double> dc, deta;
        tab.load_csv(filename.toStdString(), ',', '#');
        if (tab.get_line_count()>0 && tab.get_column_count()>=4) {
            for (int r=0; r<tab.get_line_count(); r++) {
                double c=tab.get(0,r);
                dc.append(c*1000.0);
                deta.append(tab.get(3,r));
                if (c<cMin) cMin=c;
                if (c>cMax) cMax=c;
            }
            int c_c=ui->plotter->getDatastore()->addCopiedColumn(dc.data(), dc.size(), tr("c_datafile [nM]"));
            int c_eta=ui->plotter->getDatastore()->addCopiedColumn(deta.data(), deta.size(), tr("viscosity_datafile [mPa*s]"));

            ui->plotter->get_plotter()->addGraph(c_c, c_eta, tr("reference data"), JKQTPpoints, QColor("blue"), JKQTPcross);
        }
    }

    QVector<double> dc, deta;
    for (double c=cMin; c<cMax; c=c+(cMax-cMin)/1000.0) {
        dc.append(c*1000.0);
        deta.append(plugin->evaluateComponentViscosity20degC(component, c)*1000.0);
    }
    int c_c=ui->plotter->getDatastore()->addCopiedColumn(dc.data(), dc.size(), tr("c [nM]"));
    int c_eta=ui->plotter->getDatastore()->addCopiedColumn(deta.data(), deta.size(), tr("viscosity [mPa*s]"));

    ui->plotter->get_plotter()->addGraph(c_c, c_eta, tr("fit model"), JKQTPlines, QColor("red"), JKQTPnoSymbol);
    ui->plotter->getXAxis()->set_axisLabel(tr("solute concentration $c$ [nM]"));
    ui->plotter->getYAxis()->set_axisLabel(tr("viscosity @ 20°C $\\eta_{20}$ [mPa\\cdot s]"));
    ui->plotter->set_doDrawing(true);
    ui->plotter->zoomToFit();
}


void DlgComponentInfo::showHelp()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelp(plugin->getID()));
}
