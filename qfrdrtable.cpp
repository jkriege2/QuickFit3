#include "qfrdrtable.h"
#include "dlgcsvparameters.h"


QFRawDataRecord* createQFRDRTable(QFProject* parent) {
    return new QFRDRTable(parent);
}


QList<QPair<QAction*, QFRawDataRecordFactory::QFRawDataRecordUserCreateFunc> > registerQFRDRTableMenu(QWidget* parent, QMenu* menu) {
    QList<QPair<QAction*, QFRawDataRecordFactory::QFRawDataRecordUserCreateFunc> > lst;

    QAction* insertRDTableFileAct = new QAction(QIcon(":/table_open.png"), parent->tr("Table from File"), parent);
    insertRDTableFileAct->setStatusTip(parent->tr("Insert a new table raw data item from a specified file"));

    QAction* insertRDTableAct = new QAction(QIcon(":/table_insert.png"), parent->tr("Editable Table"), parent);
    insertRDTableAct->setStatusTip(parent->tr("Insert a new table raw data item, which may be edited in QuickFit 3.0 and is not connected to a file."));

    QMenu* m=new QMenu(parent->tr("&Table"), menu);
    m->setIcon(QIcon(":/projecttree_tablefile.png"));
    m->addAction(insertRDTableAct);
    m->addAction(insertRDTableFileAct);
    menu->addMenu(m);

    lst.append(qMakePair(insertRDTableFileAct, &insertRDTableFile));
    lst.append(qMakePair(insertRDTableAct, &insertRDTable));
    return lst;
}


void insertRDTable(QFProject* project, ProgramOptions* options, QWidget* parent, QtLogFile* logWidget, QStatusBar* statusbar, QProgressBar* progressBar) {
    if (project) {
        project->addRawData("table", parent->tr("NEW_TABLE"));
    }
}


void insertRDTableFile(QFProject* project, ProgramOptions* options, QWidget* parent, QtLogFile* logWidget, QStatusBar* statusbar, QProgressBar* progressBar) {
    if (project) {

        dlgCSVParameters* csvDlg=new dlgCSVParameters(parent, options->getQSettings()->value("fcs/column_separator", ",").toString(), options->getQSettings()->value("fcs/decimal_separator", ".").toString(), options->getQSettings()->value("fcs/comment_start", "#").toString(), options->getQSettings()->value("fcs/header_start", "#!").toString());
        if (csvDlg->exec()==QDialog::Accepted) {
            QMap<QString, QVariant> p;
            p["column_separator"]=QString(csvDlg->column_separator);
            p["decimal_separator"]=QString(csvDlg->decimal_separator);
            p["comment_start"]=QString(csvDlg->comment_start);
            p["header_start"]=QString(csvDlg->header_start);
            options->getQSettings()->setValue("fcs/column_separator", QString(csvDlg->column_separator));
            options->getQSettings()->setValue("fcs/decimal_separator", QString(csvDlg->decimal_separator));
            options->getQSettings()->setValue("fcs/comment_start", QString(csvDlg->comment_start));
            options->getQSettings()->setValue("fcs/header_start", QString(csvDlg->header_start));

            QStringList roParams;
            roParams<<"column_separator"<<"decimal_separator"<<"comment_start"<<"header_start";

            QStringList files = QFileDialog::getOpenFileNames(parent, parent->tr("Select Table File(s) to Import ..."), options->getCurrentRawDataDir(), parent->tr("Comma Separated Value Files (*.txt *.dat *.csv);;All Files (*.*)"));
            QStringList list = files;
            QStringList::Iterator it = list.begin();
            progressBar->setRange(0, list.size());
            int i=0;
            while(it != list.end()) {
                i++;
                logWidget->log_text(parent->tr("importing CSV file '%1' using (column_separator='%2', decimal_separator='%3', comment_start='%4', header_start='%5')\n").arg(*it).arg(QString(csvDlg->column_separator)).arg(QString(csvDlg->decimal_separator)).arg(QString(csvDlg->comment_start)).arg(QString(csvDlg->header_start)));
                QFRawDataRecord* e=project->addRawData("table", QFileInfo(*it).fileName(), QStringList(*it), p, roParams);
                if (e->error()) {
                    project->deleteRawData(e->getID());
                    QMessageBox::critical(parent, parent->tr("QuickFit 3.0"), parent->tr("Error while importing '%1':\n%2").arg(*it).arg(e->errorDescription()));
                    logWidget->log_error(parent->tr("Error while importing '%1':\n    %2\n").arg(*it).arg(e->errorDescription()));
                }
                options->setCurrentRawDataDir(QFileInfo(*it).dir().absolutePath());
                ++it;
                progressBar->setValue(i);
                QApplication::processEvents();
            }
            progressBar->reset();
        }
        delete csvDlg;
    }
}

QFRDRTable::QFRDRTable(QFProject* parent/*, QString name, QString inputFile*/):
    datamodel(NULL), QFRawDataRecord(parent)//, name, QStringList(inputFile))
{
    datamodel=new QFTableModel(this);
    connect(datamodel, SIGNAL(modelReset()), this, SLOT(trawDataChanged()));
    connect(datamodel, SIGNAL(dataChanged( const QModelIndex & , const QModelIndex &  )), this, SLOT(tdataChanged( const QModelIndex & , const QModelIndex &  )));
}

