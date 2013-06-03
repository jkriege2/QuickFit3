#include "qffccsparameterinputdelegate.h"
#include "qffccsparameterinputtable.h"
#include "qfevaluationitem.h"
#include "qfproject.h"
#include "qfrawdatarecord.h"
#include "qfpluginservices.h"
#include "qffccsfitevaluation_item.h"
#include "qfdoubleedit.h"
#include "qfautooutputtimer.h"

QFFCCSParameterInputDelegate::QFFCCSParameterInputDelegate(QObject *parent) :
    QFHTMLDelegate(parent)
{
    //rxHTML=QRegExp("(<\\s*\\w+.*>)|(<\\s*/\\s*\\w+\\s*>)|(\\&\\w+\\;)");
    //rxHTML.setMinimal(true);
}

void QFFCCSParameterInputDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //QFAutoOutputTimer t(QString("QFFCCSParameterInputDelegate::paint(%1, %2, [%3] %4)").arg(index.row()).arg(index.column()).arg(index.data().typeName()).arg(index.data().toString()));
    QFHTMLDelegate::paint(painter, option, index);
/*    //static int htmlCount=0;
    if (index.data().toString().contains(rxHTML)) {
        QFAutoOutputTimer t(QString("QFFCCSParameterInputDelegate::paint_html(%1, %2)").arg(index.row()).arg(index.column()));

        //QElapsedTimer t;
        //t.start();
        QFHTMLDelegate::paint(painter, option, index);
        //qDebug()<<htmlCount+1<<". call to QFHTMLDelegate::paint() "<<t.elapsed()<<" ms";
        //htmlCount++;
    } else {
        QFAutoOutputTimer t(QString("QFFCCSParameterInputDelegate::paint_styled(%1, %2)").arg(index.row()).arg(index.column()));
        QStyledItemDelegate::paint(painter, option, index);
    }
    */
}

QSize QFFCCSParameterInputDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant widgetType=index.data(QFFCCSParameterInputTable::widgetTypeRole);
    if (widgetType.toInt()==QFFCCSParameterInputTable::wtFitFunctionCombobox || widgetType.toInt()==QFFCCSParameterInputTable::wtRDRCombobox) return QSize();
    //QFAutoOutputTimer t(QString("QFFCCSParameterInputDelegate::sizeHint(%1, %2)").arg(index.row()).arg(index.column()));
    return QFHTMLDelegate::sizeHint(option, index);
    /*
    static int htmlCount=0;
    static int styleCount=0;
    if (index.data().toString().contains(rxHTML)) {

        QElapsedTimer t;
        t.start();
        QSize s=QFHTMLDelegate::sizeHint(option, index);
        //qDebug()<<htmlCount+1<<". call to QFHTMLDelegate::sizeHint() "<<t.elapsed()<<" ms";
        htmlCount++;
        return s;
    } else {
        QElapsedTimer t;
        t.start();
        QSize s= QStyledItemDelegate::sizeHint(option, index);
        //qDebug()<<styleCount+1<<". call to QStyledItemDelegate::sizeHint() "<<t.elapsed()<<" ms";
        styleCount++;
        return s;
    }
    */
}


