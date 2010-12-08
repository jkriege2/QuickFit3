#include "qfrdrtable.h"
#include "dlgcsvparameters.h"










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


