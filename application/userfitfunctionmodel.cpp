/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "userfitfunctionmodel.h"
#include "float.h"

UserFitFunctionModel::UserFitFunctionModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

void UserFitFunctionModel::loadSettings(QSettings &settings)
{
    items.clear();
    int cnt=settings.value("function/param_count", 0).toInt();
    for (int i=0; i<cnt; i++) {
        QString grp=QString("parameter%1/").arg(i+1);
        FPItem itm;
        itm.id=settings.value(grp+"id", QString("p%1").arg(i+1)).toString();
        itm.name=settings.value(grp+"name", QString("p%1").arg(i+1)).toString();
        itm.label=settings.value(grp+"name_html", "").toString();
        itm.unit=settings.value(grp+"unit", "").toString();
        itm.type=settings.value(grp+"type", "float").toString();
        itm.unitLabel=settings.value(grp+"unit_html","").toString();
        itm.fit=settings.value(grp+"fit", true).toBool();
        itm.userEditable=settings.value(grp+"editable", true).toBool();
        itm.userRangeEditable=settings.value(grp+"range_editable", true).toBool();
        itm.initialFix=settings.value(grp+"init_fix", false).toBool();
        bool ok=false;
        itm.initialValue=settings.value(grp+"init_value", 0.0).toDouble(&ok);
        itm.minValue=settings.value(grp+"min", -DBL_MAX).toDouble(&ok);
        if (!ok) itm.minValue= -DBL_MAX;
        itm.maxValue=settings.value(grp+"max", DBL_MAX).toDouble(&ok);
        if (!ok) itm.maxValue= DBL_MAX;
        itm.absMinValue=settings.value(grp+"abs_min", -DBL_MAX).toDouble(&ok);
        if (!ok) itm.absMinValue= -DBL_MAX;
        itm.absMaxValue=settings.value(grp+"abs_max", DBL_MAX).toDouble(&ok);
        if (!ok) itm.absMaxValue= DBL_MAX;
        itm.inc=settings.value(grp+"inc", 1.0).toDouble(&ok);
        items.append(itm);
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}

void UserFitFunctionModel::storeSettings(QSettings &settings) const
{
    settings.setValue("function/param_count", items.count());
    for (int i=0; i<items.count(); i++) {
        QString grp=QString("parameter%1/").arg(i+1);
        settings.setValue(grp+"id", items[i].id);
        settings.setValue(grp+"name", items[i].name);
        settings.setValue(grp+"name_html", items[i].label);
        settings.setValue(grp+"unit", items[i].unit);
        settings.setValue(grp+"unit_html",items[i].unitLabel);
        settings.setValue(grp+"type",items[i].type);
        settings.setValue(grp+"fit", items[i].fit);
        settings.setValue(grp+"editable", items[i].userEditable);
        settings.setValue(grp+"range_editable", items[i].userRangeEditable);
        settings.setValue(grp+"init_fix", items[i].initialFix);
        settings.setValue(grp+"init_value", items[i].initialValue);
        settings.setValue(grp+"min", items[i].minValue);
        settings.setValue(grp+"max", items[i].maxValue);
        settings.setValue(grp+"abs_min", items[i].absMinValue);
        settings.setValue(grp+"abs_max", items[i].absMaxValue);
        settings.setValue(grp+"inc", items[i].inc);
    }
}

QVector<double> UserFitFunctionModel::getInitParams() const
{
    QVector<double> dat;
    for (int i=0; i<items.count(); i++) {
        dat<<items[i].initialValue;
    }
    return dat;
}

void UserFitFunctionModel::deleteParameter(int idx)
{
    if (idx>=0 && idx<items.size()) items.removeAt(idx);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}

void UserFitFunctionModel::addParameter()
{
    FPItem itm;
    itm.id=QString("p%1").arg(rowCount());
    itm.name=itm.id;
    itm.label="";
    itm.unit="";
    itm.unitLabel="";
    itm.fit=true;
    itm.userEditable=true;
    itm.userRangeEditable=true;
    itm.initialFix=false;
    itm.initialValue=1;
    itm.minValue=-DBL_MAX;
    itm.maxValue=DBL_MAX;
    itm.absMinValue=-DBL_MAX;
    itm.absMaxValue=DBL_MAX;
    itm.inc=1;
    itm.type="float";
    items.append(itm);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}

void UserFitFunctionModel::clear()
{
    items.clear();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}

void UserFitFunctionModel::moveUp(int idx)
{
    if (idx>0) items.swap(idx, idx-1);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}

void UserFitFunctionModel::moveDown(int idx)
{
    if (idx>=0 && idx<items.size()-1) items.swap(idx, idx+1);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}

QStringList UserFitFunctionModel::getIDs() const
{
    QStringList res;
    for (int i=0; i<items.size(); i++) {
        res<<items[i].id;
    }
    return res;
}

int UserFitFunctionModel::rowCount(const QModelIndex &parent) const
{
    return items.size();
}

int UserFitFunctionModel::columnCount(const QModelIndex &parent) const
{
    return 16;
}

Qt::ItemFlags UserFitFunctionModel::flags(const QModelIndex &index) const
{
    switch (index.column()) {
        case 5:
        case 6:
        case 7:
        case 8: return Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsEnabled;
        default: break;
    }
    return Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled;
}

QVariant UserFitFunctionModel::data(const QModelIndex &index, int role) const
{
    if (role==Qt::DisplayRole || role==Qt::EditRole) {
        switch (index.column()) {
            case 0: return items[index.row()].id;
            case 1: return items[index.row()].name;
            case 2: return items[index.row()].label;
            case 3: return items[index.row()].type;
            case 4: return items[index.row()].initialValue;
            //case 5: return items[index.row()].initialFix;
            //case 6: return items[index.row()].fit;
            //case 7: return items[index.row()].userEditable;
            //case 8: return items[index.row()].userRangeEditable;
            case 9: return items[index.row()].unit;
            case 10: return items[index.row()].unitLabel;
            case 11: return items[index.row()].minValue;
            case 12: return items[index.row()].maxValue;
            case 13: return items[index.row()].absMinValue;
            case 14: return items[index.row()].absMaxValue;
            case 15: return items[index.row()].inc;
            default: return QVariant();
        }
    }
    if (role==Qt::CheckStateRole) {
        switch (index.column()) {
            case 5: return (items[index.row()].initialFix)?Qt::Checked:Qt::Unchecked;
            case 6: return (items[index.row()].fit)?Qt::Checked:Qt::Unchecked;
            case 7: return (items[index.row()].userEditable)?Qt::Checked:Qt::Unchecked;
            case 8: return (items[index.row()].userRangeEditable)?Qt::Checked:Qt::Unchecked;
            default: return QVariant();
        }
    }
    return QVariant();
}

bool UserFitFunctionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role==Qt::EditRole) {
        switch (index.column()) {
            case 0: items[index.row()].id=value.toString();return true;  break;
            case 1: items[index.row()].name=value.toString(); return true; break;
            case 2: items[index.row()].label=value.toString(); return true; break;
            case 3: items[index.row()].type=value.toString(); return true; break;
            case 4: items[index.row()].initialValue=value.toDouble(); return true; break;
            case 9: items[index.row()].unit=value.toString(); return true; break;
            case 10: items[index.row()].unitLabel=value.toString(); return true; break;
            case 11: items[index.row()].minValue=value.toDouble(); return true; break;
            case 12: items[index.row()].maxValue=value.toDouble(); return true; break;
            case 13: items[index.row()].absMinValue=value.toDouble(); return true; break;
            case 14: items[index.row()].absMaxValue=value.toDouble(); return true; break;
            case 15: items[index.row()].inc=value.toDouble(); return true; break;
            default:  break;
        }
    }
    if (role==Qt::CheckStateRole) {
        switch (index.column()) {
            case 5: items[index.row()].initialFix=(value.toInt()==Qt::Checked); return true; break;
            case 6: items[index.row()].fit=(value.toInt()==Qt::Checked); return true; break;
            case 7: items[index.row()].userEditable=(value.toInt()==Qt::Checked); return true; break;
            case 8: items[index.row()].userRangeEditable=(value.toInt()==Qt::Checked); return true; break;
            default: break;
        }
    }
    return false;
}

QVariant UserFitFunctionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation==Qt::Horizontal) {
        if (role==Qt::DisplayRole) {
            switch (section) {
                case 0: return tr("ID");
                case 1: return tr("Name");
                case 2: return tr("Name (HTML)");
                case 3: return tr("Type");
                case 4: return tr("init. Value");
                case 5: return tr("init. Fix");
                case 6: return tr("fit");
                case 7: return tr("editble");
                case 8: return tr("range editable");
                case 9: return tr("unit");
                case 10: return tr("unit (HTML)");
                case 11: return tr("min");
                case 12: return tr("max");
                case 13: return tr("abs. min");
                case 14: return tr("abs. max");
                case 15: return tr("increment");
                default: return QVariant();
            }
        }
    }
    return QVariant();
}
