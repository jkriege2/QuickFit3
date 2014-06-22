/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#include "qfcompleterfromfile.h"
#include <QDebug>
#include <iostream>
#include <QDir>

QFCompleterFromFile::QFCompleterFromFile(QObject * parent):
    QCompleter(parent)
{
    m_stringlist=new QStringListModel(this);
    setModel(m_stringlist);
    setCompletionMode(QCompleter::PopupCompletion);
}

QFCompleterFromFile::~QFCompleterFromFile()
{
    modelDataChanged(QModelIndex(),QModelIndex());
}

void QFCompleterFromFile::setFilename(QString val) {
    modelDataChanged (QModelIndex(),QModelIndex());
    m_filename = val;
    disconnect(model(), SIGNAL(dataChanged ( const QModelIndex & , const QModelIndex &  )), this, SLOT(modelDataChanged ( const QModelIndex & , const QModelIndex &  )));
    disconnect(model(), SIGNAL(rowsInserted ( const QModelIndex & , int, int  )), this, SLOT(modelRowChanged ( const QModelIndex & , int, int )));
    disconnect(model(), SIGNAL(rowsRemoved ( const QModelIndex & , int, int  )), this, SLOT(modelRowChanged ( const QModelIndex & , int, int )));
    disconnect(model(), SIGNAL(modelReset ()), this, SLOT(modelReset()));
    if (m_filename.size()>0) {
        //std::cout<<"opening for reading '"<<m_filename.toStdString()<<"' ... ";
        //qDebug()<<"opening for reading: "<<m_filename;
        QFile f(m_filename);
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            //std::cout<<"OK\n";
            QString s=QString::fromUtf8(f.readAll());
            QStringList sl=s.split('\n', QString::SkipEmptyParts);
            m_stringlist->setStringList(sl);
        } //else std::cout<<"ERROR\n";
        connect(model(), SIGNAL(dataChanged ( const QModelIndex & , const QModelIndex &  )), this, SLOT(modelDataChanged ( const QModelIndex & , const QModelIndex &  )));
        connect(model(), SIGNAL(rowsInserted ( const QModelIndex & , int, int  )), this, SLOT(modelRowChanged ( const QModelIndex & , int, int )));
        connect(model(), SIGNAL(rowsRemoved ( const QModelIndex & , int, int  )), this, SLOT(modelRowChanged ( const QModelIndex & , int, int )));
        connect(model(), SIGNAL(modelReset ()), this, SLOT(modelReset()));
    }
}

void QFCompleterFromFile::modelDataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight ) {
    //std::cout<<"model changed\n";
    if (m_filename.size()>0) {
        QDir().mkpath(QFileInfo(m_filename).absolutePath());
        QFile f(m_filename);
        //qDebug()<<"opening for writing: "<<m_filename;
        if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
            //std::cout<<"OK\n";
            QStringList sl=m_stringlist->stringList();
            QFile f1(m_filename);
            if (f1.open(QIODevice::ReadOnly|QIODevice::Text)) {
                //std::cout<<"OK\n";
                QString s=QString::fromUtf8(f1.readAll());
                sl.append(s.split('\n', QString::SkipEmptyParts));
            } //else std::cout<<"ERROR\n";
            sl.removeDuplicates();
            sl.sort();
            for (int i=0; i<sl.size(); i++) {
                f.write(QString(sl[i]+"\n").toUtf8());
                //std::cout<<"   writing "<<itemText(i).toStdString()<<std::endl;
            }
        } //else std::cout<<"ERROR\n";
    }
}

void QFCompleterFromFile::modelRowChanged ( const QModelIndex & parent, int start, int end ) {
    modelDataChanged(QModelIndex(), QModelIndex());
}

void QFCompleterFromFile::modelReset () {
    modelDataChanged(QModelIndex(), QModelIndex());
}

