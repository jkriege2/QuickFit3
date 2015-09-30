/*
  Name: errormodel.cpp
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/

*/

#include "errormodel.h"
#include <QIcon>

ErrorModel::ErrorModel(sequencer::baseImporter* importer, QObject *parent)
     : QAbstractTableModel(parent)
{
    // load the events from the supplied importer (if one is suplied)
    events.clear();
    if (importer!=NULL) loadErrors(importer);

    // test code:
    /*sequencer::bi_event e;

    e.type=sequencer::biInfo;
    e.text="info message";
    e.num=0;
    e.position=1;
    e.time=-1;
    e.filename="test.sdff";
    events.push_back(e);

    e.type=sequencer::biError;
    e.text="error message\nwith two lines";
    e.num=2;
    e.position=2;
    e.time=5.548;
    e.filename="test2.sdff";
    events.push_back(e);

    e.type=sequencer::biWarning;
    e.text="warning message";
    e.num=3;
    e.position=3;
    e.time=8.54e-4;
    e.filename="test3.sdff";
    events.push_back(e);*/
}

void ErrorModel::loadErrors(sequencer::baseImporter* importer){
    events.clear();
    if (importer!=NULL) {
        if (importer->get_events_count()>0) for (unsigned int i=0; i<importer->get_events_count(); i++) {
            sequencer::bi_event e=importer->get_event(i);
            events.push_back(e);
        }
    }
    reset();
};

void ErrorModel::addEvent(sequencer::bi_event event) {
    events.push_back(event);
    reset();
}

int ErrorModel::rowCount(const QModelIndex &parent) const{
    return events.size();
};

int ErrorModel::columnCount(const QModelIndex & parent) const{
    return 4;
};

QVariant ErrorModel::data(const QModelIndex &index, int role) const{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= events.size())
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch(index.column()) {
            case 0: {
                if (events[index.row()].type == sequencer::biInfo) return QString(tr("info"));
                if (events[index.row()].type == sequencer::biWarning) return QString(tr("warning"));
                if (events[index.row()].type == sequencer::biError) return QString(tr("error"));
                return QVariant();
            }
            case 1: return QVariant((qlonglong)(events[index.row()].position));
            case 2: return QVariant(events[index.row()].time);
            case 3: return QString(events[index.row()].text.c_str());
            default: return QVariant();
        }
    } else if (role == Qt::DecorationRole) {
        switch(index.column()) {
            case 0: {
                if (events[index.row()].type == sequencer::biInfo) return QIcon(":/event_info.png");
                if (events[index.row()].type == sequencer::biWarning) return QIcon(":/event_warning.png");
                if (events[index.row()].type == sequencer::biError) return QIcon(":/event_error.png");
                return QVariant();
            }
            default: return QVariant();
        }
    } else if (role == Qt::ForegroundRole) { // color of the output, depending on info, error or warning message
        switch(events[index.row()].type) {
            case sequencer::biInfo: return QColor("darkgreen");
            case sequencer::biError: return QColor("darkred");
            case sequencer::biWarning: return QColor("darkblue");
            default: QColor("black");
        }
    } else if (role == Qt::SizeHintRole) {
        switch(index.column()) {
            case 0: return QSize(50, 13);
            case 1: return QSize(50, 13);
            case 2: return QSize(50, 13);
            case 3: return QSize(625, 13);
        }
    } else {
        return QVariant();
    }
    return QVariant();
};

QVariant ErrorModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return QString("type");
            case 1: return QString("line");
            case 2: return QString("time");
            case 3: return QString("errormessage");
            default: return QVariant();
        }
    } else {
        return QVariant(); //QString("%1").arg(section);
    }
};

sequencer::bi_event ErrorModel::getEvent(unsigned int n){
    return events[n];
};

