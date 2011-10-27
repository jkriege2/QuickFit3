#include "qfrdrpropertymodel.h"
#include "qfrawdatarecord.h"


QFRDRPropertyModel::QFRDRPropertyModel():
    QAbstractTableModel()
{
    record=NULL;
}

QFRDRPropertyModel::~QFRDRPropertyModel()
{
    //dtor
}


void QFRDRPropertyModel::propertiesChanged() {
    //qDebug()<<"QFRDRPropertyModel::propertiesChanged()";
    reset();
    //qDebug()<<"QFRDRPropertyModel::propertiesChanged() ... done";
}

void QFRDRPropertyModel::init(QFRawDataRecord* record) {
    setParent(record);
    this->record=record;
    disconnect();
    connect(record, SIGNAL(propertiesChanged()), this, SLOT(propertiesChanged()));
}

int QFRDRPropertyModel::rowCount(const QModelIndex &parent ) const {
    if (!record) return 0;
    //std::cout<<"getVisiblePropertyCount() = "<<record->getVisiblePropertyCount()<<"\n";
    return record->getVisiblePropertyCount();
}

int QFRDRPropertyModel::columnCount(const QModelIndex &parent) const {
    if (!record) return 0;
    return 2;
}

Qt::ItemFlags QFRDRPropertyModel::flags(const QModelIndex &index) const {
    if (!record || !index.isValid()) return 0;
    QString p=record->getVisibleProperty(index.row());
    if (record->isPropertyUserEditable(p) && index.column()==1) return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool QFRDRPropertyModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!record || !index.isValid()) return false;
    if (index.isValid() && index.column()==1 && role == Qt::EditRole) {
        QString p=record->getVisibleProperty(index.row());
        if (!record->isPropertyUserEditable(p)) return false;
        record->setQFProperty(p, value);
        return true;
    }
    return false;
}

QVariant QFRDRPropertyModel::data(const QModelIndex &index, int role) const {
    if (!record || !index.isValid()) return QVariant();

    if (role==Qt::BackgroundRole) {
        QPalette pal;
        if (index.column()==0) {
            if (index.row()%2) return pal.color(QPalette::Window).darker(130);
            return pal.color(QPalette::Window);
        } else {
            QString p=record->getVisibleProperty(index.row());
            if (record->isPropertyUserEditable(p)) {
                if (index.row()%2) return pal.color(QPalette::AlternateBase);
                return pal.color(QPalette::Base);
            } else {
                if (index.row()%2) return pal.color(QPalette::Window).darker(130);
                return pal.color(QPalette::Window);
            }
        }
    } else if (role==Qt::DisplayRole || role==Qt::EditRole) {
        QString p=record->getVisibleProperty(index.row());
        if (index.column()==0) return p;
        if (index.column()==1) return record->getProperty(p);
    }
    return QVariant();
}

QVariant QFRDRPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (!record) return QVariant();
    if (orientation==Qt::Horizontal) {
        if (role==Qt::DisplayRole) {
            if (section==0) return QVariant(tr("Name"));
            if (section==1) return QVariant(tr("Value"));
        }
    }
    return QVariant();
}



