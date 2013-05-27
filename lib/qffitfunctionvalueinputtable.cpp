#include "qffitfunctionvalueinputtable.h"
#include "qfautooutputtimer.h"
#include <QScopedPointer>
#include "qffitfunction.h"
#include "qffitfunctionmanager.h"
#include "qfpluginservices.h"
#include "qffitparameterbasicinterface.h"
#include "qfrawdatarecord.h"
#include "qffitfunction.h"

QFFitFunctionValueInputTable::QFFitFunctionValueInputTable(QObject *parent) :
    QAbstractTableModel(parent)
{
    doRebuild=true;
    this->editRanges=false;
    this->editerrors=false;
    this->editfix=false;
    this->errorvector=NULL;
    this->minvector=NULL;
    this->maxvector=NULL;
    this->fixvector=NULL;
    this->item=NULL;
    this->datavector=NULL;
    this->rownames.clear();
    this->fitfunction=NULL;
    this->datamap=NULL;
    this->ownsFitFunction=false;
    colbasicWidth=75;
    errorIdx=-1;
    rangeIdx=-1;
    fixIdx=-1;
}

QFFitFunctionValueInputTable::~QFFitFunctionValueInputTable()
{
    if (this->ownsFitFunction && this->fitfunction) delete this->fitfunction;
}

void QFFitFunctionValueInputTable::setWriteToFitParamBasic(QFFitParameterBasicInterface *interface)
{
    item=interface;
    datavector=NULL;
    rownames.clear();
    if (this->ownsFitFunction && this->fitfunction) delete this->fitfunction;
    this->fitfunction=NULL;
    rebuildModel();
}

void QFFitFunctionValueInputTable::setWriteTo(QVector<double> *datavector, QFFitFunction *fitfunction, bool ownsFitFunction)
{
    item=NULL;
    this->datavector=datavector;
    rownames.clear();
    if (this->ownsFitFunction && this->fitfunction) delete this->fitfunction;
    this->fitfunction=fitfunction;
    this->datamap=NULL;
    this->ownsFitFunction=ownsFitFunction;
    rebuildModel();
}

void QFFitFunctionValueInputTable::setWriteTo(QVector<double> *datavector, const QStringList &rownames, const QStringList& rowlabels)
{
    item=NULL;
    this->datavector=datavector;
    this->rownames=rownames;
    if (this->ownsFitFunction && this->fitfunction) delete this->fitfunction;
    this->fitfunction=NULL;
    this->datamap=NULL;
    this->rowlabels=rowlabels;
    rebuildModel();
}

void QFFitFunctionValueInputTable::setWriteTo(QMap<QString, QFFitFunctionValueInputTableFitParamData> *datamap, QFFitFunction *fitfunction, bool ownsFitFunction)
{
    item=NULL;
    this->datavector=NULL;
    this->rownames.clear();
    if (this->ownsFitFunction && this->fitfunction) delete this->fitfunction;
    this->fitfunction=fitfunction;
    this->datamap=datamap;
    this->ownsFitFunction=ownsFitFunction;
    rebuildModel();
}

void QFFitFunctionValueInputTable::setWriteTo(QMap<QString, QFFitFunctionValueInputTableFitParamData> *datamap, const QStringList &rownames, const QStringList& rowlabels)
{
    item=NULL;
    this->datavector=NULL;
    this->rownames=rownames;
    if (this->ownsFitFunction && this->fitfunction) delete this->fitfunction;
    this->fitfunction=NULL;
    this->datamap=datamap;
    this->rowlabels=rowlabels;
    rebuildModel();
}

void QFFitFunctionValueInputTable::setEditErrors(bool editerrors)
{
    this->editerrors=editerrors;
    rebuildModel();
}

void QFFitFunctionValueInputTable::setEditFix(bool editfix)
{
    this->editfix=editfix;
    rebuildModel();
}

void QFFitFunctionValueInputTable::setAuxiliaryWriteTo(QVector<double> *errorvector, QVector<double> *minvector, QVector<double> *maxvector, QVector<bool> *fixvector)
{
    this->errorvector=errorvector;
    this->minvector=minvector;
    this->maxvector=maxvector;
    this->fixvector=fixvector;
}

