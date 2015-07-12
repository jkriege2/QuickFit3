/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#ifndef QFIMFCCSPARAMETERINPUTTABLE_H
#define QFIMFCCSPARAMETERINPUTTABLE_H

#include <QAbstractTableModel>
#include "qfrawdatarecord.h"
class QFImFCCSFitEvaluationItem; //forward

class QFImFCCSParameterInputTable : public QAbstractTableModel
{
        Q_OBJECT
    public:
        enum QFImFCCSParameterInputTableWidgetTypes {
            wtNone=0,
            wtRDRCombobox=1,
            wtFitFunctionCombobox=2,
            wtFixCheck=3,
            wtValueDoubleEdit=4,
            wtValueComboBox=5,
            wtValueIntEdit=6,
            wtErrorEdit=7,
            wtGlobalParamCombo=8,
            wtValueLogDoubleEdit=9,
            wtRangeEditMin=10,
            wtRangeEditMax=11
        };

        enum QFImFCCSParameterInputTableRoles {
            rdrIDRole= Qt::UserRole,
            fitFunctionIDRole= Qt::UserRole+1,
            widgetTypeRole=  Qt::UserRole+2,
            fitParameterIDRole= Qt::UserRole+3,
            evaluationIDRole= Qt::UserRole+4,
            fitFileIDRole=Qt::UserRole+4,
            globalParamRole=Qt::UserRole+5
        };

        struct FitParam {
            FitParam();
            bool isFit;
            bool isEditable;
            QString id;
            QString label;
            QString tooltip;
            QSet<QFRawDataRecord*> visibleIn;
            bool isValid() const;



            bool operator==(const FitParam& other)const;
        };

        explicit QFImFCCSParameterInputTable(QFImFCCSFitEvaluationItem *parent = 0);
        

        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

        virtual Qt::ItemFlags flags(const QModelIndex &index) const;

        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

        int getColsPerRDR() const;

        const QList<FitParam>& getFitParamList() const;

    signals:
        void modelRebuilt();
        void fitParamChanged();
        void fitParamFixChanged();
        void fitParamGlobalChanged();
        void fitParamErrorChanged();
    public slots:
        void rebuildModel();
        bool recalculateFitParameters(bool emitFitParamSignals=true, bool emitdataChangedSignals=true);
        void setEditRanges(bool enabled);
    public:
        bool checkRebuildModel(bool alwaysreset=false);
    protected:
        QFImFCCSFitEvaluationItem* item;

        bool editRanges;


        QList<FitParam> fitparamids;

        static bool fitParamListContainsID(const QString& id, const QList<FitParam>& fitparamids, int* found=NULL);
        
};

#endif // QFIMFCCSPARAMETERINPUTTABLE_H
