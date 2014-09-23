/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate: 2014-09-02 10:24:20 +0200 (Di, 02 Sep 2014) $  (revision $Rev: 3422 $)

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

#include "dlgsetrdrpropertybyexpression.h"
#include "ui_dlgsetrdrpropertybyexpression.h"
#include "programoptions.h"
#include "qfmathparser.h"

bool DlgSetRDRPropertyByExpression_SaveWidgetFunction(const QWidget*wid, QSettings& set, const QString& prefix) {
    const DlgSetRDRPropertyByExpression* w=qobject_cast<const DlgSetRDRPropertyByExpression*>(wid);
    if (w) {
        set.setValue(prefix+"editors", w->getEditorCount());
        return true;
    }
    return false;
}

bool DlgSetRDRPropertyByExpression_LoadWidgetFunction(QWidget*wid, const QSettings& set, const QString& prefix) {
    DlgSetRDRPropertyByExpression* w=qobject_cast<DlgSetRDRPropertyByExpression*>(wid);
    if (w) {
        w->setEditorCount(set.value(prefix+"editors", w->getEditorCount()).toInt());
        return true;
    }
    return false;
}

bool DlgSetRDRPropertyByExpression_EqualsWidgetFunction(const QWidget*wid, const QSettings& set, const QString& prefix) {
    const DlgSetRDRPropertyByExpression* w=qobject_cast<const DlgSetRDRPropertyByExpression*>(wid);
    if (w) {
        return set.value(prefix+"editors", w->getEditorCount()).toInt()==w->getEditorCount();
    }
    return false;
}

bool DlgSetRDRPropertyByExpression_RegisterWidgetFunction(QWidget*wid, QConfigComboboxWidget* config) {
    DlgSetRDRPropertyByExpression* w=qobject_cast<DlgSetRDRPropertyByExpression*>(wid);
    if (w) {
        QObject::connect(w, SIGNAL(widgetContentsChanged()), config, SLOT(widgetContentsChanged()));
        return true;
    }
    return false;
}

bool DlgSetRDRPropertyByExpression_UnregisterWidgetFunction(QWidget*wid, QConfigComboboxWidget* config) {
    DlgSetRDRPropertyByExpression* w=qobject_cast<DlgSetRDRPropertyByExpression*>(wid);
    if (w) {
        QObject::disconnect(w, SIGNAL(widgetContentsChanged()), config, SLOT(widgetContentsChanged()));
        return true;
    }
    return false;
}






void fProperty(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
    QFRawDataRecord * rdr=(QFRawDataRecord *)p->getGeneraldata("currentRDR", NULL).toULongLong();

    bool ok=true;
    if (n<1 || n>2) { ok=false; p->qfmpError(QObject::tr("rdrproperty(name,default) accepts 1, 2 or 3 arguments")); }
    if (n>0 && (params[0].type!=qfmpString)) { ok=false; p->qfmpError(QObject::tr("rdrproperty(name,default) needs a string as first argument")); }
    QString prop;
    if (n>0) prop=params[0].str;
    QVariant defaultVal=QVariant();
    if (n>1) defaultVal=params[1].asVariant();
    if (ok && rdr) {
        r.setVariant(rdr->getQFProperty(prop, defaultVal));
    } else {
        r.setInvalid();
    }
}

void fFile(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
    QFRawDataRecord * rdr=(QFRawDataRecord *)p->getGeneraldata("currentRDR", NULL).toULongLong();

    bool ok=true;
    if (n!=1) { ok=false; p->qfmpError(QObject::tr("rdrfile(id) accepts 1 argument")); }
    if (n>0 && (params[0].type!=qfmpString)&& (!params[0].isUInt())) { ok=false; p->qfmpError(QObject::tr("rdrfile(id) needs a string or an unsigned integer as first argument")); }
    int id=-1;
    QString ids;
    if (n>0 && params[0].isUInt()) id=params[0].toInteger();
    else if (n>0 && params[0].type==qfmpString) ids=params[0].str;
    if (ok && rdr) {
        if (id>=0) r.setString(rdr->getFileName(id));
        else r.setString(rdr->getFileForType(ids));
    } else {
        r.setInvalid();
    }
}

