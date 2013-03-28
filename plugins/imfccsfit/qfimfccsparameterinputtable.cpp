#include "qfimfccsparameterinputtable.h"
#include "qfimfccsfitevaluation_item.h"
#include "qfautooutputtimer.h"

QFImFCCSParameterInputTable::QFImFCCSParameterInputTable(QFImFCCSFitEvaluationItem *parent) :
    QAbstractTableModel(parent)
{
    this->item=parent;
    colsPerRDR=5;
}

int QFImFCCSParameterInputTable::rowCount(const QModelIndex &parent) const
{
    return 2+fitparamids.size();
}

int QFImFCCSParameterInputTable::columnCount(const QModelIndex &parent) const
{
    return 1+colsPerRDR*item->getNumberOfFitFiles();
}

Qt::ItemFlags QFImFCCSParameterInputTable::flags(const QModelIndex &index) const
{
    //QFAutoOutputTimer(QString("flags(%1, %2, %3)").arg(index.row()).arg(index.column()));
    Qt::ItemFlags f=Qt::ItemIsEnabled|Qt::ItemIsSelectable;
    int col=index.column();
    int row=index.row();
    if (col>0) {
        int cols=(col-1)/colsPerRDR;
        int coli=(col-1)%colsPerRDR;
        if (row>1 || coli==0) f=f|Qt::ItemIsEditable;
        int s=(col-1)/colsPerRDR;
        int i=(col-1)%colsPerRDR;
        QFRawDataRecord* rdr=item->getFitFile(cols);
        QFFitFunction* ff=item->getFitFunction(cols);
        if (i==3) {
            FitParam fp=fitparamids.value(row-2, FitParam());
            if (fp.isValid() && ff->hasParameter(fp.id) &&ff->getDescription(fp.id).fit) {
                f=f|Qt::ItemIsUserCheckable;
            }
        }else if (i==1) {
            FitParam fp=fitparamids.value(row-2, FitParam());
            if (fp.isValid() && ff->hasParameter(fp.id) &&ff->getDescription(fp.id).displayError==QFFitFunction::EditError) {
                f=f|Qt::ItemIsEditable;
            }
        }
    }
    return f;
}

