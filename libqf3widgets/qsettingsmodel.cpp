/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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



#include "qsettingsmodel.h"
#include <QApplication>
#include <QColor>
#include <QFont>
#include <QDebug>

QSettingsModel::QSettingsModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    m_settings=NULL;
    root=NULL;
    byDepthColoring=false;
    readOptimizedColoring=false;
    rxFilter.setCaseSensitivity(Qt::CaseInsensitive);
    init();
}

QSettingsModel::~QSettingsModel()
{
    delete root;
}

void QSettingsModel::setSettings(QSettings *settings) {
    m_settings=settings;
    init();
}

void QSettingsModel::setByDepthColoring(bool byDepth)
{
    byDepthColoring=byDepth;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}

void QSettingsModel::setReadOptimizedColoring(bool ReadOptimizedColoring) {
    readOptimizedColoring=ReadOptimizedColoring;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}

void QSettingsModel::setFilter(const QString &filter, QRegExp::PatternSyntax syntax)
{
    rxFilter.setPatternSyntax(syntax);
    rxFilter.setPattern(filter);
    init();
}

QModelIndex QSettingsModel::index(int row, int column, const QModelIndex &parent) const {
    QSettingsModel::TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = root;
    else
        parentItem = static_cast<QSettingsModel::TreeItem*>(parent.internalPointer());

    QSettingsModel::TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex QSettingsModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return QModelIndex();

    QSettingsModel::TreeItem *childItem = static_cast<QSettingsModel::TreeItem*>(index.internalPointer());
    QSettingsModel::TreeItem *parentItem = childItem->parent();

    if (parentItem == root)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int QSettingsModel::rowCount(const QModelIndex &parent) const {
    QSettingsModel::TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = root;
    else
        parentItem = static_cast<QSettingsModel::TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int QSettingsModel::columnCount(const QModelIndex &/*parent*/) const {
    return 3;
}

QVariant QSettingsModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    QSettingsModel::TreeItem *item = static_cast<QSettingsModel::TreeItem*>(index.internalPointer());

    if (!item || !m_settings) return QVariant();

    double d=double(item->depth()-1.0)/double(root->maxDepth());
    if (role == Qt::DisplayRole) {
        if (index.column()==0) return item->getName();
        if (index.column()==1 && item->childCount()<=0) {
            return m_settings->value(item->getPath(), QVariant()).typeName();
        }
        if (index.column()==2 && item->childCount()<=0) {
            return m_settings->value(item->getPath(), QVariant()).toString();
        }
    } else if (readOptimizedColoring && (role == Qt::TextColorRole)) {
        QColor c0("lightskyblue");
        QColor c1("deepskyblue");
        int r=qRound(255.0*(c0.redF()+d*(c1.redF()-c0.redF())));
        int g=qRound(255.0*(c0.greenF()+d*(c1.greenF()-c0.greenF())));
        int b=qRound(255.0*(c0.blueF()+d*(c1.blueF()-c0.blueF())));
        if (r<0) r=0; if (r>255) r=255;
        if (g<0) g=0; if (g>255) g=255;
        if (b<0) b=0; if (b>255) b=255;
        if (index.column()==1) return QColor(r,g,b);
    } else if (byDepthColoring && (role == Qt::BackgroundColorRole)) {
        double g0=0.65;
        double g1=0.96;
        int g=qRound(255.0*(g0+d*(g1-g0)));
        if (g<0) g=0;
        if (g>255) g=255;
        return QColor(g,g,g);
    } else if (readOptimizedColoring && (role == Qt::FontRole)) {
        QFont f=QApplication::font();
        if (item->childCount()>0) f.setBold(true);
        else f.setItalic(true);
        if (index.column()==0) return f;
    }

    return QVariant();
}

Qt::ItemFlags QSettingsModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QSettingsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section==0) return tr("path");
        if (section==1) return tr("type");
        if (section==2) return tr("value");
    }

    return QVariant();
}

void QSettingsModel::init() {
    QSettingsModel::TreeItem* rold=root;
    QSettingsModel::TreeItem* r1=new QSettingsModel::TreeItem("", "");
    initIntoItem(r1);
    if (!rxFilter.pattern().isEmpty()) r1->removeNonmatchingChildren(&rxFilter, true, true, m_settings);
    root=r1;
    delete rold;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}

void QSettingsModel::initIntoItem(QSettingsModel::TreeItem *item) {
    if (!m_settings) return;
    for (int i=0; i<m_settings->childGroups().size(); i++) {
        QString g=m_settings->childGroups().at(i);
        m_settings->beginGroup(g);
        QSettingsModel::TreeItem* r1=new QSettingsModel::TreeItem(m_settings->group(), g, item);
        initIntoItem(r1);
        item->appendChild(r1);
        m_settings->endGroup();
    }
    for (int i=0; i<m_settings->childKeys().size(); i++) {
        QString k=m_settings->childKeys().at(i);
        QSettingsModel::TreeItem* r1=new QSettingsModel::TreeItem(m_settings->group()+"/"+k, k, item);
        item->appendChild(r1);
    }
}








QSettingsModel::TreeItem::TreeItem(const QString &path, const QString &name, TreeItem *parent) {
    this->path=path;
    this->name=name;
    this->parentItem=parent;
}

QSettingsModel::TreeItem::~TreeItem() {
    qDeleteAll(childItems);
}

void QSettingsModel::TreeItem::appendChild(QSettingsModel::TreeItem *item) {
    childItems.append(item);
}

QSettingsModel::TreeItem *QSettingsModel::TreeItem::child(int row) {
    return childItems.value(row);
}

int QSettingsModel::TreeItem::childCount() const {
    return childItems.count();
}

QString QSettingsModel::TreeItem::getPath() const {
    return path;
}

QString QSettingsModel::TreeItem::getName() const {
    return name;
}

int QSettingsModel::TreeItem::row() const {
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<QSettingsModel::TreeItem*>(this));

    return 0;
}

QSettingsModel::TreeItem *QSettingsModel::TreeItem::parent() {
    return parentItem;
}

int QSettingsModel::TreeItem::depth() const {
    if (parentItem) return parentItem->depth()+1;
    else return 0;
}

int QSettingsModel::TreeItem::maxDepth() const {
    int d=depth();
    for (int i=0; i<childItems.size(); i++) {
        int md=childItems[i]->maxDepth();
        if (md>d) d=md;
    }
    return d;
}

bool QSettingsModel::TreeItem::matches(const QRegExp *rx, bool matchPath, bool matchValue, QSettings* settings) const {
    if (!rx) return false;
    if (rx->pattern().isEmpty()) return true;
    bool m=false;
    if (matchPath) m= (rx->indexIn(path)>=0);
    if (!m && matchValue && settings) m=(rx->indexIn(settings->value(path, "").toString())>=0);
    if (!m) {
        for (int i=0; i<childItems.size(); i++) {
            if (childItems[i]->matches(rx, matchPath, matchValue, settings)) return true;
        }
    }
    return m;
}

void QSettingsModel::TreeItem::removeNonmatchingChildren(const QRegExp *rx, bool matchPath, bool matchValue, QSettings* settings) {
    for (int i=childItems.size()-1; i>=0; i--) {
        if (!childItems[i]->matches(rx, matchPath, matchValue, settings)) {
            delete childItems[i];
            childItems.removeAt(i);
        } else {
            childItems[i]->removeNonmatchingChildren(rx, matchPath, matchValue, settings);
        }
    }
}