void fFileType(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
    QFRawDataRecord * rdr=(QFRawDataRecord *)p->getGeneraldata("currentRDR", NULL).toULongLong();

    bool ok=true;
    if (n!=1) { ok=false; p->qfmpError(QObject::tr("rdrfiletype(id) accepts 1 argument")); }
    if (n>0 && (!params[0].isUInt())) { ok=false; p->qfmpError(QObject::tr("rdrfiletype(id) needs an unsigned integer as first argument")); }
    int id=-1;
    if (n>0 && params[0].isUInt()) id=params[0].toInteger();
    if (ok && rdr) {
        if (id>=0) r.setString(rdr->getFileType(id));
        else r.setString("");
    } else {
        r.setInvalid();
    }
}

void fFiles(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
    QFRawDataRecord * rdr=(QFRawDataRecord *)p->getGeneraldata("currentRDR", NULL).toULongLong();

    bool ok=true;
    if (n!=1) { ok=false; p->qfmpError(QObject::tr("rdrfiles(id) accepts 1 argument")); }
    if (n>0 && (params[0].type!=qfmpString)&& (!params[0].isUInt())) { ok=false; p->qfmpError(QObject::tr("rdrfiles(id) needs a string or an unsigned integer as first argument")); }
    int id=-1;
    QString ids;
    if (n>0 && params[0].isUInt()) id=params[0].toInteger();
    else if (n>0 && params[0].type==qfmpString) ids=params[0].str;
    if (ok && rdr) {
        if (id>=0) r.setString(rdr->getFileName(id));
        else r.setStringVec(rdr->getFilesForType(ids));
    } else {
        r.setInvalid();
    }
}



DlgSetRDRPropertyByExpression::DlgSetRDRPropertyByExpression(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgSetRDRPropertyByExpression)
{

    mp.addFunction("rdrproperty", fProperty);
    mp.addFunction("rdrfile", fFile);
    mp.addFunction("rdrfiletype", fFileType);
    mp.addFunction("rdrfiles", fFiles);

    lastExpression="";
    lastNode=NULL;

    ui->setupUi(this);
    layItems=new QVBoxLayout();
    layItems->addStretch();
    ui->scrollContents->setLayout(layItems);

    ui->widConfig->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/setrdrpropbyexpression.ini");

    QConfigComboboxWidget::WidgetFunctions f;
    f.equalsWidget=DlgSetRDRPropertyByExpression_EqualsWidgetFunction;
    f.loadWidget=DlgSetRDRPropertyByExpression_LoadWidgetFunction;
    f.registerWidget=DlgSetRDRPropertyByExpression_RegisterWidgetFunction;
    f.saveWidget=DlgSetRDRPropertyByExpression_SaveWidgetFunction;
    f.unregisterWidget=DlgSetRDRPropertyByExpression_UnregisterWidgetFunction;
    f.widget=this;
    ui->widConfig->registerWidget("main", f);

    on_btnAdd_clicked();

    ProgramOptions::getConfigWindowGeometry(this, "DlgSetRDRPropertyByExpression/window");
    ProgramOptions::getConfigQSplitter(ui->splitter, "DlgSetRDRPropertyByExpression/splitter");
}

DlgSetRDRPropertyByExpression::~DlgSetRDRPropertyByExpression()
{
    ui->widConfig->unregisterWidgets();
    ProgramOptions::setConfigWindowGeometry(this, "DlgSetRDRPropertyByExpression/window");
    ProgramOptions::setConfigQSplitter(ui->splitter, "DlgSetRDRPropertyByExpression/splitter");
    delete ui;
}