QVariant QFImFCCSParameterInputTable::data(const QModelIndex &index, int role) const
{
    //QFAutoOutputTimer(QString("data(%1, %2, %3)").arg(index.row()).arg(index.column()).arg(role));
    int col=index.column();
    int row=index.row();

    if (role==evaluationIDRole) return item->getID();


    if (col==0) {
        if (row==0) {
            if (role==Qt::DisplayRole || role==Qt::EditRole) return tr("file");
        } else if (row==1) {
            if (role==Qt::DisplayRole || role==Qt::EditRole) return tr("fit model");
        } else if (row>1) {
            if (role==Qt::DisplayRole || role==Qt::EditRole) return fitparamids.value(row-2, QFImFCCSParameterInputTable::FitParam()).label;
        }

        if (role==Qt::FontRole) {
            QFont f=QApplication::font();
            f.setBold(true);
            return f;
        }
    } else if (col>0) {
        int cols=(col-1)/colsPerRDR;
        int coli=(col-1)%colsPerRDR;
        if (role==fitFileIDRole) return cols;
        QFRawDataRecord* rdr=item->getFitFile(cols);
        QFFitFunction* ff=item->getFitFunction(cols);
        if (role==rdrIDRole) {
            if (rdr) return rdr->getID();
            else return QVariant();
        }
        if (role==fitFunctionIDRole) {
            if (ff) return ff->id();
            else return QVariant();
        }
        if (row==0) {
            if (rdr && coli==0) {
                if (role==Qt::DisplayRole || role==Qt::EditRole) return rdr->getName();
                if (role==Qt::DecorationRole) return rdr->getSmallIcon();
                if (role==widgetTypeRole) return wtRDRCombobox;
            }
        } else if (row==1) {
            if (ff && coli==0) {
                if (role==Qt::DisplayRole || role==Qt::EditRole) return ff->name();
                if (role==Qt::DecorationRole) return QIcon(":/lib/fitfunc_icon.png");
                if (role==widgetTypeRole) return wtFitFunctionCombobox;
            }
        } else if (row>1) {
            FitParam fp=fitparamids.value(row-2, FitParam());
            if (role==fitParameterIDRole) {
                if (fp.isValid()) return fp.id;
                else return QVariant();
            }
            if (role==Qt::ToolTipRole) {
                if (fp.isValid()) return fp.tooltip;
                else return QVariant();
            }
            //double* p=item->allocFillParameters(rdr, item->getCurrentIndex(), ff);
            if (fp.isValid() && ff && ff->hasParameter(fp.id) /*&& ff->isParameterVisible(ff->getParameterNum(fp.id), p)*/) {
                QFFitFunction::ParameterDescription desc=ff->getDescription(fp.id);
                /*double* p=item->allocFillParameters(rdr, item->getCurrentIndex(), ff);
                bool visible=ff->isParameterVisible(ff->getParameterNum(fp.id), p);
                free(p);
                if (visible) {*/
                    if (coli==0) {
                        if (role==Qt::DisplayRole || role==Qt::EditRole) return item->getFitValue(fp.id, rdr);
                        if (role==widgetTypeRole && desc.userEditable) {
                            if (desc.type==QFFitFunction::FloatNumber) return wtValueDoubleEdit;
                            if (desc.type==QFFitFunction::IntNumber) return wtValueIntEdit;
                            if (desc.type==QFFitFunction::IntCombo) return wtValueComboBox;
                        }
                        if (role==Qt::BackgroundRole && !desc.userEditable) return QBrush(QApplication::palette().color(QPalette::Window));
                    } if (coli==1) {
                        if ((role==Qt::DisplayRole || role==Qt::EditRole)  && desc.displayError!=QFFitFunction::NoError) return item->getFitError(fp.id, rdr);
                        if (role==widgetTypeRole) {
                            if (desc.displayError==QFFitFunction::EditError) return wtErrorEdit;
                        }
                        if (role==Qt::BackgroundRole && desc.displayError!=QFFitFunction::EditError) return QBrush(QApplication::palette().color(QPalette::Window));
                    } if (coli==2) {
                        if (role==Qt::DisplayRole || role==Qt::EditRole) return desc.unitLabel;
                    } if (coli==3) {
                        if (role==Qt::CheckStateRole && desc.fit) {
                            if (item->getFitFix(fp.id, rdr)) return Qt::Checked;
                            return Qt::Unchecked;
                        }
                    }
                //}
            } else {
                if (role==Qt::BackgroundRole) return QBrush(QApplication::palette().color(QPalette::Window).darker());
            }

        }
    }
    if (role==widgetTypeRole) return wtNone;
    return QVariant();
}

QVariant QFImFCCSParameterInputTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role==Qt::DisplayRole) {
        if (orientation==Qt::Horizontal) {
            if (section==0) return QString("parameter");
            else if (section>0) {
                int s=(section-1)/colsPerRDR;
                int i=(section-1)%colsPerRDR;
                if (i==0) return tr("value");
                if (i==1) return tr("error");
                if (i==3) return tr("fix");
                if (i==4) return tr("global");
            }
        } else if (orientation==Qt::Vertical) {
            /*if (section==0) return QString("file");
            else if (section==1) return QString("model");
            else if (section>1) {
                return fitparamids;
            }*/
        }
    }
    return QVariant();
}

