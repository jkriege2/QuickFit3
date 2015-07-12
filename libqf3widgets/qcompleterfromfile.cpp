#include "qcompleterfromfile.h"
#include <iostream>


QCompleterFromFile::QCompleterFromFile(QObject * parent):
    QCompleter(parent)
{
    m_stringlist=new QStringListModel(this);
    setModel(m_stringlist);
}

QCompleterFromFile::~QCompleterFromFile()
{
    //dtor
}

void QCompleterFromFile::setFilename(QString val) {
    m_filename = val;
    disconnect(model(), SIGNAL(dataChanged ( const QModelIndex & , const QModelIndex &  )), this, SLOT(modelDataChanged ( const QModelIndex & , const QModelIndex &  )));
    disconnect(model(), SIGNAL(rowsInserted ( const QModelIndex & , int, int  )), this, SLOT(modelRowChanged ( const QModelIndex & , int, int )));
    disconnect(model(), SIGNAL(rowsRemoved ( const QModelIndex & , int, int  )), this, SLOT(modelRowChanged ( const QModelIndex & , int, int )));
    disconnect(model(), SIGNAL(modelReset ()), this, SLOT(modelReset()));
    if (m_filename.size()>0) {
        //std::cout<<"opening for reading '"<<m_filename.toStdString()<<"' ... ";
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

void QCompleterFromFile::modelDataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight ) {
    //std::cout<<"model changed\n";
    if (m_filename.size()>0) {
        QFile f(m_filename);
        //std::cout<<"opening for writing '"<<m_filename.toStdString()<<"' ... ";
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

void QCompleterFromFile::modelRowChanged ( const QModelIndex & parent, int start, int end ) {
    modelDataChanged(QModelIndex(), QModelIndex());
}

void QCompleterFromFile::modelReset () {
    modelDataChanged(QModelIndex(), QModelIndex());
}