void DlgSetRDRPropertyByExpression::setProject(QFProject *project)
{
    this->project=project;

    if (project) {
        for (int i=0; i<editors.size(); i++) {
            if (editors[i]) {
                editors[i]->setProject(project);
            }
        }
        rdrs.clear();
        for (int i=0; i<project->getRawDataCount(); i++) {
            QPointer<QFRawDataRecord> r=project->getRawDataByNum(i);
            rdrs.append(r);
            QListWidgetItem* item=new QListWidgetItem(r->getName(), ui->lstRDR);
            item->setCheckState(Qt::Checked);
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
            item->setData(Qt::UserRole, r->getID());
            item->setIcon(r->getSmallIcon());
            ui->lstRDR->addItem(item);
        }
        ui->lstRDR->setCurrentRow(0);
    }

}

QList<QPointer<QFRawDataRecord> > DlgSetRDRPropertyByExpression::getSelectedRDRs() const
{
    QList<QPointer<QFRawDataRecord> > l;
    for (int i=0; i<ui->lstRDR->count(); i++)  {
        if (ui->lstRDR->item(i)->checkState()==Qt::Checked) {
            l<<rdrs[i];
        }
    }
    return l;
}



QVariant DlgSetRDRPropertyByExpression::getResult(const QString& expression, QFRawDataRecord *rdr, bool* apply, QString *error)
{
    if (apply) *apply=false;
    QFMathParser::qfmpNode* n=NULL;
    if (lastNode && lastExpression==expression) {
        n=lastNode;
    } else {
        mp.resetErrors();
        n=mp.parse(expression);
        if (n && !mp.hasErrorOccured()) {
            lastNode=n;
            lastExpression=expression;
        }
    }
    QVariant res;
    if (n) {
        if (rdr) {
            mp.addVariableString("name", rdr->getName());
            mp.addVariableString("description", rdr->getDescription());
            mp.addVariableString("group", rdr->getGroupName());
            mp.addVariableDouble("groupid", rdr->getGroup());
            mp.addVariableString("role", rdr->getRole());
            mp.addVariableString("folder", rdr->getFolder());
            mp.addVariableStringVector("files", rdr->getFiles());
            mp.addVariableDouble("filecount", rdr->getFilesCount());
            mp.setGeneralData("currentRDR", (uint64_t)rdr);
        }

        mp.resetErrors();
        res=n->evaluate().asVariant();
        if (mp.hasErrorOccured()) {
            if (error) *error=mp.getLastErrors().join("\n");
        } else {
            if (apply) *apply=res.isValid();
        }

    }
    //qDebug()<<"getResult("<<expression<<rdr->getName()<<"): "<<res;
    return res;

}

void DlgSetRDRPropertyByExpression::applyResult(DlgSetRDRPropertyByExpressionEditor* wid, QFRawDataRecord *rdr)
{
    bool apply=false;
    QVariant res=getResult(wid->getExpression(), rdr, &apply);
    if (apply) {
        switch (wid->getType()) {
            case 0: // RDR group
                rdr->setGroup(project->addOrFindRDRGroup(res.toString()));
                break;
            case 1: // role
                rdr->setRole(res.toString());
                break;
            case 2: // folder
                rdr->setFolder(res.toString());
                break;
            case 3: // name
                rdr->setName(res.toString());
                break;
            case 4: // property name
                rdr->setQFProperty(wid->getPropertyName(), res);
                break;
        }
    }
}

QFRawDataRecord *DlgSetRDRPropertyByExpression::currentRDR() const
{
    int idx=ui->lstRDR->currentRow();
    //qDebug()<<"curRow: "<<idx;
    if (idx<0) idx=0;
    //qDebug()<<"curRow_checked: "<<idx<<rdrs.value(idx, NULL);
    //if (rdrs.value(idx, NULL)) qDebug()<<"   "<<rdrs.value(idx, NULL)->getName();
    return rdrs.value(idx, NULL);
}

int DlgSetRDRPropertyByExpression::getEditorCount() const
{
    return editors.size();
}

