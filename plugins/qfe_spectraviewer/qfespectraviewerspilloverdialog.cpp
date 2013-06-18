#include "qfespectraviewerspilloverdialog.h"
#include "ui_qfespectraviewerspilloverdialog.h"

QFESpectraViewerSpilloverDialog::QFESpectraViewerSpilloverDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFESpectraViewerSpilloverDialog)
{
    ui->setupUi(this);
    model.setReadonly(false);
    ui->tableView->setModel(&model);
}

QFESpectraViewerSpilloverDialog::~QFESpectraViewerSpilloverDialog()
{
    delete ui;
}
struct QFESpectraViewerSpilloverDialogBandpass {
    QString name;
    double center;
    double width;
    QFESpectraViewerPlotItemType type;
};

void QFESpectraViewerSpilloverDialog::init(const QList<QFESpectraViewerPlotItem> &plotItems, SpectrumManager *manager)
{
    model.disableSignals();
    model.setReadonly(false);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QList<SpectrumManager::FilterData> filters;
    QList<SpectrumManager::FluorophoreData> fluorophores;
    QList<QFESpectraViewerSpilloverDialogBandpass> filtersSimple;
    for (int i=0; i<plotItems.size(); i++) {
        if (plotItems[i].type==qfesFluorohpore && manager->fluorophoreExists(plotItems[i].name)) {
            fluorophores<<manager->getFluorophoreData(plotItems[i].name);
        } else if (plotItems[i].type==qfesFilterSpectrum && manager->filterExists(plotItems[i].name)) {
            filters<<manager->getFilterData(plotItems[i].name);
        } else if (plotItems[i].type==qfesFilterBandpass||plotItems[i].type==qfesFilterLongpass||plotItems[i].type==qfesFilterShortpass||plotItems[i].type==qfesFilterNotch) {
            QFESpectraViewerSpilloverDialogBandpass f;
            f.name=plotItems[i].displayName;
            f.center=plotItems[i].centralWavelength;
            f.width=plotItems[i].spectralWidth;
            f.type=plotItems[i].type;
            filtersSimple<<f;
        }
    }

    model.clear();

    for (int i=0; i<fluorophores.size(); i++) {
        int c=0;
        model.setCellCreate(i, c, fluorophores[i].name);
        model.setColumnTitleCreate(c, tr("fluorophore"));
        c++;        
        for (int j=0; j<filters.size(); j++) {
            model.setColumnTitleCreate(c, filters[j].name);
            qDebug()<<fluorophores[i].name<<filters[j].name;
            if (manager->spectrumExists(fluorophores[i].spectrum_fl) && manager->spectrumExists(filters[j].spectrum)) {
                model.setCellCreate(i, c, QString::number(manager->getSpectrum(fluorophores[i].spectrum_fl)->getMulSpectrumIntegral(manager->getSpectrum(filters[j].spectrum))/manager->getSpectrum(fluorophores[i].spectrum_fl)->getSpectrumFullIntegral()*100.0)+QString(" %"));
            }
            c++;
        }
        for (int j=0; j<filtersSimple.size(); j++) {
            model.setColumnTitleCreate(c, filtersSimple[j].name);
            qDebug()<<fluorophores[i].name<<filtersSimple[j].name;
            if (manager->spectrumExists(fluorophores[i].spectrum_fl)) {
                if (filtersSimple[j].type==qfesFilterBandpass) {
                    model.setCellCreate(i, c, QString::number(manager->getSpectrum(fluorophores[i].spectrum_fl)->getSpectrumIntegral(filtersSimple[j].center-filtersSimple[j].width/2.0,filtersSimple[j].center+filtersSimple[j].width/2.0)/manager->getSpectrum(fluorophores[i].spectrum_fl)->getSpectrumFullIntegral()*100.0)+QString(" %"));
                } else if (filtersSimple[j].type==qfesFilterNotch) {
                    double full=manager->getSpectrum(fluorophores[i].spectrum_fl)->getSpectrumFullIntegral();
                    model.setCellCreate(i, c, QString::number((full-manager->getSpectrum(fluorophores[i].spectrum_fl)->getSpectrumIntegral(filtersSimple[j].center-filtersSimple[j].width/2.0,filtersSimple[j].center+filtersSimple[j].width/2.0))/full*100.0)+QString(" %"));
                } else if (filtersSimple[j].type==qfesFilterLongpass) {
                    model.setCellCreate(i, c, QString::number(manager->getSpectrum(fluorophores[i].spectrum_fl)->getSpectrumIntegral(filtersSimple[j].center,manager->getSpectrum(fluorophores[i].spectrum_fl)->getWavelengthMax())/manager->getSpectrum(fluorophores[i].spectrum_fl)->getSpectrumFullIntegral()*100.0)+QString(" %"));
                } else if (filtersSimple[j].type==qfesFilterShortpass) {
                    model.setCellCreate(i, c, QString::number(manager->getSpectrum(fluorophores[i].spectrum_fl)->getSpectrumIntegral(manager->getSpectrum(fluorophores[i].spectrum_fl)->getWavelengthMin(),filtersSimple[j].center)/manager->getSpectrum(fluorophores[i].spectrum_fl)->getSpectrumFullIntegral()*100.0)+QString(" %"));
                }
            }
            c++;
        }
    }

    model.setReadonly(true);
    model.enableSignals(true);
    ui->tableView->setModel(&model);
    ui->tableView->resizeColumnsToContents();
    QApplication::restoreOverrideCursor();
}
