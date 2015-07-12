/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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


#ifndef QSETTINGSMODEL_H
#define QSETTINGSMODEL_H

#include <QVariant>
#include <QStringList>
#include <QAbstractItemModel>
#include <QSettings>
#include <QRegExp>
#include "libwid_imexport.h"

class QFWIDLIB_EXPORT QSettingsModel : public QAbstractItemModel
{
        Q_OBJECT
    public:
        explicit QSettingsModel(QObject *parent = 0);
        ~QSettingsModel();

        void setSettings(QSettings* settings);

        void setByDepthColoring(bool byDepth);

        void setReadOptimizedColoring(bool ReadOptimizedColoring);

        void setFilter(const QString& filter, QRegExp::PatternSyntax syntax=QRegExp::Wildcard);


        QModelIndex index       (int   row, int col, const QModelIndex & parent = QModelIndex() ) const;
        QModelIndex parent      (const QModelIndex & index                                      ) const;
        int         rowCount    (const QModelIndex & parent = QModelIndex()                     ) const;
        int         columnCount (const QModelIndex & parent = QModelIndex()                     ) const;
        QVariant    data        (const QModelIndex & index, int role = Qt::DisplayRole          ) const;

        Qt::ItemFlags flags(const QModelIndex &index) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    signals:
        
    public slots:

    protected:
        QSettings* m_settings;
        bool byDepthColoring;
        bool readOptimizedColoring;
        QRegExp rxFilter;



        class TreeItem {
            public:
                TreeItem(const QString& path,const QString& name, TreeItem *parent = 0);
                ~TreeItem();

                void appendChild(TreeItem *item);

                TreeItem *child(int row);
                int childCount() const;
                QString getPath() const;
                QString getName() const;
                int row() const;
                TreeItem *parent();
                int depth() const;
                int maxDepth() const;

                bool matches(const QRegExp* rx, bool matchPath=true, bool matchValue=false, QSettings *settings=NULL) const;

                void removeNonmatchingChildren(const QRegExp* rx, bool matchPath=true, bool matchValue=false, QSettings *settings=NULL);

            private:
                QList<TreeItem*> childItems;
                QString path;
                QString name;
                TreeItem *parentItem;
        };

        TreeItem* root;

        void init();
        void initIntoItem(TreeItem* item);

};

#endif // QSETTINGSMODEL_H
