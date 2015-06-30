/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-02-13 16:56:45 +0100 (Fr, 13 Feb 2015) $  (revision $Rev: 3827 $)

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

#include "qfgroupandrolemodel.h"
#include "qfrawdatarecord.h"

QFGroupAndRoleModel::QFGroupAndRoleModel(QFProject *project, QObject *parent) :
    QAbstractTableModel(parent)
{
    this->project=project;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
    roleAlwaysEditable=false;
    nameEditable=false;
    connect(project, SIGNAL(structureChanged()), this, SLOT(projectDataChanged()));
    connect(project, SIGNAL(sortOrderChanged(int)), this, SLOT(projectDataChanged()));
}

Qt::ItemFlags QFGroupAndRoleModel::flags(const QModelIndex &index) const
{
    int col=index.column();
    int row=index.row();
    if (project) {
        QFRawDataRecord* rdr=project->getRawDataByNum(row);
        if (rdr) {
            if (col==0) { // name
                if (nameEditable) return Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable;
                else return Qt::ItemIsSelectable|Qt::ItemIsEnabled;

            } else if (col==1) { // group
                return Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable;

            } else if (col==2) { // role
                if (rdr->isRoleUserEditable() || roleAlwaysEditable) return Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable;
                else return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
            } else if (col==3) { // folder
                return Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable;
            }
        }
    }
    return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}

QVariant QFGroupAndRoleModel::data(const QModelIndex &index, int role) const
{
    int col=index.column();
    int row=index.row();
    if (project) {
        QFRawDataRecord* rdr=project->getRawDataByNum(row);
        if (rdr) {
            if (col==0) {
                if (role==Qt::EditRole || role==Qt::DisplayRole) {
                    return rdr->getName();
                }
            } else if (col==1) {
                if (role==Qt::EditRole) {
                    return rdr->getGroup();
                } else if (role==Qt::DisplayRole) {
                    return rdr->getGroupName();
                } else if (role==Qt::BackgroundRole || role==Qt::BackgroundColorRole) {
                    QColor groupBaseColor=QColor("aliceblue");
                    double grpCount=qMax(10,project->getRDRGroupCount());
                    double grp=rdr->getGroup()+1;
                    if (grp>0) {
                        int colFactor=100+grp/grpCount*80.0;
                        //qDebug()<<grp<<grpCount<<"  -> "<<colFactor;
                        return groupBaseColor.darker(colFactor);
                    }
                }
            } else if (col==2) {
                if (role==Qt::EditRole || role==Qt::DisplayRole) {
                    return rdr->getRole();
                }
            } else if (col==3) {
                if (role==Qt::EditRole || role==Qt::DisplayRole) {
                    return rdr->getFolder();
                }
            }
        }
    }
    return QVariant();
}

QVariant QFGroupAndRoleModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation==Qt::Horizontal) {
        if (role==Qt::DisplayRole) {
            if (section==0) return tr("RDR");
            if (section==1) return tr("group");
            if (section==2) return tr("role");
            if (section==3) return tr("folder");
        }
    }
    return QVariant();
}

int QFGroupAndRoleModel::rowCount(const QModelIndex &/*parent*/) const
{
    return project->getRawDataCount();
}

int QFGroupAndRoleModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 4;
}

bool QFGroupAndRoleModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int col=index.column();
    int row=index.row();
    if (project) {
        QFRawDataRecord* rdr=project->getRawDataByNum(row);
        if (rdr) {
            if (col==1) {
                if (role==Qt::EditRole || role==Qt::DisplayRole) {
                    if (value.type()==QVariant::String) {
                        rdr->setGroup(project->addOrFindRDRGroup(value.toString()));
                        return true;
                    } else if (value.canConvert(QVariant::Int)) {
                        rdr->setGroup(value.toInt());
                        return true;
                    }
                }

            } else if (col==2) {
                if (role==Qt::EditRole || role==Qt::DisplayRole) {
                    if (rdr->isRoleUserEditable() || roleAlwaysEditable) {
                        rdr->setRole(value.toString());
                        return true;
                    }
                }
            } else if (col==3) {
                if (role==Qt::EditRole || role==Qt::DisplayRole) {
                    rdr->setFolder(value.toString());
                    return true;
                }
            }  else if (col==0 && nameEditable) {
                if (role==Qt::EditRole || role==Qt::DisplayRole) {
                    rdr->setName(value.toString());
                    return true;
                }
            }
        }
    }
    return false;
}

void QFGroupAndRoleModel::setRoleAlwaysEditable(bool enabled)
{
    roleAlwaysEditable=enabled;
    emit dataChanged(index(0, 2), index(rowCount()-1, 2));
}

void QFGroupAndRoleModel::setNameEditable(bool enabled)
{
    nameEditable=enabled;
    emit dataChanged(index(0, 0), index(rowCount()-1, 0));
}

void QFGroupAndRoleModel::projectDataChanged()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}


QFGroupAndRoleDelegate::QFGroupAndRoleDelegate(QFProject *project, QObject *parent):
    QStyledItemDelegate(parent)
{
    this->project=project;
}