/*QFRDRTable::QFRDRTable(QDomElement& e, QFProject* parent):
    datamodel(NULL), QFRawDataRecord(e, parent)
{
    std::cout<<"created QFRDRTable\n";
}*/

QFRDRTable::~QFRDRTable()
{
    //dtor
}


void QFRDRTable::exportData(const QString& format, const QString& filename)const  {
    if (!datamodel) return;
    QString f=format.toUpper();
    if (f=="CSV") {
        datamodel->saveCSV(filename);
    } else if (f=="SLK" || f=="SYLK") {
        datamodel->saveSYLK(filename);
    }
}


void QFRDRTable::intWriteData(QXmlStreamWriter& w) {
    if (!datamodel) return;
    if (files.size()>0) {
        if (datamodel->hasChanged()) datamodel->saveCSV(files[0]);
    } else {
        for (quint16 c=0; c<datamodel->columnCount(); c++) {
            w.writeStartElement("column");
            w.writeAttribute("title", datamodel->columnTitle(c));
            for (quint16 r=0; r<datamodel->rowCount(); r++) {
                w.writeStartElement("row");
                w.writeAttribute("type", QString(datamodel->cell(r, c).typeName()));
                w.writeAttribute("value", datamodel->cell(r, c).toString());
                w.writeEndElement();
            }
            w.writeEndElement();
        }
    }
}

void QFRDRTable::intReadData(QDomElement* e) {
    if (!datamodel) {
        datamodel=new QFTableModel(this);
        connect(datamodel, SIGNAL(modelReset()), this, SLOT(rawDataChanged()));
        connect(datamodel, SIGNAL(dataChanged( const QModelIndex & , const QModelIndex &  )), this, SLOT(tdataChanged( const QModelIndex & , const QModelIndex &  )));
    }
    datamodel->clear();
    datamodel->setReadonly(false);
    if (files.size()>0) {
        std::cout<<"    reading CSV\n";
        QString s=getProperty("column_separator", ",").toString();
        char column_separator=(s.size()>0)?s[0].toAscii():',';
        s=getProperty("decimal_separator", ".").toString();
        char decimal_separator=(s.size()>0)?s[0].toAscii():'.';
        s=getProperty("header_start", "#!").toString();
        QString header_start=s;
        s=getProperty("coment_start", "#").toString();
        char comment_start=(s.size()>0)?s[0].toAscii():'#';

        datamodel->readCSV(files[0], column_separator, decimal_separator, header_start, comment_start);
        datamodel->setReadonly(true);
        datamodel->resetChanged();
    } else if (e) {
        QDomElement te=e->firstChildElement("column");
        quint16 rows=0, columns=0;
        datamodel->setReadonly(false);
        if (!te.isNull()) {
            while (!te.isNull()) {
                columns++;
                QString n=te.attribute("title");
                QDomElement re=te.firstChildElement("row");
                quint16 r=0;
                //std::cout<<"resize("<<rows<<", "<<columns<<")\n";
                datamodel->resize(rows, columns);
                //std::cout<<"resize("<<rows<<", "<<columns<<") DONE\n";
                while (!re.isNull()) {
                    QString t=re.attribute("type").toLower();
                    QVariant v=re.attribute("value");
                    if (r+1>rows) {
                        rows=r+1;
                        //std::cout<<"resize("<<rows<<", "<<columns<<")\n";
                        datamodel->resize(rows, columns);
                        //std::cout<<"resize("<<rows<<", "<<columns<<") DONE\n";
                    }
                    if (t=="bool") { v.convert(QVariant::Bool); }
                    else if (t=="char") { v.convert(QVariant::Char); }
                    else if (t=="date") { v.convert(QVariant::Date); }
                    else if (t=="datetime") { v.convert(QVariant::DateTime); }
                    else if (t=="double") { v.convert(QVariant::Double); }
                    else if (t=="int") { v.convert(QVariant::Int); }
                    else if (t=="longlong") { v.convert(QVariant::LongLong); }
                    else if (t=="string") { v.convert(QVariant::String); }
                    else if (t=="stringlist") { v.convert(QVariant::StringList); }
                    else if (t=="uint") { v.convert(QVariant::UInt); }
                    else if (t=="ulonglong") { v.convert(QVariant::ULongLong); }
                    else if (t=="time") { v.convert(QVariant::Time); }
                    //std::cout<<"setCell("<<r<<", "<<columns-1<<", '"<<v.toString().toStdString()<<"' ["<<v.typeName()<<"])\n";
                    datamodel->setCell(r, columns-1, v);


                    re = re.nextSiblingElement("row");
                    r++;
                }
                datamodel->setColumnTitle(columns-1, n);
                te = te.nextSiblingElement("column");
            }
        } else {
            datamodel->resize(1,1);
        }
        datamodel->resetChanged();
    }
}