int QFFitFunctionValueInputTable::rowCount(const QModelIndex &parent) const
{
    return fitparamids.size();
}

int QFFitFunctionValueInputTable::columnCount(const QModelIndex &parent) const
{
    return 1+getColsPerRDR();
}

Qt::ItemFlags QFFitFunctionValueInputTable::flags(const QModelIndex &index) const
{
    //QFAutoOutputTimer(QString("flags(%1, %2, %3)").arg(index.row()).arg(index.column()));
    Qt::ItemFlags f=Qt::ItemIsEnabled|Qt::ItemIsSelectable;
    int col=index.column();
    int row=index.row();
    if (col>0) {
        int cols=(col-1)/getColsPerRDR();
        int coli=(col-1)%getColsPerRDR();
        if (coli==0) {
            if (getParameterEditable(row)) f=f|Qt::ItemIsEditable;
        } else if (fitfunction && editfix && coli==fixIdx) {
            FitParam fp=fitparamids.value(row, FitParam());
            if (fp.isValid() && fitfunction->hasParameter(fp.id) && fitfunction->getDescription(fp.id).fit) {
                f=f|Qt::ItemIsUserCheckable;
            }
        } else if (editfix && coli==fixIdx) {
            f=f|Qt::ItemIsUserCheckable;
        } else if (fitfunction && editerrors && coli==errorIdx) {
            FitParam fp=fitparamids.value(row, FitParam());
            if (fp.isValid() && fitfunction->hasParameter(fp.id) && fitfunction->getDescription(fp.id).displayError==QFFitFunction::EditError) {
                f=f|Qt::ItemIsEditable;
            }
        } else if (editerrors && coli==errorIdx) {
            f=f|Qt::ItemIsEditable;
        } else if (fitfunction && editRanges && (coli==rangeIdx || coli==rangeIdx+1)) {
            FitParam fp=fitparamids.value(row, FitParam());
            if (fp.isValid() && fitfunction->hasParameter(fp.id) && fitfunction->getDescription(fp.id).displayError==QFFitFunction::EditError) {
                f=f|Qt::ItemIsEditable;
            }
        } else if (editRanges && (coli==rangeIdx || coli==rangeIdx+1)) {
            f=f|Qt::ItemIsEditable;
        }
    }
    return f;
}

