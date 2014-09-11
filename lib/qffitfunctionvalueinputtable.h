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

#ifndef QFFitFunctionValueInputTable_H
#define QFFitFunctionValueInputTable_H

#include <QAbstractTableModel>
#include "lib_imexport.h"
#include <QStringList>
#include<QMap>
#include<QVector>
#include "float.h"


class QFFitParameterBasicInterface;
class QFRawDataRecord;
class QFFitFunction;


struct QFLIB_EXPORT QFFitFunctionValueInputTableFitParamData {
        QFFitFunctionValueInputTableFitParamData();
        QFFitFunctionValueInputTableFitParamData(double value, double error=0, bool fix=false, double min=-DBL_MAX, double max=DBL_MAX, bool editable=true);

    double value;
    double error;
    bool fix;
    double min;
    double max;
    bool editable;
};

class QFLIB_EXPORT QFFitFunctionValueInputTable : public QAbstractTableModel
{
        Q_OBJECT
    public:

        explicit QFFitFunctionValueInputTable(QObject *parent = 0);
        virtual ~QFFitFunctionValueInputTable();

    public:
        enum WidgetTypes {
            wtNone=0,
            wtFixCheck=1,
            wtValueDoubleEdit=2,
            wtValueComboBox=3,
            wtValueIntEdit=4,
            wtErrorEdit=5,
            wtValueLogDoubleEdit=6,
            wtRangeEditMin=7,
            wtRangeEditMax=8
        };

        enum  MyUserRoles{
            fitFunctionIDRole= Qt::UserRole+1,
            widgetTypeRole=  Qt::UserRole+2,
            fitParameterIDRole= Qt::UserRole+3,
            fitParameterMinRole=Qt::UserRole+4,
            fitParameterMaxRole=Qt::UserRole+5,
            fileIDRole=Qt::UserRole+6
        };


        void setWriteTo(QVector<double>* datavector, QFFitFunction* fitfunction, bool ownsFitFunction=true);
        void setWriteTo(QVector<double>* datavector, const QStringList& rownames, const QStringList& rowlabels=QStringList());
        void setWriteTo(QMap<QString, QFFitFunctionValueInputTableFitParamData>* datamap, QFFitFunction* fitfunction, bool ownsFitFunction=true);
        void setWriteTo(QMap<QString, QFFitFunctionValueInputTableFitParamData>* datamap, const QStringList& rownames, const QStringList& rowlabels=QStringList());

        void setWriteToFitParamBasic(QFFitParameterBasicInterface* intf);
        void setAuxiliaryWriteTo(QVector<double>* errorvector, QVector<double>* minvector, QVector<double>* maxvector, QVector<bool>* fixvector);

        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

        virtual Qt::ItemFlags flags(const QModelIndex &index) const;

        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

        int getColsPerRDR() const;
        void setColBasicWidth(int w);
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
        void setEditErrors(bool editerrors);
        void setEditFix(bool editfix);
        void setUserEditRanges(bool enabled);
        void setUserEditErrors(bool editerrors);
        void setUserEditFix(bool editfix);
        void setDoRebuildModel(bool doReb);
    public:
        bool checkRebuildModel(bool alwaysreset=false);
    protected:
        int colbasicWidth;
        bool doRebuild;
        QFFitParameterBasicInterface* item;
        QVector<double>* datavector;
        QVector<double>* errorvector;
        QVector<double>* minvector;
        QVector<double>* maxvector;
        QVector<bool>* fixvector;
        QMap<QString, QFFitFunctionValueInputTableFitParamData>* datamap;
        QFFitFunction* fitfunction;
        bool ownsFitFunction;
        QStringList rownames;
        QStringList rowlabels;
        bool editerrors;
        bool editfix;
        bool editRanges;

        bool errorsUserEditable;
        bool rangesUserEditable;
        bool fixUserEditable;
        int rangeIdx;
        int errorIdx;
        int fixIdx;
        QStringList colNames;
        QList<int> colWidth;

        struct FitParam {
            FitParam();
            bool isFit;
            QString id;
            int num;
            QString label;
            QString tooltip;
            bool isValid() const;

            bool operator==(const FitParam& other)const;
        };

        QList<FitParam> fitparamids;

        static bool fitParamListContainsID(const QString& id, const QList<FitParam>& fitparamids, int* found=NULL);

        double getParameterMin(int row) const;
        double getParameterMax(int row) const;
        double getParameterValue(int row) const;
        double getParameterError(int row) const;
        bool getParameterFix(int row) const;
        bool getParameterEditable(int row) const;
        double getParameterValue(const QString& id) const;
        double getParameterError(const QString& id) const;

        void setParameterMin(int row, double value);
        void setParameterMax(int row, double value);
        void setParameterValue(int row, double value);
        void setParameterError(int row, double value);
        void setParameterValueError(int row, double value, double error, bool emitSignals=true);
        void setParameterFix(int row, bool value);

};

#endif // QFFitFunctionValueInputTable_H