void DlgSetRDRPropertyByExpression::setEditorCount(int cnt)
{
    if (cnt>getEditorCount()) {
        while (getEditorCount()<cnt) {
            addEditor();
        }
    } else if (cnt<getEditorCount()) {
        while (getEditorCount()>cnt) {
            removeEditor();
        }
    }
}

void DlgSetRDRPropertyByExpression::addEditor()
{
    DlgSetRDRPropertyByExpressionEditor* edt=new DlgSetRDRPropertyByExpressionEditor(this, this);
    layItems->insertWidget(layItems->count()-1, edt);
    ui->widConfig->registerWidget(QString("editor%1").arg(editors.size()), edt);
    edt->setID(QString("editor%1").arg(editors.size()));
    edt->setProject(project);
    connect(edt, SIGNAL(removeMe(DlgSetRDRPropertyByExpressionEditor*)), this, SLOT(removeEditor(DlgSetRDRPropertyByExpressionEditor*)));
    editors.append(edt);
}

void DlgSetRDRPropertyByExpression::removeEditor()
{
    DlgSetRDRPropertyByExpressionEditor* edt=editors.last();
    if (editors.contains(edt)) {
        editors.removeAll(edt);
        layItems->removeWidget(edt);
        ui->widConfig->unregisterWidget(edt);
        edt->hide();
        disconnect(edt, SIGNAL(removeMe(DlgSetRDRPropertyByExpressionEditor*)), this, SLOT(removeEditor(DlgSetRDRPropertyByExpressionEditor*)));
        edt->deleteLater();
        for (int i=0; i<editors.size(); i++) {
            if (editors[i]) {
                ui->widConfig->setEditorName(editors[i], QString("editor%1").arg(i));
                editors[i]->setID(QString("editor%1").arg(i));
            }
        }
    }
}

QList<DlgSetRDRPropertyByExpressionEditor *> DlgSetRDRPropertyByExpression::getEditors() const
{
    return editors;
}

void DlgSetRDRPropertyByExpression::on_btnSelectNone_clicked()
{
    for (int i=0; i<ui->lstRDR->count(); i++)  {
        ui->lstRDR->item(i)->setCheckState(Qt::Unchecked);
    }
}

void DlgSetRDRPropertyByExpression::on_btnSelectAll_clicked()
{
    for (int i=0; i<ui->lstRDR->count(); i++)  {
        ui->lstRDR->item(i)->setCheckState(Qt::Checked);
    }
}

void DlgSetRDRPropertyByExpression::on_btnAdd_clicked()
{
    addEditor();
    emitWidgetContentsChanged();
}

void DlgSetRDRPropertyByExpression::showHelp()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("dlgsetrdrpropertybyexpression.html");

}

void DlgSetRDRPropertyByExpression::removeEditor(DlgSetRDRPropertyByExpressionEditor *edt)
{
    if (editors.contains(edt)) {
        layItems->removeWidget(edt);
        editors.removeAll(edt);
        ui->widConfig->unregisterWidget(edt);
        edt->hide();
        disconnect(edt, SIGNAL(removeMe(DlgSetRDRPropertyByExpressionEditor*)), this, SLOT(removeEditor(DlgSetRDRPropertyByExpressionEditor*)));
        edt->deleteLater();
        for (int i=0; i<editors.size(); i++) {
            if (editors[i]) {
                ui->widConfig->setEditorName(editors[i], QString("editor%1").arg(i));
                editors[i]->setID(QString("editor%1").arg(i));
            }
        }
        emitWidgetContentsChanged();
    }
}

void DlgSetRDRPropertyByExpression::emitWidgetContentsChanged()
{
    emit widgetContentsChanged();
}

void DlgSetRDRPropertyByExpression::updateTest()
{
    for (int i=0; i<editors.size(); i++) {
        if (editors[i]) {
            editors[i]->updateTest();
        }
    }
}