bool QFImFCCSParameterInputTable::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int col=index.column();
    int row=index.row();

    //QFAutoOutputTimer(QString("setData(%1, %2, %3, r=%4)").arg(index.row()).arg(index.column()).arg(value.toString()).arg(role));

    if (role==Qt::EditRole && col>0) {
        int cols=(col-1)/colsPerRDR;
        int coli=(col-1)%colsPerRDR;
        QFRawDataRecord* rdr=item->getFitFile(cols);
        QFFitFunction* ff=item->getFitFunction(cols);

        if (row==0 && coli==0) {
            QFRawDataRecord* newrdr=item->getProject()->getRawDataByID(value.toInt());
            if (newrdr) {
                item->setFitFile(cols, newrdr);
                emit dataChanged(index, index);
                return true;
            }
        } else if (row==1) {
            item->setFitFunction(cols, value.toString());
            if (!checkRebuildModel(false)) {
                emit dataChanged(index, index);
            }
            return true;
        } else if (row>1) {
            FitParam pid=fitparamids.value(row-2, FitParam());
            if (pid.isValid()) {
                if (coli==0) {
                    item->setFitValue(pid.id, value.toDouble(), rdr);
                    if (!checkRebuildModel(false)) {
                        emit dataChanged(index, index);
                    }
                    return true;
                }
                if (coli==1) {
                    item->setFitError(pid.id, value.toDouble(), rdr);
                    emit dataChanged(index, index);
                    return true;
                }
                if (coli==3) {
                    item->setFitFix(pid.id, value.toBool(), rdr);
                    emit dataChanged(index, index);
                    return true;
                }
            }
        }
    }

    if (role==Qt::CheckStateRole && col>0) {
        int cols=(col-1)/colsPerRDR;
        int coli=(col-1)%colsPerRDR;
        QFRawDataRecord* rdr=item->getFitFile(cols);

        if (row>1) {
            FitParam pid=fitparamids.value(row-2, FitParam());
            if (pid.isValid()) {
                if (coli==3) {
                    item->setFitFix(pid.id, value.toBool(), rdr);
                    emit dataChanged(index, index);
                    return true;
                }
            }
        }
    }

    // imfccs->setFitFile(fileID.toInt(), project->getRawDataByID(cmb->itemData(cmb->currentIndex()).toInt()));
    return false;
}

int QFImFCCSParameterInputTable::getColsPerRDR() const
{
    return colsPerRDR;
}

void QFImFCCSParameterInputTable::rebuildModel()
{
    checkRebuildModel(true);
}

bool QFImFCCSParameterInputTable::checkRebuildModel(bool alwaysreset)
{
    QList<FitParam> fitparamids_new;
    bool ok=false;
    static int rebuildCount=0;
    QElapsedTimer t;
    qDebug()<<"QFImFCCSParameterInputTable::checkRebuildModel(): "<<rebuildCount+1;
    rebuildCount++;
    t.start();

    for (int i=0; i<item->getNumberOfFitFiles(); i++) {
        QFRawDataRecord* rdr=item->getFitFile(i);
        QFFitFunction* ff=item->getFitFunction(i);
        //qDebug()<<"   file="<<i<<"   rdr="<<rdr<<"   ff="<<ff;
        if (rdr&&ff) {
            double* p=item->allocFillParameters(rdr, item->getCurrentIndex(), ff);
            QStringList fpids=ff->getParameterIDs();
            //qDebug()<<"   file="<<i<<"   rdr="<<rdr->getName()<<"   ff="<<ff->name()<<"   fpids="<<fpids;
            for (int fi=0; fi<fpids.size(); fi++) {
                if (ff->isParameterVisible(fi, p)) {
                    QFFitFunction::ParameterDescription desc=ff->getDescription(fi);
                    FitParam fprec;
                    fprec.id=fpids[fi];
                    fprec.label=desc.label;
                    fprec.tooltip=desc.name;
                    if (!fitParamListContainsID(fprec.id, fitparamids_new)) fitparamids_new<<fprec;
                    //qDebug()<<"   added "<<fprec.id<<"  "<<fprec.label;
                }
            }
            free(p);
        }
    }

    if (fitparamids_new!=fitparamids) {
        fitparamids=fitparamids_new;
        emit modelRebuilt();
        ok=true;
    }
    if (ok || alwaysreset) reset();
    qDebug()<<"QFImFCCSParameterInputTable::checkRebuildModel() "<<rebuildCount<<": done "<<t.elapsed()<<"ms    =>"<<ok;

    return ok;
}

bool QFImFCCSParameterInputTable::fitParamListContainsID(const QString &id, const QList<QFImFCCSParameterInputTable::FitParam> &fitparamids)
{
    for (int i=0; i<fitparamids.size(); i++) {
        if (fitparamids[i].isValid() && fitparamids[i].id==id ) return true;
    }
    return false;
}

QFImFCCSParameterInputTable::FitParam::FitParam()
{
    id="";
    label="";
    tooltip="";
}

bool QFImFCCSParameterInputTable::FitParam::isValid() const
{
    return !id.isEmpty();
}

bool QFImFCCSParameterInputTable::FitParam::operator==(const QFImFCCSParameterInputTable::FitParam &other) const
{
    return (id==other.id)&&(label==other.label);
}
