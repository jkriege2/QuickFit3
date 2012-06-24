#include "qfrdrpropertymodel.h"
#include "qfrawdatarecord.h"


QFRDRPropertyModel::QFRDRPropertyModel():
    QAbstractTableModel()
{
    record=NULL;
    props.clear();
}

QFRDRPropertyModel::~QFRDRPropertyModel()
{
    //dtor
}


void QFRDRPropertyModel::propertiesChanged(const QString &property, bool visible) {
    //qDebug()<<"QFRDRPropertyModel::propertiesChanged()   porperty="<<property<<"   visible="<<visible;
    if (visible) {
        if (property.isEmpty()){
            //qDebug()<<"QFRDRPropertyModel::propertiesChanged: updateModel()";
            updateModel();
        } else {
            int row=props.indexOf(property);
            if (row<0) {
                //updateModel();
                int i=0;
                while (i<props.size() && property>props[i]) {
                    i++;
                }
                //qDebug()<<"QFRDRPropertyModel::propertiesChanged: insert i="<<i<<"   props.size()="<<props.size();
                beginInsertRows(QModelIndex(), i, i);
                props.insert(i, property);
                endInsertRows();
                //emit dataChanged(index(i, 0), index(i, columnCount()-1));
            } else {
                //qDebug()<<"QFRDRPropertyModel::propertiesChanged: data changed row="<<row;
                emit dataChanged(index(row, 0), index(row, columnCount()-1));
            }
        }
    }
    //qDebug()<<"QFRDRPropertyModel::propertiesChanged() ... done";
}

bool QFRDRPropertyModel_caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
 return s1.toLower() < s2.toLower();
}

void QFRDRPropertyModel::updateModel(bool doReset)
{
    props=record->getVisibleProperties();
    qSort(props.begin(), props.end(), QFRDRPropertyModel_caseInsensitiveLessThan);
    if (doReset) reset();
}

void QFRDRPropertyModel::init(QFRawDataRecord* record) {
    //if (record) qDebug()<<"QFRDRPropertyModel::init("<<record->getName()<<")";
    setParent(record);
    this->record=record;
    disconnect();
    props.clear();
    if (record) updateModel();
    connect(record, SIGNAL(propertiesChanged(QString,bool)), this, SLOT(propertiesChanged(QString,bool)));
}

int QFRDRPropertyModel::rowCount(const QModelIndex &parent ) const {
    if (!record) return 0;
    //std::cout<<"getVisiblePropertyCount() = "<<record->getVisiblePropertyCount()<<"\n";
    return props.size();
}

int QFRDRPropertyModel::columnCount(const QModelIndex &parent) const {
    if (!record) return 0;
    return 3;
}

Qt::ItemFlags QFRDRPropertyModel::flags(const QModelIndex &index) const {
    if (!record || !index.isValid()) return 0;
    QString p=props.value(index.row(), "");
    if (!p.isEmpty() && record->isPropertyUserEditable(p) && index.column()==2) return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool QFRDRPropertyModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!record || !index.isValid()) return false;
    if (index.isValid() && index.column()==2 && role == Qt::EditRole) {
        QString p=props.value(index.row(), "");
        if (!p.isEmpty()) {
            if (!record->isPropertyUserEditable(p)) return false;
            record->setQFProperty(p, value);
            emit dataChanged(index, index);
            return true;
        }
    }
    return false;
}

QString QFRDRPropertyModel::getPropertyForRow(int row) const
{
    return props.value(row, "");
}


QVariant QFRDRPropertyModel::data(const QModelIndex &index, int role) const {
    if (!record || !index.isValid()) return QVariant();

    if (role==Qt::BackgroundRole) {
        QPalette pal;
        if (index.column()==0) {
            //if (index.row()%2) return pal.color(QPalette::Window).darker(130);
            return pal.color(QPalette::Window);
        } else {
            QString p=props.value(index.row(), "");
            if (!p.isEmpty()) {
                if (record->isPropertyUserEditable(p)) {
                    //if (index.row()%2) return pal.color(QPalette::AlternateBase);
                    return pal.color(QPalette::Base);
                } else {
                    //if (index.row()%2) return pal.color(QPalette::Window).darker(130);
                    //return pal.color(QPalette::Window);
                    return pal.color(QPalette::Window).darker(130);
                }
            }
        }
    } else if (role==Qt::DisplayRole || role==Qt::EditRole) {
        QString p=props.value(index.row(), "");
        if (!p.isEmpty()) {
            if (index.column()==0) return p;
            if (index.column()==1) return record->getProperty(p).typeName();
            if (index.column()==2) return record->getProperty(p);
        }
    }
    return QVariant();
}

QVariant QFRDRPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (!record) return QVariant();
    if (orientation==Qt::Horizontal) {
        if (role==Qt::DisplayRole) {
            if (section==0) return QVariant(tr("Name"));
            if (section==1) return QVariant(tr("Type"));
            if (section==2) return QVariant(tr("Value"));
        }
    }
    return QVariant();
}



