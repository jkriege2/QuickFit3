/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfrdrpropertymodel.h"
#include "qftools.h"
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
    if (doReset) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
    }
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

int QFRDRPropertyModel::rowCount(const QModelIndex &/*parent*/ ) const {
    if (!record) return 0;
    //std::cout<<"getVisiblePropertyCount() = "<<record->getVisiblePropertyCount()<<"\n";
    return props.size();
}

int QFRDRPropertyModel::columnCount(const QModelIndex &/*parent*/) const {
    if (!record) return 0;
    return 3;
}

Qt::ItemFlags QFRDRPropertyModel::flags(const QModelIndex &index) const {
    if (!record || !index.isValid()) return 0;
    QString p=props.value(index.row(), "");
    if (!p.isEmpty() && record->isPropertyUserEditable(p) /*&& index.column()==2*/) return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool QFRDRPropertyModel::setData(const QModelIndex &idx, const QVariant &value, int role) {
    if (!record || !idx.isValid()) return false;
    if (idx.isValid()) {
        if (idx.column()==2 && role == Qt::EditRole) {
            QString p=props.value(idx.row(), "");
            if (!p.isEmpty()) {
                if (!record->isPropertyUserEditable(p)) return false;
                record->setQFProperty(p, value);
                emit dataChanged(idx, idx);
                return true;
            }
        } else if (idx.column()==0 && role == Qt::EditRole) {
            QString newname=value.toString();
            QString oldname=props.value(idx.row(), "");
            if (!oldname.isEmpty()) {
                QVariant val=record->getProperty(oldname);
                if (newname!=oldname) {
                    bool ok=true;
                    if (record->propertyExists(newname)) {
                        ok=(QMessageBox::question(NULL, tr("QuickFit"),
                                                        tr("A property with the new name '%1' already exists\n  - old value is '%2'\n  - old value would be '%3'\n Do you want to overwrite the old property (old data will be lost)?").arg(newname).arg(record->getProperty(newname).toString()).arg(record->getProperty(oldname).toString()),
                                                        QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes);
                    }
                    if (ok) {
                        record->setQFProperty(newname, val);
                        record->deleteProperty(oldname);
                    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                        beginResetModel();
                        endResetModel();
                    #else
                        reset();
                    #endif
                    }
                }
            }
        } else if (idx.column()==1 && role == Qt::EditRole) {
            bool isInt=false;
            int type=value.toInt(&isInt);
            QString typeS=value.toString().toLower().trimmed();
            QString p=props.value(idx.row(), "");

            if (!p.isEmpty()) {
                if (!record->isPropertyUserEditable(p)) return false;

                QVariant currentData=record->getProperty(p, QVariant());
                bool ok=false;
                bool askUser=false;
                QVariant::Type newtype=QVariant::String;
                if (isInt) {
                    if (type==0) { ok=true; newtype=QVariant::String; }
                    else if (type==1) { ok=true; newtype=QVariant::Double; }
                    else if (type==2) { ok=true; newtype=QVariant::LongLong; }
                    else if (type==3) { ok=true; newtype=QVariant::Bool; }
                    else if (type==4) { ok=true; newtype=QVariant::DateTime; }
                } else {
                    if (typeS==tr("string")) { ok=true; newtype=QVariant::String; }
                    else if (typeS==tr("number")||typeS==tr("double")||typeS==tr("float")) { ok=true; newtype=QVariant::Double; }
                    else if (typeS==tr("integer")||typeS==tr("long")||typeS==tr("int")||typeS==tr("longlong")||typeS==tr("qlonglong")) { ok=true; newtype=QVariant::LongLong; }
                    else if (typeS==tr("boolean")||typeS==tr("bool")) { ok=true; newtype=QVariant::Bool; }
                    else if (typeS==tr("date/time")||typeS==tr("date")||typeS==tr("time")) { ok=true; newtype=QVariant::DateTime; }
                    else {
                        newtype=QVariant::nameToType(value.toString().toLocal8Bit().data());
                        ok=true;
                    }
                }

                if (ok) {
                    QVariant newdata=currentData;
                    bool conv=newdata.convert(newtype);
                    if (currentData.type()==QVariant::Double && (newtype==QVariant::LongLong || newtype==QVariant::LongLong || newtype==QVariant::Int)) {
                        askUser=currentData.toLongLong()!=currentData.toDouble();
                    }

                    if (!conv || askUser) {
                        conv=(QMessageBox::question(NULL, tr("QuickFit"),
                                                          tr("The selected new type might lead to data loss:\n  - old value is '%1'\n  - old value would be '%2'\n Do you still want to change the data type?").arg(currentData.toString()).arg(newdata.toString()),
                                                          QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes);
                    }

                    if (conv) {
                        record->setQFProperty(p, newdata);
                        emit dataChanged(index(idx.row(),0), index(idx.row(),columnCount()-1));
                        return true;
                    }
                }
            }
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
            if (index.column()==0) {
                return p;
            } else if (index.column()==1) {
                switch (record->getProperty(p).type()) {
                    case QVariant::LongLong:
                    case QVariant::Int:
                    case QVariant::ULongLong:
                    case QVariant::UInt:
                        return tr("integer");
                    case QVariant::Double:
                        return tr("number");
                    case QVariant::Bool:
                        return tr("boolean");
                    case QVariant::String:
                        return tr("string");
                    case QVariant::Date:
                    case QVariant::Time:
                    case QVariant::DateTime:
                        return tr("date/time");
                    default:
                        return record->getProperty(p).typeName();
                }
            } else if (index.column()==2) {
                return record->getProperty(p);
            }
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