QWidget *QFGroupAndRoleDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &index) const
{
    int col=index.column();
    int row=index.row();
    if (project) {
        if (col==1) {
            QFRawDataRecord* rdr=project->getRawDataByNum(row);
            if (rdr) {
                QComboBox* cmb=new QComboBox(parent);
                cmb->setEditable(false);
                QColor groupBaseColor=QColor("aliceblue");
                QColor gc=groupBaseColor;
                double grpCount=qMax(10,project->getRDRGroupCount());

                for (int i=0; i<project->getRDRGroupCount(); i++) {
                    int colFactor=100+i/grpCount*80.0;
                    gc=groupBaseColor.darker(colFactor);
                    cmb->addItem(project->getRDRGroupName(i));
                    cmb->setItemData(i, gc, Qt::BackgroundRole);
                }
                cmb->addItem("--- new group ---");
                cmb->setCurrentIndex(index.data(Qt::EditRole).toInt());
                connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
                return cmb;
            }
        } else if (col==2) {
            QFRawDataRecord* rdr=project->getRawDataByNum(row);
            if (rdr) {
                QComboBox* cmb=new QComboBox(parent);
                cmb->setEditable(true);
                cmb->addItems(rdr->getAvailableRoles());
                if (!rdr->getAvailableRoles().contains(index.data().toString())) {
                    cmb->addItem(index.data().toString());
                }
                cmb->setEditText(index.data().toString());
                return cmb;
            }
        } else if (col==3) {
            QFRawDataRecord* rdr=project->getRawDataByNum(row);
            if (rdr) {
                QComboBox* cmb=new QComboBox(parent);
                cmb->setEditable(true);
                cmb->addItems(project->getRDRFolders());
                cmb->setEditText(index.data().toString());
                return cmb;
            }
        } else if (col==0) {
            QFRawDataRecord* rdr=project->getRawDataByNum(row);
            if (rdr) {
                QLineEdit* cmb=new QLineEdit(parent);
                cmb->setText(index.data().toString());
                return cmb;
            }
        }
    }
    return NULL;
}

void QFGroupAndRoleDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox* cmb=qobject_cast<QComboBox*>(editor);
    QLineEdit* edt=qobject_cast<QLineEdit*>(editor);
    int col=index.column();
    int row=index.row();
    if (project&&cmb) {
        QFRawDataRecord* rdr=project->getRawDataByNum(row);
        if (rdr) {
            if (col==1) {
                cmb->setCurrentIndex(rdr->getGroup());
            } else if (col==2) {
                cmb->setEditText(rdr->getRole());
            } else if (col==3) {
                cmb->setEditText(rdr->getFolder());
            }
        }
    } else if (project&&edt) {
        QFRawDataRecord* rdr=project->getRawDataByNum(row);
        if (rdr) {
            if (col==0) {
                edt->setText(rdr->getName());
            }
        }
    }
}

void QFGroupAndRoleDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox* cmb=qobject_cast<QComboBox*>(editor);
    QLineEdit* edt=qobject_cast<QLineEdit*>(editor);
    int col=index.column();
    int row=index.row();
    if (project&&cmb) {
        QFRawDataRecord* rdr=project->getRawDataByNum(row);
        if (rdr) {
            if (col==1) {
                if (cmb->count()-1>project->getRDRGroupCount()) {
                    disconnect(project, SIGNAL(structureChanged()), this, SLOT(projectDataChanged()));
                    disconnect(project, SIGNAL(sortOrderChanged(int)), this, SLOT(projectDataChanged()));
                    for (int i=project->getRDRGroupCount(); i<cmb->count()-1; i++) {
                        project->addRDRGroup(cmb->itemText(i));
                    }
                    connect(project, SIGNAL(structureChanged()), this, SLOT(projectDataChanged()));
                    connect(project, SIGNAL(sortOrderChanged(int)), this, SLOT(projectDataChanged()));
                }
                model->setData(index, cmb->currentIndex());
            } else if (col==2) {
                model->setData(index, cmb->currentText());
            } else if (col==3) {
                model->setData(index, cmb->currentText());
            }
        }
    } else     if (project&&edt) {
        QFRawDataRecord* rdr=project->getRawDataByNum(row);
        if (rdr) {
            if (col==0) {
                model->setData(index, edt->text());
            }
        }
    }

}

void QFGroupAndRoleDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

void QFGroupAndRoleDelegate::currentIndexChanged(int index)
{
    QComboBox* cmb=qobject_cast<QComboBox*>(sender());
    if (cmb&&project) {
        if (index==cmb->count()-1) {
            bool ok=false;
            disconnect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
            QString newgroup=QInputDialog::getItem(cmb, tr("new group"), tr("name of new group:"), project->getRDRGroupNames(), -1, true, &ok);
            //qDebug()<<cmb<<newgroup<<ok;
            if (ok) {
                //qDebug()<<cmb->count()<<newgroup;
                cmb->insertItem(cmb->count()-1, newgroup);
                //qDebug()<<cmb->count()<<newgroup;
                cmb->setCurrentIndex(cmb->count()-2);
                //qDebug()<<cmb->count()<<newgroup;
            }
            connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
        }
    }
}