QVariant QFFitFunctionValueInputTable::data(const QModelIndex &index, int role) const
{

    int col=index.column();
    int row=index.row();
    if (col==0) {
        if (role==Qt::DisplayRole || role==Qt::EditRole) return fitparamids.value(row, QFFitFunctionValueInputTable::FitParam()).label;

        if (role==Qt::FontRole) {
            QFont f=QApplication::font();
            f.setBold(true);
            return f;
        }
    } else if (col>0) {
        int cols=(col-1)/getColsPerRDR();
        int coli=(col-1)%getColsPerRDR();
        //if (role==fitFileIDRole) return cols;

        if (role==fitFunctionIDRole) {
            if (fitfunction) return fitfunction->id();
            else if (row>=0 && row<rownames.size()) return rownames[row];
            else return QVariant();
        }

        FitParam fp=fitparamids.value(row, FitParam());
        if (role==fitParameterIDRole) {
            if (fp.isValid()) return fp.id;
            else return QVariant();
        }
        if (role==fitParameterMinRole) {
            return getParameterMin(row);
        }
        if (role==fitParameterMaxRole) {
            return getParameterMax(row);
        }

        if (coli==0) {
            if (role==Qt::DisplayRole || role==Qt::EditRole) {
                const double val=getParameterValue(row);
                const double err=getParameterError(row);
                if (role==Qt::DisplayRole) {
                    if (fabs(err)>1e-25) return roundWithError(val, err);
                    return roundError(val, 4);
                } else {
                    return val;
                }
            }
            if (role==widgetTypeRole || role==Qt::BackgroundRole) {
                if (fitfunction) {
                    QFFitFunction::ParameterDescription desc=fitfunction->getDescription(fp.id);
                    if (role==widgetTypeRole) {
                        if (desc.userEditable) {
                            if (desc.type==QFFitFunction::FloatNumber) return wtValueDoubleEdit;
                            if (desc.type==QFFitFunction::LogFloatNumber) return wtValueLogDoubleEdit;
                            if (desc.type==QFFitFunction::IntNumber) return wtValueIntEdit;
                            if (desc.type==QFFitFunction::IntCombo) return wtValueComboBox;
                        }
                        return wtNone;
                    } else if (role==Qt::BackgroundRole && !desc.userEditable){
                        return QBrush(QApplication::palette().color(QPalette::Window));
                    }
                } else if (role==widgetTypeRole) {
                    if (getParameterEditable(row)) return wtValueDoubleEdit;
                } else if (role==Qt::BackgroundRole && !getParameterEditable(row)){
                    return QBrush(QApplication::palette().color(QPalette::Window));
                }
            }
        } else if (coli==errorIdx && editerrors) {
            if (role==Qt::DisplayRole || role==Qt::EditRole) {
                const double err=getParameterError(row);
                if (role==Qt::DisplayRole) {
                    roundError(err);
                } else {
                    return err;
                }
            }
            if (role==widgetTypeRole || role==Qt::BackgroundRole) {
                if (fitfunction) {
                    QFFitFunction::ParameterDescription desc=fitfunction->getDescription(fp.id);
                    if (role==widgetTypeRole) {
                        if (desc.displayError==QFFitFunction::EditError) {
                            return wtErrorEdit;
                        }
                        return wtNone;
                    } else if (role==Qt::BackgroundRole && desc.displayError!=QFFitFunction::EditError){
                        return QBrush(QApplication::palette().color(QPalette::Window));
                    }
                } else if (role==widgetTypeRole) {
                    return wtErrorEdit;
                }
            }
        } else if (coli==rangeIdx && editRanges) {
            if (role==Qt::DisplayRole || role==Qt::EditRole) {
                return getParameterMin(row);
            }
            if (role==widgetTypeRole || role==Qt::BackgroundRole) {
                if (fitfunction) {
                    QFFitFunction::ParameterDescription desc=fitfunction->getDescription(fp.id);
                    if (role==widgetTypeRole) {
                        if (desc.userRangeEditable) {
                            return wtRangeEditMin;
                        }
                        return wtNone;
                    } else if (role==Qt::BackgroundRole && !desc.userRangeEditable){
                        return QBrush(QApplication::palette().color(QPalette::Window));
                    }
                } else if (role==widgetTypeRole) {
                    return wtRangeEditMin;
                }
            }
        } else if (coli==rangeIdx+1 && editRanges) {
            if (role==Qt::DisplayRole || role==Qt::EditRole) {
                return getParameterMax(row);
            }
            if (role==widgetTypeRole || role==Qt::BackgroundRole) {
                if (fitfunction) {
                    QFFitFunction::ParameterDescription desc=fitfunction->getDescription(fp.id);
                    if (role==widgetTypeRole) {
                        if (desc.userRangeEditable) {
                            return wtRangeEditMax;
                        }
                        return wtNone;
                    } else if (role==Qt::BackgroundRole && !desc.userRangeEditable){
                        return QBrush(QApplication::palette().color(QPalette::Window));
                    }
                } else if (role==widgetTypeRole) {
                    return wtRangeEditMax;
                }
            }
        } else if (coli==fixIdx && editfix) {
            if (role==Qt::CheckStateRole||role==Qt::DisplayRole||role==Qt::EditRole) {
                return getParameterFix(row);
            }
        }
    }
    if (role==widgetTypeRole) return wtNone;
    if (role==Qt::SizeHintRole) {
        QFontMetrics fm(QApplication::font());
        if (col-1>=0 && col-1<colWidth.size()) return QSize(colWidth[col-1], fm.height()+2);
        else return QSize(colbasicWidth, fm.height()+2);
    }
    return QVariant();


}

QVariant QFFitFunctionValueInputTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role==Qt::DisplayRole) {
        if (orientation==Qt::Horizontal) {
            if (section==0) return QString("parameter");
            else if (section>0) {
                if (section-1<colNames.size()) return colNames[section-1];
            }
        } else if (orientation==Qt::Vertical) {

        }
    }
    if (role==Qt::SizeHintRole) {
        QFontMetrics fm(QApplication::font());
        if (section-1>=0 && section-1<colWidth.size()) return QSize(colWidth[section-1], fm.height()*2/3);
        else return QSize(colbasicWidth, fm.height()+2);
    }
    return QVariant();
}

