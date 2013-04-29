#include "qfimfccsparameterinputdelegate.h"
#include "qfimfccsparameterinputtable.h"
#include "qfevaluationitem.h"
#include "qfproject.h"
#include "qfrawdatarecord.h"
#include "qfpluginservices.h"
#include "qfimfccsfitevaluation_item.h"
#include "qfdoubleedit.h"
#include "qfautooutputtimer.h"

QFImFCCSParameterInputDelegate::QFImFCCSParameterInputDelegate(QObject *parent) :
    QFHTMLDelegate(parent)
{
    //rxHTML=QRegExp("(<\\s*\\w+.*>)|(<\\s*/\\s*\\w+\\s*>)|(\\&\\w+\\;)");
    //rxHTML.setMinimal(true);
}

void QFImFCCSParameterInputDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //QFAutoOutputTimer t(QString("QFImFCCSParameterInputDelegate::paint(%1, %2, [%3] %4)").arg(index.row()).arg(index.column()).arg(index.data().typeName()).arg(index.data().toString()));
    QFHTMLDelegate::paint(painter, option, index);
/*    //static int htmlCount=0;
    if (index.data().toString().contains(rxHTML)) {
        QFAutoOutputTimer t(QString("QFImFCCSParameterInputDelegate::paint_html(%1, %2)").arg(index.row()).arg(index.column()));

        //QElapsedTimer t;
        //t.start();
        QFHTMLDelegate::paint(painter, option, index);
        //qDebug()<<htmlCount+1<<". call to QFHTMLDelegate::paint() "<<t.elapsed()<<" ms";
        //htmlCount++;
    } else {
        QFAutoOutputTimer t(QString("QFImFCCSParameterInputDelegate::paint_styled(%1, %2)").arg(index.row()).arg(index.column()));
        QStyledItemDelegate::paint(painter, option, index);
    }
    */
}

QSize QFImFCCSParameterInputDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //QFAutoOutputTimer t(QString("QFImFCCSParameterInputDelegate::sizeHint(%1, %2)").arg(index.row()).arg(index.column()));
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


QWidget *QFImFCCSParameterInputDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //QFAutoOutputTimer ta(QString("QFImFCCSParameterInputDelegate::createEditor(%1, %2)").arg(index.row()).arg(index.column()));
    QElapsedTimer t;
    t.start();
    QVariant widgetType=index.data(QFImFCCSParameterInputTable::widgetTypeRole);
    QVariant rdrID=index.data(QFImFCCSParameterInputTable::rdrIDRole);
    QVariant ffID=index.data(QFImFCCSParameterInputTable::fitFunctionIDRole);
    QVariant fpID=index.data(QFImFCCSParameterInputTable::fitParameterIDRole);
    QVariant eID=index.data(QFImFCCSParameterInputTable::evaluationIDRole);
    QVariant fileID=index.data(QFImFCCSParameterInputTable::fitFileIDRole);
    QFProject* project=QFPluginServices::getInstance()->getCurrentProject();

    //qDebug()<<"createEditor: project="<<project<<" eID="<<eID<<" widgetType="<<widgetType<<" fileID="<<fileID;
    if (project && eID.isValid() && widgetType.isValid() && fileID.isValid()) {
        QFEvaluationItem* eval=project->getEvaluationByID(eID.toInt());
        QFImFCCSFitEvaluationItem* imfccs=qobject_cast<QFImFCCSFitEvaluationItem*>(eval);
        QFRawDataRecord* rdr=NULL;
        QFFitFunction* ff=imfccs->getFitFunctionForID(ffID.toString());
        if (rdrID.isValid()) rdr=project->getRawDataByID(rdrID.toInt());
        //qDebug()<<"             imfccs="<<imfccs<<"   fpID="<<fpID<<"   ffID="<<ffID<<"  rdrID="<<rdrID<<"  rdr="<<rdr;
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtRDRCombobox) {
            QComboBox* cmb=new QComboBox(parent);
            QList<QPointer<QFRawDataRecord> > sl=imfccs->getApplicableRecords();
            for (int i=0; i<sl.size(); i++) {
                if (sl[i]) cmb->addItem(sl[i]->getSmallIcon(), sl[i]->getName(), sl[i]->getID());
            }
            //qDebug()<<"   created rdrcombo "<<t.elapsed()<<"ms";

            cmb->view()->setMinimumWidth(450);
            connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(onEditorFinished()));
            return cmb;
        }
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtFitFunctionCombobox) {
            QComboBox* cmb=new QComboBox(parent);
            QStringList sl=imfccs->getAvailableFitFunctions();
            for (int i=0; i<sl.size(); i++) {
                cmb->addItem(QIcon(":/lib/fitfunc_icon.png"), imfccs->getFitFunctionForID(sl[i])->name(), sl[i]);
            }

            //qDebug()<<"   created ffcombo "<<t.elapsed()<<"ms";
            cmb->view()->setMinimumWidth(450);
            connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(onEditorFinished()));
            return cmb;
        }
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtGlobalParamCombo) {
            QComboBox* cmb=new QComboBox(parent);
            int cnt=imfccs->getLinkParameterCount();
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
        /*if (imfccs && fpID.isValid() && widgetType.toInt()==QFImFCCSParameterInputTable::wtFixCheck) {
            QCheckBox* chk=new QCheckBox(parent);
            chk->setText("");
            //qDebug()<<"   created fixcheck";
            return chk;
        }*/
        if (imfccs && fpID.isValid() && widgetType.toInt()==QFImFCCSParameterInputTable::wtValueDoubleEdit) {
            QFDoubleEdit* edt=new QFDoubleEdit(parent);
            edt->setRange(imfccs->getFitMin(fpID.toString(), rdr), imfccs->getFitMax(fpID.toString(), rdr));
            //qDebug()<<"   created valuedoubleedit "<<t.elapsed()<<"ms";
            return edt;
        }
        if (imfccs && fpID.isValid() && widgetType.toInt()==QFImFCCSParameterInputTable::wtErrorEdit) {
            QFDoubleEdit* edt=new QFDoubleEdit(parent);
            //qDebug()<<"   created erroredit "<<t.elapsed()<<"ms";
            return edt;
        }
        if (imfccs && fpID.isValid() && widgetType.toInt()==QFImFCCSParameterInputTable::wtValueIntEdit) {
            QSpinBox* edt=new QSpinBox(parent);
            edt->setRange(imfccs->getFitMin(fpID.toString(), rdr), imfccs->getFitMax(fpID.toString(), rdr));
            //qDebug()<<"   created valuedoubleedit "<<t.elapsed()<<"ms";
            return edt;
        }
        if (ff && imfccs && fpID.isValid() && widgetType.toInt()==QFImFCCSParameterInputTable::wtValueComboBox) {
            QComboBox* edt=new QComboBox(parent);
            connect(edt, SIGNAL(currentIndexChanged(int)), this, SLOT(onEditorFinished()));
            QFFitFunction::ParameterDescription desc=ff->getDescription(fpID.toString());
            int cnt=0;
            for (int i=imfccs->getFitMin(fpID.toString(), rdr); i<=imfccs->getFitMax(fpID.toString(), rdr); i++) {
                edt->addItem(desc.comboItems.value(cnt, QString::number(i)), i);
                cnt++;
            }
            //edt->setRange(imfccs->getFitMin(fpID.toString(), rdr), imfccs->getFitMax(fpID.toString(), rdr));
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

void QFImFCCSParameterInputDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    //QFAutoOutputTimer ta(QString("QFImFCCSParameterInputDelegate::setEditorData(%1, %2)").arg(index.row()).arg(index.column()));
    QVariant widgetType=index.data(QFImFCCSParameterInputTable::widgetTypeRole);
    QVariant rdrID=index.data(QFImFCCSParameterInputTable::rdrIDRole);
    QVariant ffID=index.data(QFImFCCSParameterInputTable::fitFunctionIDRole);
    QVariant fpID=index.data(QFImFCCSParameterInputTable::fitParameterIDRole);
    QVariant eID=index.data(QFImFCCSParameterInputTable::evaluationIDRole);
    QVariant fileID=index.data(QFImFCCSParameterInputTable::fitFileIDRole);
    QFProject* project=QFPluginServices::getInstance()->getCurrentProject();

    //qDebug()<<"setEditorData: project="<<project<<" eID="<<eID<<" widgetType="<<widgetType<<" fileID="<<fileID<<"  index="<<index;
    if (project && eID.isValid() && widgetType.isValid() && fileID.isValid()) {
        QFEvaluationItem* eval=project->getEvaluationByID(eID.toInt());
        QFImFCCSFitEvaluationItem* imfccs=qobject_cast<QFImFCCSFitEvaluationItem*>(eval);
        QFRawDataRecord* rdr=NULL;
        if (rdrID.isValid()) rdr=project->getRawDataByID(rdrID.toInt());
        //qDebug()<<"   rdr="<<rdr<<"   eval="<<eval<<"   imfccs="<<imfccs<<"   fpID="<<fpID<<"   ffID="<<ffID;

        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtRDRCombobox) {
            QComboBox* cmb=qobject_cast<QComboBox*>(editor);
            if (cmb) {
                if (rdrID.isValid()) cmb->setCurrentIndex(cmb->findData(rdrID.toInt()));
                return;
            }
        }
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtFitFunctionCombobox) {
            QComboBox* cmb=qobject_cast<QComboBox*>(editor);
            if (cmb) {
                if (ffID.isValid()) cmb->setCurrentIndex(cmb->findData(ffID.toString()));
                return;
            }
        }
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtGlobalParamCombo) {
            QComboBox* cmb=qobject_cast<QComboBox*>(editor);
            if (cmb) {
                bool ok=false;
                int idx=index.data(QFImFCCSParameterInputTable::globalParamRole).toInt(&ok);
                if (!ok || idx<-1) idx=-1;
                cmb->setCurrentIndex(idx);
                return;
            }
        }
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtFixCheck) {
            QCheckBox* edt=qobject_cast<QCheckBox*>(editor);
            if (edt) {
                edt->setChecked(imfccs->getFitFix(fpID.toString(), rdr));
                return;
            }
        }
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtValueDoubleEdit) {
            QFDoubleEdit* edt=qobject_cast<QFDoubleEdit*>(editor);
            if (edt) {
                edt->setValue(imfccs->getFitValue(fpID.toString(), rdr));
                return;
            }
        }
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtValueIntEdit) {
            QSpinBox* edt=qobject_cast<QSpinBox*>(editor);
            if (edt) {
                edt->setValue(imfccs->getFitValue(fpID.toString(), rdr));
                return;
            }
        }
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtValueComboBox) {
            QComboBox* edt=qobject_cast<QComboBox*>(editor);
            if (edt) {
                edt->setCurrentIndex(edt->findData((int)imfccs->getFitValue(fpID.toString(), rdr)));
                return;
            }
        }
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtErrorEdit) {
            QFDoubleEdit* edt=qobject_cast<QFDoubleEdit*>(editor);
            if (edt) {
                edt->setValue(imfccs->getFitError(fpID.toString(), rdr));
                return;
            }
        }
    }
    QFHTMLDelegate::setEditorData(editor, index);
}

void QFImFCCSParameterInputDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    //QFAutoOutputTimer ta(QString("QFImFCCSParameterInputDelegate::setModelData(%1, %2)").arg(index.row()).arg(index.column()));
    QVariant widgetType=index.data(QFImFCCSParameterInputTable::widgetTypeRole);
    QVariant rdrID=index.data(QFImFCCSParameterInputTable::rdrIDRole);
    QVariant ffID=index.data(QFImFCCSParameterInputTable::fitFunctionIDRole);
    QVariant fpID=index.data(QFImFCCSParameterInputTable::fitParameterIDRole);
    QVariant eID=index.data(QFImFCCSParameterInputTable::evaluationIDRole);
    QVariant fileID=index.data(QFImFCCSParameterInputTable::fitFileIDRole);
    QFProject* project=QFPluginServices::getInstance()->getCurrentProject();

    //qDebug()<<"setModelData: project="<<project<<" eID="<<eID<<" widgetType="<<widgetType<<" fileID="<<fileID<<"  index="<<index;
    if (project && eID.isValid() && widgetType.isValid() && fileID.isValid()) {
        QFEvaluationItem* eval=project->getEvaluationByID(eID.toInt());
        QFImFCCSFitEvaluationItem* imfccs=qobject_cast<QFImFCCSFitEvaluationItem*>(eval);
        QFRawDataRecord* rdr=NULL;
        if (rdrID.isValid()) rdr=project->getRawDataByID(rdrID.toInt());

        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtRDRCombobox) {
            QComboBox* cmb=qobject_cast<QComboBox*>(editor);
            if (cmb) {
                //imfccs->setFitFile(fileID.toInt(), project->getRawDataByID(cmb->itemData(cmb->currentIndex()).toInt()));
                model->setData(index, cmb->itemData(cmb->currentIndex()).toInt());
                return;
            }
        }
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtFitFunctionCombobox) {
            QComboBox* cmb=qobject_cast<QComboBox*>(editor);
            if (cmb) {
                model->setData(index, cmb->itemData(cmb->currentIndex()).toString());
                //imfccs->setFitFunction(fileID.toInt(), project->getRawDataByID(cmb->itemData(cmb->currentIndex()).toString()));
                return;
            }
        }
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtGlobalParamCombo) {
            QComboBox* cmb=qobject_cast<QComboBox*>(editor);
            if (cmb) {
                //qDebug()<<"globalParam="<<cmb->currentIndex()-1;
                int g=cmb->currentIndex()-1;
                if (g<-1) g=-1;
                model->setData(index, g);
                return;
            }
        }
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtFixCheck) {
            QCheckBox* edt=qobject_cast<QCheckBox*>(editor);
            if (edt) {
                model->setData(index, edt->isChecked());
                return;
            }
        }
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtValueDoubleEdit) {
            QFDoubleEdit* edt=qobject_cast<QFDoubleEdit*>(editor);
            if (edt) {
                model->setData(index, edt->value());
                return;
            }
        }
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtValueIntEdit) {
            QSpinBox* edt=qobject_cast<QSpinBox*>(editor);
            if (edt) {
                model->setData(index, edt->value());
                return;
            }
        }
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtValueComboBox) {
            QComboBox* edt=qobject_cast<QComboBox*>(editor);
            if (edt) {
                model->setData(index, edt->itemData(edt->currentIndex()).toDouble());
                return;
            }
        }
        if (imfccs && widgetType.toInt()==QFImFCCSParameterInputTable::wtErrorEdit) {
            QFDoubleEdit* edt=qobject_cast<QFDoubleEdit*>(editor);
            if (edt) {
                model->setData(index, edt->value());
                return;
            }
        }
    }
    QFHTMLDelegate::setModelData(editor, model, index);
}

void QFImFCCSParameterInputDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //QFAutoOutputTimer ta(QString("QFImFCCSParameterInputDelegate::updateEditorGeometry(%1, %2)").arg(index.row()).arg(index.column()));
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

void QFImFCCSParameterInputDelegate::onEditorFinished()
{
    QWidget * widEditor = static_cast<QWidget *>(sender());
    if (widEditor) {
        emit commitData(widEditor);
        emit closeEditor(widEditor);
    }
}
