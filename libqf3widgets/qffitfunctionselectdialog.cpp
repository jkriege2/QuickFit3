#include "qffitfunctionselectdialog.h"
#include "ui_qffitfunctionselectdialog.h"
#include "qffitfunctionmanager.h"
#include "qfpluginservices.h"
#include "qffitfunction.h"
#include "libwid_imexport.h"
#include "qfhtmlhelptools.h"

QFFitFunctionSelectDialog::QFFitFunctionSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFFitFunctionSelectDialog)
{
    model=NULL;
    ui->setupUi(this);
    filterModel.setDynamicSortFilter(true);
    filterModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->lstModels->setModel(&filterModel);
    connect(ui->lstModels->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(currentRowChanged(QModelIndex,QModelIndex)));
}

QFFitFunctionSelectDialog::~QFFitFunctionSelectDialog()
{
    delete ui;
}

QString QFFitFunctionSelectDialog::getSelected() const
{
    QString id="";
    if (ui->lstModels->currentIndex().row()>=0) {
        id=ui->lstModels->currentIndex().data(Qt::UserRole+1).toString();
    }
    return id;
}

void QFFitFunctionSelectDialog::init(const QString &filter, const QString &current)
{
    QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
    bool upd=updatesEnabled();
    int idx=ui->lstModels->currentIndex().row();
    if (idx<0) idx=0;
    setUpdatesEnabled(false);

    filterModel.setSourceModel(NULL);
    if (model) delete model;
    model=new QStandardItemModel(this);
    QMap<QString, QFFitFunction*> m_fitFunctions=manager->getModels(filter, this);
    QMapIterator<QString, QFFitFunction*> it(m_fitFunctions);
    int i=0;

    while (it.hasNext())  {
        it.next();
        if (it.value()) {
            QStandardItem* item=new QStandardItem(QIcon(":/lib/fitfunc_icon.png"), it.value()->shortName());
            item->setData(it.key(), Qt::UserRole+1);
            model->appendRow(item);
            if (it.key()==current) idx=i;
            i++;
        }
    }
    filterModel.setSourceModel(model);
    ui->lstModels->setModel(&filterModel);
    model->sort(0);
    setUpdatesEnabled(upd);
    ui->lstModels->setCurrentIndex(model->index(idx, 0));
    ui->edtFilter->clear();
}

void QFFitFunctionSelectDialog::currentRowChanged(const QModelIndex & current, const QModelIndex & previous)
{


    QString pid=getSelected();
    int ppid=QFPluginServices::getInstance()->getFitFunctionManager()->getPluginForID(pid);
    QString help=QFPluginServices::getInstance()->getFitFunctionManager()->getPluginHelp(ppid, pid);
    ui->txtHelp->setHtml(transformQF3HelpHTMLFile(help));
}

void QFFitFunctionSelectDialog::on_edtFilter_textChanged(const QString &text)
{
    filterModel.setFilterRegExp(QRegExp(QString("*")+text+QString("*"), Qt::CaseInsensitive, QRegExp::Wildcard));
}