bool QFFitFunctionValueInputTable::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int col=index.column();
    int row=index.row();
    if (col>0) {
        int cols=(col-1)/getColsPerRDR();
        int coli=(col-1)%getColsPerRDR();

        FitParam fp=fitparamids.value(row, FitParam());

        if (coli==0) {
            if (role==Qt::DisplayRole || role==Qt::EditRole) {
                if (value.canConvert(QVariant::Double)) {
                    setParameterValue(row, value.toDouble());
                    return true;
                }
            }
        } else if (coli==errorIdx && editerrors) {
            if (role==Qt::DisplayRole || role==Qt::EditRole) {
                if (value.canConvert(QVariant::Double)) {
                    setParameterError(row, value.toDouble());
                    return true;
                }
            }
        } else if (coli==rangeIdx && editRanges) {
            if (role==Qt::DisplayRole || role==Qt::EditRole) {
                if (value.canConvert(QVariant::Double)) {
                    setParameterMin(row, value.toDouble());
                    return true;
                }
            }
        } else if (coli==rangeIdx+1 && editRanges) {
            if (role==Qt::DisplayRole || role==Qt::EditRole) {
                if (value.canConvert(QVariant::Double)) {
                    setParameterMax(row, value.toDouble());
                    return true;
                }
            }
        } else if (coli==fixIdx && editfix) {
            if (role==Qt::CheckStateRole||role==Qt::DisplayRole||role==Qt::EditRole) {
                if (value.canConvert(QVariant::Bool)) {
                    setParameterFix(row, value.toBool());
                    return true;
                }
            }
        }
    }
    return false;

}

int QFFitFunctionValueInputTable::getColsPerRDR() const
{
    int cols=1;
    if (editRanges) cols+=2;
    if (editfix) cols+=1;
    if (editerrors) cols+=1;
    return cols;
}

void QFFitFunctionValueInputTable::setColBasicWidth(int w)
{
    colbasicWidth=w;
}

void QFFitFunctionValueInputTable::rebuildModel()
{
    checkRebuildModel(true);
}

bool QFFitFunctionValueInputTable::recalculateFitParameters(bool emitFitParamSignals, bool dataChangedSignals)
{
    bool changed=false;

    if (fitfunction) {
        // 1. store old parameters and error, store which parameters were visible
        // 2. recalculate parameters
        // 3. emit dataChanged() signals for all cells that have changed and set changed to true, if any changed

        QStringList fpids=fitfunction->getParameterIDs();
        double* p_old=(double*)calloc(fitfunction->paramCount(), sizeof(double));
        double* e_old=(double*)calloc(fitfunction->paramCount(), sizeof(double));
        for (int i=0; i<fpids.size(); i++) {
            p_old[i]=getParameterValue(fpids[i]);
            e_old[i]=getParameterError(fpids[i]);
        }
        double* p=duplicateArray(p_old, fitfunction->paramCount());
        double* e=duplicateArray(e_old, fitfunction->paramCount());


        fitfunction->calcParameter(p, e);
        for (int fi=0; fi<fpids.size(); fi++) {
            if (fitfunction->isParameterVisible(fi, p)) {
                int row=-1;
                int col=1;
                QModelIndex idx;
                QModelIndex eidx;
                if (fitParamListContainsID(fpids[fi], fitparamids, &row)) {
                    idx=index(row, 1);
                    eidx=index(row, errorIdx);
                }
                if (p_old[fi]!=p[fi]) {
                    changed=true;
                    emit dataChanged(idx, idx);
                }
                if (e_old[fi]!=e[fi]) {
                    changed=true;
                    emit dataChanged(eidx, eidx);
                }
            }
        }
        free(p);
        free(e);
        free(p_old);
        free(e_old);
    }


    if (changed && emitFitParamSignals) {
        emit fitParamChanged();
        emit fitParamErrorChanged();
    }
    return changed;
}

void QFFitFunctionValueInputTable::setEditRanges(bool enabled)
{
    editRanges=enabled;
    rebuildModel();
}

