#include "qfplotterprivate.h"
#include "qfplotter.h"
#include "qfselectrdrdialog.h"
#include "qfrdrtableinterface.h"
#include "qfpluginservices.h"
#include <QDebug>

class MyQFSelectRDRDialogMatchFunctor: public QFMatchRDRFunctor {
    public:
        explicit MyQFSelectRDRDialogMatchFunctor(): QFMatchRDRFunctor() {
            //qDebug()<<"MyQFSelectRDRDialogMatchFunctor:constructor ";
        }

        virtual bool matches(const QFRawDataRecord* record) const {
            QFRDRTableInterface* tab=qobject_cast<QFRDRTableInterface*>(record);
            if (tab) return !tab->tableIsReadonly();
            return false;
        }
};

QFPlotterPrivate::QFPlotterPrivate(QFPlotter *plotter, QObject *parent) :
    QObject(parent)
{
    this->plotter=plotter;
    actCopyToTable=new QAction(QIcon(":/libqf3widgets/qfplotter_copytotable.png"), tr("copy plot data to table RDR"), this);
    connect(actCopyToTable, SIGNAL(triggered()), this, SLOT(copyToTable()));
}


void QFPlotterPrivate::copyToTable()
{
    if (!QFPluginServices::getInstance()->getCurrentProject()) return;

    QFSelectRDRDialog* dlg=new QFSelectRDRDialog(new MyQFSelectRDRDialogMatchFunctor(), true, plotter);
    dlg->setDescription(tr("Select a table record into which to write the plot data."));
    if (dlg->exec()) {

    }
    delete dlg;
}