QWidget *QFFCCSParameterInputDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //QFAutoOutputTimer ta(QString("QFFCCSParameterInputDelegate::createEditor(%1, %2)").arg(index.row()).arg(index.column()));
    QElapsedTimer t;
    t.start();
    QVariant widgetType=index.data(QFFCCSParameterInputTable::widgetTypeRole);
    QVariant rdrID=index.data(QFFCCSParameterInputTable::rdrIDRole);
    QVariant ffID=index.data(QFFCCSParameterInputTable::fitFunctionIDRole);
    QVariant fpID=index.data(QFFCCSParameterInputTable::fitParameterIDRole);
    QVariant eID=index.data(QFFCCSParameterInputTable::evaluationIDRole);
    QVariant fileID=index.data(QFFCCSParameterInputTable::fitFileIDRole);
    QFProject* project=QFPluginServices::getInstance()->getCurrentProject();

    //qDebug()<<"createEditor: project="<<project<<" eID="<<eID<<" widgetType="<<widgetType<<" fileID="<<fileID;
    if (project && eID.isValid() && widgetType.isValid() && fileID.isValid()) {
        QFEvaluationItem* eval=project->getEvaluationByID(eID.toInt());
        QFFCCSFitEvaluationItem* FCCS=qobject_cast<QFFCCSFitEvaluationItem*>(eval);
        QFRawDataRecord* rdr=NULL;
        QFFitFunction* ff=FCCS->getFitFunctionForID(ffID.toString());
        if (rdrID.isValid()) rdr=project->getRawDataByID(rdrID.toInt());
        //qDebug()<<"             FCCS="<<FCCS<<"   fpID="<<fpID<<"   ffID="<<ffID<<"  rdrID="<<rdrID<<"  rdr="<<rdr;
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtRDRCombobox) {
            QComboBox* cmb=new QComboBox(parent);
            QList<QPointer<QFRawDataRecord> > sl=FCCS->getApplicableRecords();
            for (int i=0; i<sl.size(); i++) {
                if (sl[i]) cmb->addItem(sl[i]->getSmallIcon(), sl[i]->getName(), sl[i]->getID());
            }
            //qDebug()<<"   created rdrcombo "<<t.elapsed()<<"ms";

            cmb->view()->setMinimumWidth(450);
            connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(onEditorFinished()));
            return cmb;
        }
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtFitFunctionCombobox) {
            QComboBox* cmb=new QComboBox(parent);
            QStringList sl=FCCS->getAvailableFitFunctions();
            for (int i=0; i<sl.size(); i++) {
                cmb->addItem(QIcon(":/lib/fitfunc_icon.png"), FCCS->getFitFunctionForID(sl[i])->name(), sl[i]);
            }

            //qDebug()<<"   created ffcombo "<<t.elapsed()<<"ms";
            cmb->view()->setMinimumWidth(450);
            connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(onEditorFinished()));
            return cmb;
        }
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtGlobalParamCombo) {
            QComboBox* cmb=new QComboBox(parent);
            int cnt=FCCS->getLinkParameterCount();
            cmb->addItem(QIcon(":/lib/clear.png"), tr("-- not linked --").arg(cnt), -1);
            //cmb->setItemData(0, Q, Qt::ForegroundRole);
            for (int i=0; i<cnt; i++) {
                cmb->addItem(tr("global param #%1").arg(i), i);
                cmb->setItemData(1+i, QBrush(getCycleColor(i, 10, 0.5, 0.8)), Qt::BackgroundRole);
            }
            cmb->addItem(QIcon(":/lib/add.png"), tr("new global param #%1").arg(cnt), cnt);
            cmb->setItemData(cnt, QBrush(getCycleColor(cnt, 10, 0.5, 0.8)), Qt::BackgroundRole);
            cmb->view()->setMinimumWidth(250);
            //qDebug()<<"   created ffcombo "<<t.elapsed()<<"ms";
            //cmb->view()->setMinimumWidth(450);
            connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(onEditorFinished()));
            return cmb;
        }
        /*if (FCCS && fpID.isValid() && widgetType.toInt()==QFFCCSParameterInputTable::wtFixCheck) {
            QCheckBox* chk=new QCheckBox(parent);
            chk->setText("");
            //qDebug()<<"   created fixcheck";
            return chk;
        }*/
        if (FCCS && fpID.isValid() && (widgetType.toInt()==QFFCCSParameterInputTable::wtValueDoubleEdit || widgetType.toInt()==QFFCCSParameterInputTable::wtValueLogDoubleEdit || widgetType.toInt()==QFFCCSParameterInputTable::wtRangeEditMin || widgetType.toInt()==QFFCCSParameterInputTable::wtRangeEditMax)) {
            QFDoubleEdit* edt=new QFDoubleEdit(parent);
            if (widgetType.toInt()==QFFCCSParameterInputTable::wtValueLogDoubleEdit) {
                edt->setLogScale(true, 10);
            }
            if (widgetType.toInt()==QFFCCSParameterInputTable::wtRangeEditMin || widgetType.toInt()==QFFCCSParameterInputTable::wtRangeEditMax) {
                edt->setCheckBounds(false, false);
            } else {
                edt->setRange(FCCS->getFitMin(fpID.toString(), rdr), FCCS->getFitMax(fpID.toString(), rdr));
            }
            //qDebug()<<"   created valuedoubleedit "<<t.elapsed()<<"ms";
            return edt;
        }
        if (FCCS && fpID.isValid() && widgetType.toInt()==QFFCCSParameterInputTable::wtErrorEdit) {
            QFDoubleEdit* edt=new QFDoubleEdit(parent);
            //qDebug()<<"   created erroredit "<<t.elapsed()<<"ms";
            return edt;
        }
        if (FCCS && fpID.isValid() && widgetType.toInt()==QFFCCSParameterInputTable::wtValueIntEdit) {
            QSpinBox* edt=new QSpinBox(parent);
            edt->setRange(FCCS->getFitMin(fpID.toString(), rdr), FCCS->getFitMax(fpID.toString(), rdr));
            //qDebug()<<"   created valuedoubleedit "<<t.elapsed()<<"ms";
            return edt;
        }
        if (ff && FCCS && fpID.isValid() && widgetType.toInt()==QFFCCSParameterInputTable::wtValueComboBox) {
            QComboBox* edt=new QComboBox(parent);
            connect(edt, SIGNAL(currentIndexChanged(int)), this, SLOT(onEditorFinished()));
            QFFitFunction::ParameterDescription desc=ff->getDescription(fpID.toString());
            int cnt=0;
            for (int i=FCCS->getFitMin(fpID.toString(), rdr); i<=FCCS->getFitMax(fpID.toString(), rdr); i++) {
                edt->addItem(desc.comboItems.value(cnt, QString::number(i)), i);
                cnt++;
            }
            //edt->setRange(FCCS->getFitMin(fpID.toString(), rdr), FCCS->getFitMax(fpID.toString(), rdr));
            //qDebug()<<"   created valuedoubleedit "<<t.elapsed()<<"ms";
            return edt;
        }
        //qDebug()<<"   created NOTHING "<<t.elapsed()<<"ms";
        return NULL;
    }
    //return QFHTMLDelegate::createEditor(parent, option, index);
    //qDebug()<<"   created NOTHING "<<t.elapsed()<<"ms";
    return NULL;
}

void QFFCCSParameterInputDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    //QFAutoOutputTimer ta(QString("QFFCCSParameterInputDelegate::setEditorData(%1, %2)").arg(index.row()).arg(index.column()));
    QVariant widgetType=index.data(QFFCCSParameterInputTable::widgetTypeRole);
    QVariant rdrID=index.data(QFFCCSParameterInputTable::rdrIDRole);
    QVariant ffID=index.data(QFFCCSParameterInputTable::fitFunctionIDRole);
    QVariant fpID=index.data(QFFCCSParameterInputTable::fitParameterIDRole);
    QVariant eID=index.data(QFFCCSParameterInputTable::evaluationIDRole);
    QVariant fileID=index.data(QFFCCSParameterInputTable::fitFileIDRole);
    QFProject* project=QFPluginServices::getInstance()->getCurrentProject();

    //qDebug()<<"setEditorData: project="<<project<<" eID="<<eID<<" widgetType="<<widgetType<<" fileID="<<fileID<<"  index="<<index;
    if (project && eID.isValid() && widgetType.isValid() && fileID.isValid()) {
        QFEvaluationItem* eval=project->getEvaluationByID(eID.toInt());
        QFFCCSFitEvaluationItem* FCCS=qobject_cast<QFFCCSFitEvaluationItem*>(eval);
        QFRawDataRecord* rdr=NULL;
        if (rdrID.isValid()) rdr=project->getRawDataByID(rdrID.toInt());
        //qDebug()<<"   rdr="<<rdr<<"   eval="<<eval<<"   FCCS="<<FCCS<<"   fpID="<<fpID<<"   ffID="<<ffID;

        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtRDRCombobox) {
            QComboBox* cmb=qobject_cast<QComboBox*>(editor);
            if (cmb) {
                if (rdrID.isValid()) cmb->setCurrentIndex(cmb->findData(rdrID.toInt()));
                return;
            }
        }
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtFitFunctionCombobox) {
            QComboBox* cmb=qobject_cast<QComboBox*>(editor);
            if (cmb) {
                if (ffID.isValid()) cmb->setCurrentIndex(cmb->findData(ffID.toString()));
                return;
            }
        }
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtGlobalParamCombo) {
            QComboBox* cmb=qobject_cast<QComboBox*>(editor);
            if (cmb) {
                bool ok=false;
                int idx=index.data(QFFCCSParameterInputTable::globalParamRole).toInt(&ok);
                if (!ok || idx<-1) idx=-1;
                cmb->setCurrentIndex(idx);
                return;
            }
        }
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtFixCheck) {
            QCheckBox* edt=qobject_cast<QCheckBox*>(editor);
            if (edt) {
                edt->setChecked(FCCS->getFitFix(fpID.toString(), rdr));
                return;
            }
        }
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtValueDoubleEdit) {
            QFDoubleEdit* edt=qobject_cast<QFDoubleEdit*>(editor);
            if (edt) {
                edt->setValue(FCCS->getFitValue(fpID.toString(), rdr));
                return;
            }
        }
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtRangeEditMin) {
            QFDoubleEdit* edt=qobject_cast<QFDoubleEdit*>(editor);
            if (edt) {
                edt->setValue(FCCS->getFitMin(fpID.toString(), rdr));
                return;
            }
        }
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtRangeEditMax) {
            QFDoubleEdit* edt=qobject_cast<QFDoubleEdit*>(editor);
            if (edt) {
                edt->setValue(FCCS->getFitMax(fpID.toString(), rdr));
                return;
            }
        }
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtValueIntEdit) {
            QSpinBox* edt=qobject_cast<QSpinBox*>(editor);
            if (edt) {
                edt->setValue(FCCS->getFitValue(fpID.toString(), rdr));
                return;
            }
        }
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtValueComboBox) {
            QComboBox* edt=qobject_cast<QComboBox*>(editor);
            if (edt) {
                edt->setCurrentIndex(edt->findData((int)FCCS->getFitValue(fpID.toString(), rdr)));
                return;
            }
        }
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtErrorEdit) {
            QFDoubleEdit* edt=qobject_cast<QFDoubleEdit*>(editor);
            if (edt) {
                edt->setValue(FCCS->getFitError(fpID.toString(), rdr));
                return;
            }
        }
    }
    QFHTMLDelegate::setEditorData(editor, index);
}

void QFFCCSParameterInputDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    //QFAutoOutputTimer ta(QString("QFFCCSParameterInputDelegate::setModelData(%1, %2)").arg(index.row()).arg(index.column()));
    QVariant widgetType=index.data(QFFCCSParameterInputTable::widgetTypeRole);
    QVariant rdrID=index.data(QFFCCSParameterInputTable::rdrIDRole);
    QVariant ffID=index.data(QFFCCSParameterInputTable::fitFunctionIDRole);
    QVariant fpID=index.data(QFFCCSParameterInputTable::fitParameterIDRole);
    QVariant eID=index.data(QFFCCSParameterInputTable::evaluationIDRole);
    QVariant fileID=index.data(QFFCCSParameterInputTable::fitFileIDRole);
    QFProject* project=QFPluginServices::getInstance()->getCurrentProject();

    //qDebug()<<"setModelData: project="<<project<<" eID="<<eID<<" widgetType="<<widgetType<<" fileID="<<fileID<<"  index="<<index;
    if (project && eID.isValid() && widgetType.isValid() && fileID.isValid()) {
        QFEvaluationItem* eval=project->getEvaluationByID(eID.toInt());
        QFFCCSFitEvaluationItem* FCCS=qobject_cast<QFFCCSFitEvaluationItem*>(eval);
        QFRawDataRecord* rdr=NULL;
        if (rdrID.isValid()) rdr=project->getRawDataByID(rdrID.toInt());

        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtRDRCombobox) {
            QComboBox* cmb=qobject_cast<QComboBox*>(editor);
            if (cmb) {
                //FCCS->setFitFile(fileID.toInt(), project->getRawDataByID(cmb->itemData(cmb->currentIndex()).toInt()));
                model->setData(index, cmb->itemData(cmb->currentIndex()).toInt());
                return;
            }
        }
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtFitFunctionCombobox) {
            QComboBox* cmb=qobject_cast<QComboBox*>(editor);
            if (cmb) {
                model->setData(index, cmb->itemData(cmb->currentIndex()).toString());
                //FCCS->setFitFunction(fileID.toInt(), project->getRawDataByID(cmb->itemData(cmb->currentIndex()).toString()));
                return;
            }
        }
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtGlobalParamCombo) {
            QComboBox* cmb=qobject_cast<QComboBox*>(editor);
            if (cmb) {
                //qDebug()<<"globalParam="<<cmb->currentIndex()-1;
                int g=cmb->currentIndex()-1;
                if (g<-1) g=-1;
                model->setData(index, g);
                return;
            }
        }
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtFixCheck) {
            QCheckBox* edt=qobject_cast<QCheckBox*>(editor);
            if (edt) {
                model->setData(index, edt->isChecked());
                return;
            }
        }
        if (FCCS && (widgetType.toInt()==QFFCCSParameterInputTable::wtValueDoubleEdit || widgetType.toInt()==QFFCCSParameterInputTable::wtRangeEditMin || widgetType.toInt()==QFFCCSParameterInputTable::wtRangeEditMax)) {
            QFDoubleEdit* edt=qobject_cast<QFDoubleEdit*>(editor);
            if (edt) {
                model->setData(index, edt->value());
                return;
            }
        }

        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtValueIntEdit) {
            QSpinBox* edt=qobject_cast<QSpinBox*>(editor);
            if (edt) {
                model->setData(index, edt->value());
                return;
            }
        }
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtValueComboBox) {
            QComboBox* edt=qobject_cast<QComboBox*>(editor);
            if (edt) {
                model->setData(index, edt->itemData(edt->currentIndex()).toDouble());
                return;
            }
        }
        if (FCCS && widgetType.toInt()==QFFCCSParameterInputTable::wtErrorEdit) {
            QFDoubleEdit* edt=qobject_cast<QFDoubleEdit*>(editor);
            if (edt) {
                model->setData(index, edt->value());
                return;
            }
        }
    }
    QFHTMLDelegate::setModelData(editor, model, index);
}

void QFFCCSParameterInputDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //QFAutoOutputTimer ta(QString("QFFCCSParameterInputDelegate::updateEditorGeometry(%1, %2)").arg(index.row()).arg(index.column()));
    QPoint offset(0,0);
    /*QComboBox *cmb=qobject_cast<QComboBox*>(editor);
    QCheckBox *chk=qobject_cast<QCheckBox*>(editor);
    if (cmb) {
        editor->setGeometry(QRect(offset+option.rect.topLeft(), QSize(option.rect.width()-offset.x(), option.rect.height())));
    } else if (chk) {
        editor->setGeometry(QRect(offset+option.rect.topLeft(), QSize(option.rect.width()-offset.x(), option.rect.height())));
    } else {
        QFHTMLDelegate::updateEditorGeometry(editor, option, index);
    }*/
    editor->setGeometry(QRect(offset+option.rect.topLeft(), QSize(option.rect.width()-offset.x(), option.rect.height())));
    //QFHTMLDelegate::updateEditorGeometry(editor, option, index);
}

void QFFCCSParameterInputDelegate::onEditorFinished()
{
    QWidget * widEditor = static_cast<QWidget *>(sender());
    if (widEditor) {
        emit commitData(widEditor);
        emit closeEditor(widEditor);
    }
}