void QFFitFunctionValueInputTable::setDoRebuildModel(bool doReb)
{
    doRebuild=doReb;
    if (doReb) rebuildModel();
}

bool QFFitFunctionValueInputTable::checkRebuildModel(bool alwaysreset)
{
    if (!doRebuild) return false;
    QList<FitParam> fitparamids_new;
    bool ok=false;
    static int rebuildCount=0;
    QElapsedTimer t;
    //qDebug()<<"QFFitFunctionValueInputTable::checkRebuildModel(): "<<rebuildCount+1;
    rebuildCount++;
    t.start();

    colNames.clear();
    colWidth.clear();
    colNames.append(tr("value"));
    colWidth.append(colbasicWidth);
    errorIdx=-1;
    rangeIdx=-1;
    fixIdx=-1;
    if (editerrors) {
        colNames.append(tr("error"));
        colWidth.append(colbasicWidth*2/3);
        errorIdx=1;
    }
    if (editfix) {
        colNames.append(tr("fix"));
        colWidth.append(24);
        fixIdx=2;
    }
    if (editRanges) {
        colNames.append(tr("min"));
        colNames.append(tr("max"));
        colWidth.append(colbasicWidth/2);
        colWidth.append(colbasicWidth/2);
        errorIdx=3;
    }


    if (fitfunction) {
        double* p=(double*)calloc(fitfunction->paramCount(), sizeof(double));
        QStringList fpids=fitfunction->getParameterIDs();
        for (int i=0; i<fpids.size(); i++) {
            p[i]=getParameterValue(fpids[i]);
        }

        for (int fi=0; fi<fpids.size(); fi++) {
            if (fitfunction->isParameterVisible(fi, p)) {
                QFFitFunction::ParameterDescription desc=fitfunction->getDescription(fi);
                FitParam fprec;
                fprec.id=fpids[fi];
                fprec.label=desc.label;
                fprec.tooltip=desc.name;
                fprec.isFit=desc.fit;
                fprec.num=fi;
                int found=-1;
                if (!fitParamListContainsID(fprec.id, fitparamids_new, &found)) fitparamids_new<<fprec;
            }
        }
        free(p);
    } else {
        for (int fi=0; fi<rownames.size(); fi++) {
            FitParam fprec;
            fprec.id=rownames[fi];
            fprec.label=rowlabels.value(fi, fprec.id);
            fprec.tooltip=tr("fit parameter: %1").arg(fprec.label);
            fprec.isFit=true;
            fprec.num=fi;
            int found=-1;
            if (!fitParamListContainsID(fprec.id, fitparamids_new, &found)) fitparamids_new<<fprec;

        }
    }


    if (fitparamids_new!=fitparamids) {
        fitparamids=fitparamids_new;
        emit modelRebuilt();
        ok=true;
    }
    if (ok || alwaysreset) reset();
    //qDebug()<<"QFFitFunctionValueInputTable::checkRebuildModel() "<<rebuildCount<<": done "<<t.elapsed()<<"ms    =>"<<ok;

    return ok;
}



bool QFFitFunctionValueInputTable::fitParamListContainsID(const QString &id, const QList<QFFitFunctionValueInputTable::FitParam> &fitparamids, int* found)
{
    for (int i=0; i<fitparamids.size(); i++) {
        if (fitparamids[i].isValid() && fitparamids[i].id==id ) {
            if (found) *found=i;
            return true;
        }
    }
    return false;
}

bool QFFitFunctionValueInputTable::getParameterEditable(int row) const
{
    FitParam fp=fitparamids.value(row, FitParam());
    bool rangeval=true;
    if (fitfunction&&fitfunction->hasParameter(fp.id)) {
        rangeval=fitfunction->getDescription(fp.id).userEditable;
    }
    if (datamap && fp.isValid() && datamap->contains(fp.id)) {
        rangeval=(*datamap)[fp.id].editable;
    }
    return rangeval;
}
double QFFitFunctionValueInputTable::getParameterMin(int row) const
{
    FitParam fp=fitparamids.value(row, FitParam());
    double rangeval=-DBL_MAX;
    if (item && fp.isValid()) {
        rangeval=item->getFitMin(fp.id);
    } else {
        if (fitfunction&&fitfunction->hasParameter(fp.id)) {
            rangeval=fitfunction->getDescription(fp.id).minValue;
        }
        if (minvector) {
            rangeval=minvector->value(fp.num, rangeval);
        } else if (datamap && fp.isValid() && datamap->contains(fp.id)) {
            rangeval=(*datamap)[fp.id].min;
        }
    }
    return rangeval;
}

