#include "qfplotter.h"
#include "qfplotterprivate.h"
#include "programoptions.h"
#include "datatools.h"


class DataToolsJKQTPSaveDataAdapter: public JKQtBasePlotter::JKQTPSaveDataAdapter {
    public:
        DataToolsJKQTPSaveDataAdapter(int format) {
            this->format=format;
        }

        virtual QString getFilter() const {
            return QFDataExportHandler::getFormats().value(format);
        }
        virtual void saveJKQTPData(const QString& filename, const QList<QVector<double> >& data, const QStringList& columnNames) const {
            QFDataExportHandler::save(data, format, filename, columnNames);
        }
    protected:
        int format;
};

QFPlotter::QFPlotter(QWidget *parent) :
    JKQtPlotter(parent)
{
    initQFPlotter();
}

QToolBar *QFPlotter::createToolbar(QWidget *parent, const QString &name)
{
    QString n=name;
    if (n.isEmpty()) n=objectName()+"_toolbar";
    QToolBar* tool=new QToolBar(n, parent);
    populateToolbar(tool);
    return tool;
}

QFPlotter::QFPlotter(bool datastore_internal, QWidget *parent, JKQTPdatastore *datast):
    JKQtPlotter(datastore_internal, parent, datast)
{
    initQFPlotter();
}

void QFPlotter::initQFPlotter()
{
    p=new QFPlotterPrivate(this, this);
    get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    //toolbar->insertAction(plotter->get_actCopyMatlab(), p->actCopyToTable);
    toolbar->clear();
    toolbar->addSeparator();
    toolbar->addSeparator();
    populateToolbar(toolbar);

    QStringList formats=QFDataExportHandler::getFormats();
    for (int i=0; i<formats.size(); i++) {
        DataToolsJKQTPSaveDataAdapter* a=new DataToolsJKQTPSaveDataAdapter(i);
        if (!JKQtBasePlotter::registerSaveDataAdapter(a)) {
            delete a;
        }
    }
}

void QFPlotter::modifyContextMenu(QMenu *menu)
{
    menu->insertAction(plotter->get_actCopyMatlab(), p->actCopyToTable);
    menu->addSeparator();
    menu->addAction(p->actHelp);
}

void QFPlotter::populateToolbar(QToolBar *toolbar) const
{
    JKQtPlotter::populateToolbar(toolbar);
    toolbar->insertAction(plotter->get_actCopyMatlab(), p->actCopyToTable);
    toolbar->addSeparator();
    toolbar->addAction(p->actHelp);
}

QAction *QFPlotter::get_actCopyToTable() const
{
    return p->actCopyToTable;
}

QAction *QFPlotter::get_actHelp() const
{
    return p->actHelp;
}