double QFFitFunctionValueInputTable::getParameterMax(int row) const
{
    FitParam fp=fitparamids.value(row, FitParam());
    double rangeval=DBL_MAX;
    if (item && fp.isValid()) {
        rangeval=item->getFitMax(fp.id);
    } else {
        if (fitfunction&&fitfunction->hasParameter(fp.id)) {
            rangeval=fitfunction->getDescription(fp.id).maxValue;
        }
        if (maxvector) {
            rangeval=maxvector->value(fp.num, rangeval);
        } else if (datamap && fp.isValid() && datamap->contains(fp.id)) {
            rangeval=(*datamap)[fp.id].max;
        }
    }
    return rangeval;
}



bool QFFitFunctionValueInputTable::getParameterFix(int row) const
{
    FitParam fp=fitparamids.value(row, FitParam());
    bool rangeval=false;
    if (item && fp.isValid()) {
        rangeval=item->getFitFix(fp.id);
    } else {
        if (fitfunction&&fitfunction->hasParameter(fp.id)) {
            rangeval=fitfunction->getDescription(fp.id).initialFix;
        }
        if (fixvector) {
            rangeval=fixvector->value(fp.num, rangeval);
        } else if (datamap && fp.isValid() && datamap->contains(fp.id)) {
            rangeval=(*datamap)[fp.id].fix;
        }
    }
    return rangeval;
}

double QFFitFunctionValueInputTable::getParameterValue(const QString &id) const
{
    double rangeval=0;
    if (item) {
        rangeval=item->getFitValue(id);
    } else {
        if (fitfunction&&fitfunction->hasParameter(id)) {
            rangeval=fitfunction->getDescription(id).initialValue;
        }
        if (datamap && datamap->contains(id)) {
            rangeval=(*datamap)[id].value;
        }
    }
    return rangeval;
}

double QFFitFunctionValueInputTable::getParameterError(const QString &id) const
{
    double rangeval=0;
    if (item) {
        rangeval=item->getFitError(id);
    } else {
        if (datamap && datamap->contains(id)) {
            rangeval=(*datamap)[id].error;
        }
    }
    return rangeval;
}

double QFFitFunctionValueInputTable::getParameterValue(int row) const
{
    FitParam fp=fitparamids.value(row, FitParam());
    double rangeval=0;
    if (item && fp.isValid()) {
        rangeval=item->getFitValue(fp.id);
    } else {
        if (fitfunction&&fitfunction->hasParameter(fp.id)) {
            rangeval=fitfunction->getDescription(fp.id).initialValue;
        }
        if (datavector) {
            rangeval=datavector->value(fp.num, rangeval);
        } else if (datamap && fp.isValid() && datamap->contains(fp.id)) {
            rangeval=(*datamap)[fp.id].value;
        }
    }
    return rangeval;
}

double QFFitFunctionValueInputTable::getParameterError(int row) const {
    FitParam fp=fitparamids.value(row, FitParam());
    double rangeval=0;
    if (item && fp.isValid()) {
        rangeval=item->getFitError(fp.id);
    } else {
        if (errorvector) {
            rangeval=errorvector->value(fp.num, rangeval);
        } else if (datamap && fp.isValid() && datamap->contains(fp.id)) {
            rangeval=(*datamap)[fp.id].error;
        }
    }
    return rangeval;
}

void QFFitFunctionValueInputTable::setParameterMin(int row, double value)
{
    FitParam fp=fitparamids.value(row, FitParam());
    if (item && fp.isValid()) {
        item->setFitMin(fp.id, value);
    } else {
        if (minvector) {
            if (fp.num>=minvector->size()) minvector->resize(fp.num+1);
            (*minvector)[fp.num]=value;
        }
        if (datamap && fp.isValid() && datamap->contains(fp.id)) {
            (*datamap)[fp.id].min=value;
        }
    }
    if (rangeIdx>=0) emit dataChanged(index(row, rangeIdx), index(row, rangeIdx));
    if (!checkRebuildModel(false)) {
        recalculateFitParameters(false);
    }
    emit fitParamChanged();
}

void QFFitFunctionValueInputTable::setParameterMax(int row, double value)
{
    FitParam fp=fitparamids.value(row, FitParam());
    if (item && fp.isValid()) {
        item->setFitMax(fp.id, value);
    } else {
        if (maxvector) {
            if (fp.num>=maxvector->size()) maxvector->resize(fp.num+1);
            (*maxvector)[fp.num]=value;
        }
        if (datamap && fp.isValid() && datamap->contains(fp.id)) {
            (*datamap)[fp.id].max=value;
        }
    }
    if (rangeIdx>=0) emit dataChanged(index(row, rangeIdx+1), index(row, rangeIdx+1));
    if (!checkRebuildModel(false)) {
        recalculateFitParameters(false);
    }
    emit fitParamChanged();
}

void QFFitFunctionValueInputTable::setParameterValue(int row, double value)
{
    FitParam fp=fitparamids.value(row, FitParam());
    if (item && fp.isValid()) {
        item->setFitValue(fp.id, value);
    } else {
        if (datavector) {
            if (fp.num>=datavector->size()) datavector->resize(fp.num+1);
            (*datavector)[fp.num]=value;
        }
        if (datamap && fp.isValid() && datamap->contains(fp.id)) {
            (*datamap)[fp.id].value=value;
        }
    }

    emit dataChanged(index(row, 1), index(row, 1));
    if (!checkRebuildModel(false)) {
        recalculateFitParameters(false);
    }
    emit fitParamChanged();
}

void QFFitFunctionValueInputTable::setParameterError(int row, double value)
{
    FitParam fp=fitparamids.value(row, FitParam());
    if (item && fp.isValid()) {
        item->setFitError(fp.id, value);
    } else {
        if (errorvector) {
            if (fp.num>=errorvector->size()) errorvector->resize(fp.num+1);
            (*errorvector)[fp.num]=value;
        }
        if (datamap && fp.isValid() && datamap->contains(fp.id)) {
            (*datamap)[fp.id].error=value;
        }
    }
    if (errorIdx>=0) emit dataChanged(index(row, errorIdx), index(row, errorIdx));
    if (!checkRebuildModel(false)) {

        recalculateFitParameters(false);
    }
    emit fitParamChanged();
}

void QFFitFunctionValueInputTable::setParameterFix(int row, bool value)
{
    FitParam fp=fitparamids.value(row, FitParam());
    if (item && fp.isValid()) {
        item->setFitFix(fp.id, value);
    } else {
        if (fixvector) {
            if (fp.num>=fixvector->size()) fixvector->resize(fp.num+1);
            (*fixvector)[fp.num]=value;
        }
        if (datamap && fp.isValid() && datamap->contains(fp.id)) {
            (*datamap)[fp.id].fix=value;
        }
    }
    if (fixIdx>=0) emit dataChanged(index(row, fixIdx), index(row, fixIdx));
    if (!checkRebuildModel(false)) {
        recalculateFitParameters(false);
    }
    emit fitParamChanged();
}

QFFitFunctionValueInputTable::FitParam::FitParam()
{
    num=-1;
    id="";
    isFit=false;
    label="";
    tooltip="";
}

bool QFFitFunctionValueInputTable::FitParam::isValid() const
{
    return !id.isEmpty();
}

bool QFFitFunctionValueInputTable::FitParam::operator==(const QFFitFunctionValueInputTable::FitParam &other) const
{
    return (id==other.id)&&(label==other.label);
}

QFFitFunctionValueInputTableFitParamData::QFFitFunctionValueInputTableFitParamData()
{
    value=0;
    error=0;
    fix=false;
    min=-DBL_MAX;
    max=DBL_MAX;
    editable=true;

}

QFFitFunctionValueInputTableFitParamData::QFFitFunctionValueInputTableFitParamData(double value, double error, bool fix, double min, double max, bool editable)
{
    this->value=value;
    this->error=error;
    this->fix=fix;
    this->min=min;
    this->max=max;
    this->editable=editable;
}
