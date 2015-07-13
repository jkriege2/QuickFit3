/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfwizard.h"
#include "programoptions.h"
#include<QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif



QFWizard::QFWizard(QWidget *parent, const QString &config_prefix) :
    QWizard(parent)
{
    configPrefix=config_prefix;
    setWizardStyle(QWizard::ModernStyle);
    ProgramOptions::getConfigWindowGeometry(this, prefix+"wizard_geometry/");
}

void QFWizard::closeEvent(QCloseEvent *e)
{
    if (!configPrefix.isEmpty()) {
        ProgramOptions::setConfigWindowGeometry(this, prefix+"wizard_geometry/");
    }
    QWizard::closeEvent(e);
}


QFWizardPage::QFWizardPage(QWidget *parent):
    QWizardPage(parent)
{
    m_userLast=NULL;
    m_userValidatePage=NULL;
    m_userLastArg=NULL;
    m_userValidateArg=NULL;
    m_externalvalidate=false;
    m_isvalid=false;
}

QFWizardPage::QFWizardPage(const QString &title, QWidget *parent):
    QWizardPage(parent)
{
    m_userLast=NULL;
    m_userValidatePage=NULL;
    m_userLastArg=NULL;
    m_userValidateArg=NULL;
    m_externalvalidate=false;
    m_isvalid=false;
    setTitle(title);
}

void QFWizardPage::initializePage()
{
    QWizardPage::initializePage();
    emit onInitialize(this);
    emit onInitialize(this, m_userLast);
    emit onInitializeA(this, m_userLastArg);
}

bool QFWizardPage::validatePage()
{
    emit onValidate(this);
    emit onValidate(this, m_userValidatePage);
    emit onValidateA(this, m_userValidateArg);
    return QWizardPage::validatePage();
}

bool QFWizardPage::isComplete() const
{
    if (m_externalvalidate) {
        return m_isvalid;
    }
    return QWizardPage::isComplete();
}

void QFWizardPage::setUserPreviousPage(QWizardPage *page)
{
    m_userLast=page;
}

void QFWizardPage::setUserOnValidatePage(QWizardPage *page)
{
    m_userValidatePage=page;
}

void QFWizardPage::setUserPreviousArgument(void *page)
{
    m_userLastArg=page;
}

void QFWizardPage::setUserOnValidateArgument(void *page)
{
    m_userValidateArg=page;
}

void QFWizardPage::setExternalValidate(bool enabled)
{
    m_externalvalidate=enabled;
    emit completeChanged();
}

void QFWizardPage::setExternalIsValid(bool valid)
{
    m_isvalid=valid;
    emit completeChanged();
}


QFTextWizardPage::QFTextWizardPage(const QString& title, const QString &text, QWidget *parent):
    QFWizardPage(title, parent)
{
    QLabel *label = new QLabel(text, this);
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    setLayout(layout);
}



QFComboBoxWizardPage::QFComboBoxWizardPage(const QString &title, QWidget *parent):
    QFEnableableFormWizardPage(title, parent)
{
    setEnableable(false);
    label = new QLabel(this);
    combo=new QFEnhancedComboBox(this);
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
    addRow(label, combo);
}

void QFComboBoxWizardPage::setLabel(const QString &label)
{
    this->label->setText(label);
}

void QFComboBoxWizardPage::setItems(const QStringList &items, const QList<QIcon> &icons)
{
    combo->clear();
    for (int i=0; i<items.size(); i++) {
        combo->addItem(icons.value(i, QIcon()), items.value(i));
    }
}

void QFComboBoxWizardPage::setData(const QVariantList &data, int role)
{
    for (int i=0; i<data.size(); i++) {
        if (i<combo->count()) combo->setItemData(i, data.value(i, QVariant()), role);
    }
}

void QFComboBoxWizardPage::addItem(const QString &text, const QVariant &data)
{
    combo->addItem(text, data);
}

void QFComboBoxWizardPage::addItem(const QIcon &icon, const QString &text, const QVariant &data)
{
    combo->addItem(icon, text, data);
}

int QFComboBoxWizardPage::currentItem() const
{
    return combo->currentIndex();
}

void QFComboBoxWizardPage::setCurrentItem(int idx)
{
    combo->setCurrentIndex(idx);
}

QVariant QFComboBoxWizardPage::currentData(int role) const
{
    return combo->currentData(role);
}

void QFComboBoxWizardPage::setCurrentData(const QVariant &data, int role)
{
    combo->setCurrentData(data, role);
}

void QFComboBoxWizardPage::indexChanged(int index)
{
    emit currentIndexChanged(index);
}


QFFormWizardPage::QFFormWizardPage(QWidget *parent):
    QFWizardPage(parent)
{
    createWidgets();
}

QFFormWizardPage::QFFormWizardPage(const QString &title, QWidget *parent):
    QFWizardPage(title, parent)
{
    createWidgets();
}

void QFFormWizardPage::addRow(QWidget *label, QWidget *field)
{
    m_layout->addRow(label, field);
}

void QFFormWizardPage::addRow(QWidget *label, QLayout *field)
{
    m_layout->addRow(label, field);
}

void QFFormWizardPage::addRow(const QString &labelText, QWidget *field)
{
    m_layout->addRow(labelText, field);
}

void QFFormWizardPage::addRow(const QString &labelText, QLayout *field)
{
    m_layout->addRow(labelText, field);
}

void QFFormWizardPage::addRow(QWidget *widget)
{
    m_layout->addRow(widget);
}

void QFFormWizardPage::addRow(QLayout *layout)
{
    m_layout->addRow(layout);
}

void QFFormWizardPage::setRowEnabled(int row, bool enabled)
{
    if (row>=0 && row<m_layout->rowCount() && m_layout->itemAt(row, QFormLayout::FieldRole)) setWidgetEnabled(m_layout->itemAt(row, QFormLayout::FieldRole)->widget(), enabled);
}

void QFFormWizardPage::setWidgetEnabled(QWidget *wid, bool enabled)
{
    if (wid) {
        wid->setEnabled(enabled);
        QWidget* w2=m_layout->labelForField(wid);
        if (w2) {
            w2->setEnabled(enabled);
        }
    }
}

void QFFormWizardPage::setRowVisible(int row, bool enabled)
{
    if (row>=0 && row<m_layout->rowCount() && m_layout->itemAt(row, QFormLayout::FieldRole)) setWidgetVisible(m_layout->itemAt(row, QFormLayout::FieldRole)->widget(), enabled);
}

void QFFormWizardPage::setWidgetVisible(QWidget *wid, bool enabled)
{
    if (wid) {
        wid->setVisible(enabled);
        QWidget* w2=m_layout->labelForField(wid);
        if (w2) {
            w2->setVisible(enabled);
        }
    }
}


void QFFormWizardPage::createWidgets()
{
    m_layout = new QFormLayout;
    widMain=new QWidget(this);
    m_mainlay=new QVBoxLayout();

    widMain->setLayout(m_layout);
    widMain->setEnabled(true);
    m_mainlay->addWidget(widMain);
    setLayout(m_mainlay);
}


QFEnableableFormWizardPage::QFEnableableFormWizardPage(QWidget *parent):
    QFFormWizardPage(parent)
{
    nextIfDisabled=NULL;
    createWidgets();
}

QFEnableableFormWizardPage::QFEnableableFormWizardPage(const QString &title, QWidget *parent):
    QFFormWizardPage(title, parent)
{
    nextIfDisabled=NULL;
    createWidgets();
}

void QFEnableableFormWizardPage::setEnableCheckbox(const QString &name, bool enabled)
{
    chkMain->setText(name);
    chkMain->setChecked(enabled);
    widMain->setEnabled(enabled);
}

void QFEnableableFormWizardPage::setEnableable(bool enableable)
{
    if (enableable) {
        chkMain->setVisible(true);
        connect(chkMain, SIGNAL(toggled(bool)), widMain, SLOT(setEnabled(bool)));
        widMain->setEnabled(chkMain->isChecked());
    } else {
        chkMain->setVisible(false);
        disconnect(chkMain, SIGNAL(toggled(bool)), widMain, SLOT(setEnabled(bool)));
        widMain->setEnabled(true);
    }
}

void QFEnableableFormWizardPage::setNextPageIfDisabled(QWizardPage *nextIfDisabled)
{
    this->nextIfDisabled=nextIfDisabled;
}

int QFEnableableFormWizardPage::nextId() const
{
    if (nextIfDisabled && !widMain->isEnabled() && wizard()) {
        QList<int>	ids=wizard()->pageIds();
        for (int i=0; i<ids.size(); i++) {
            if (wizard()->page(ids[i])==nextIfDisabled) return ids[i];
        }
    }
    return QFFormWizardPage::nextId();
}

void QFEnableableFormWizardPage::createWidgets()
{
    chkMain=new QCheckBox(tr("enable"), this);
    m_mainlay->insertWidget(0, chkMain);
    chkMain->setChecked(false);
    widMain->setEnabled(false);
    setEnableable(true);
}


QFCheckboxListWizardPage::QFCheckboxListWizardPage(const QString &title, QWidget *parent):
    QFEnableableFormWizardPage(title, parent)
{
    setEnableable(false);
}

void QFCheckboxListWizardPage::setItems(const QStringList &items)
{
    clear();
    for (int i=0; i<items.size(); i++)  {
        addItem(items[i]);
    }
}

void QFCheckboxListWizardPage::clear()
{
    for (int i=0; i<boxes.size(); i++) {
        m_layout->removeWidget(boxes[i]);
        delete boxes[i];
    }
}

void QFCheckboxListWizardPage::addItem(const QString &item)
{
    QCheckBox* chk=new QCheckBox(item, this);
    boxes.append(chk);
    addRow("", chk);
}

void QFCheckboxListWizardPage::setChecked(const QList<bool> &checked)
{
    for (int i=0; i<boxes.size(); i++) {
        if (i<checked.size()) boxes[i]->setChecked(checked[i]);
    }
}

void QFCheckboxListWizardPage::setChecked(int id, bool checked)
{
    if (id<boxes.size() && id>=0) boxes[id]->setChecked(checked);
}

bool QFCheckboxListWizardPage::getChecked(int id) const
{
    if (id<boxes.size() && id>=0) return boxes[id]->isChecked();
    return false;
}

QList<bool> QFCheckboxListWizardPage::getChecked() const
{
    QList<bool> ch;
    for (int i=0; i<boxes.size(); i++) {
        ch<<boxes[i]->isChecked();
    }
    return ch;
}

int QFCheckboxListWizardPage::count() const
{
    return boxes.size();
}

void QFCheckboxListWizardPage::setNextPageIfAllDisabled(QWizardPage *nextIfDisabled)
{
    this->nextIfAllDisabled=nextIfDisabled;
}

int QFCheckboxListWizardPage::nextId() const
{
    QList<int>	ids=wizard()->pageIds();
    //qDebug()<<"QFCheckboxListWizardPage::nextId() "<<nextIfAllDisabled << widMain->isEnabled() << wizard();
    if (nextIfAllDisabled && widMain->isEnabled() && wizard()) {
        bool allDisabled=true;
        for (int i=0; i<count(); i++) {
            if (getChecked(i)) {
                allDisabled=false;
                break;
            }
        }
        //qDebug()<<"  allDisabled="<<allDisabled;
        if (allDisabled) {
            for (int i=0; i<ids.size(); i++) {
                if (wizard()->page(ids[i])==nextIfAllDisabled) return ids[i];
            }
        }
    }
    return QFEnableableFormWizardPage::nextId();

}


QFProcessingWizardPage::QFProcessingWizardPage(const QString &title, const QString &text, QWidget *parent):
    QFWizardPage(title, parent)
{
    QLabel *label = new QLabel(text, this);
    label->setWordWrap(true);

    mainLay = new QVBoxLayout;
    mainLay->addWidget(label);
    setLayout(mainLay);
    m_canceled=false;
    m_done=false;
    m_allowCancel=true;

    mainLay->addStretch();

    labMessage=new QLabel(this);
    labMessage->setAlignment(Qt::AlignHCenter);
    labMessage->setVisible(false);
    mainLay->addWidget(labMessage);

    progress=new QProgressBar;
    progress->setRange(0,100);
    progress->setValue(0);
    progress->setVisible(false);
    mainLay->addWidget(progress);

    mainLay->addStretch();

    addSubProgressWidgets();

    mainLay->addStretch();

    btnStart=new QPushButton(tr("START ..."));
    connect(btnStart, SIGNAL(clicked()), this, SLOT(startClicked()));
    mainLay->addWidget(btnStart);
    mainLay->addStretch();

    edtErrors=new QTextEdit(this);
    edtErrors->setVisible(false);
    mainLay->addWidget(edtErrors);
}

bool QFProcessingWizardPage::wasCanceled() const
{
    return m_canceled;
}

void QFProcessingWizardPage::setAllowCancel(bool enabled)
{
    m_allowCancel=enabled;
}


bool QFProcessingWizardPage::isComplete() const
{
    return m_done || m_canceled;
}

void QFProcessingWizardPage::setProcessingFinished(bool status)
{
    btnStart->setVisible(false);
    m_done=status;
    emit completeChanged();
    QApplication::processEvents();
}

void QFProcessingWizardPage::setProgress(int value)
{
    progress->setVisible(true);
    progress->setValue(value);
    QApplication::processEvents();
}

void QFProcessingWizardPage::setSubProgress(int value)
{
    progressSub[0]->setVisible(true);
    progressSub[0]->setValue(value);
    QApplication::processEvents();
}

void QFProcessingWizardPage::setSubProgress(int id, int value)
{
    if (id>=0) {
        while (id>=progressSub.size()) {
            addSubProgressWidgets();
        }
        progressSub[id]->setVisible(true);
        progressSub[id]->setValue(value);
        QApplication::processEvents();
    }

}

void QFProcessingWizardPage::setRange(int minimum, int maximum)
{
    progress->setVisible(true);
    progress->setRange(minimum, maximum);
    QApplication::processEvents();
}

void QFProcessingWizardPage::setSubRange(int id, int minimum, int maximum)
{
    if (id>=0) {
        while (id>=progressSub.size()) {
            addSubProgressWidgets();
        }
        progressSub[id]->setVisible(true);
        progressSub[id]->setRange(minimum, maximum);
        QApplication::processEvents();
    }

}

void QFProcessingWizardPage::setSubRange(int minimum, int maximum)
{
    progressSub[0]->setVisible(true);
    progressSub[0]->setRange(minimum, maximum);
    QApplication::processEvents();
}

void QFProcessingWizardPage::setMessage(const QString &text)
{
    labMessage->setVisible(true);
    labMessage->setText(text);
    QApplication::processEvents();
}

void QFProcessingWizardPage::setSubMessage(const QString &text)
{
    labSubMessage[0]->setVisible(true);
    labSubMessage[0]->setText(text);
    QApplication::processEvents();
}

void QFProcessingWizardPage::setSubMessage(int id, const QString &text)
{
    if (id>=0) {
        while (id>=progressSub.size()) {
            addSubProgressWidgets();
        }
        labSubMessage[id]->setVisible(true);
        labSubMessage[id]->setText(text);
        QApplication::processEvents();
    }
}

void QFProcessingWizardPage::cancelClicked()
{
    m_canceled=true;
    emit cancelProcessing();
    emit completeChanged();
}

void QFProcessingWizardPage::startClicked()
{
    edtErrors->setVisible(false);
    edtErrors->clear();
    QTextCursor c=edtErrors->textCursor();
    c.movePosition(QTextCursor::End);
    c.insertHtml("<font color=\"red\"><b>ERRORS:</b><br></font>");
    disconnect(btnStart, SIGNAL(clicked()), this, SLOT(startClicked()));
    btnStart->setText(tr("CANCEL ..."));
    connect(btnStart, SIGNAL(clicked()), this, SLOT(cancelClicked()));
    btnStart->setVisible(m_allowCancel);
    emit startProcessing();
    emit completeChanged();
}

void QFProcessingWizardPage::incProgress(int inc)
{
    setProgress(progress->value()+inc);
}

void QFProcessingWizardPage::incSubProgress(int inc)
{
    setSubProgress(progressSub[0]->value()+inc);
}

void QFProcessingWizardPage::addErrorMessage(const QString &message)
{
    edtErrors->setVisible(true);
    QTextCursor c=edtErrors->textCursor();
    c.movePosition(QTextCursor::End);
    c.insertHtml(QString("<br><font color=\"red\">%1</font>").arg(message));
    edtErrors->moveCursor(QTextCursor::End);
    QApplication::processEvents();
}

void QFProcessingWizardPage::addSubProgressWidgets()
{
    QLabel* l=new QLabel(this);
    l->setAlignment(Qt::AlignHCenter);
    l->setVisible(false);
    QFont f=l->font();
    f.setPointSize(f.pointSize()-1);
    l->setFont(f);

    QProgressBar* p=new QProgressBar;
    p->setRange(0,100);
    p->setValue(0);
    p->setVisible(false);
    p->setMaximumHeight(10);
    p->setFont(f);
    if (progressSub.size()<=0) {
        mainLay->addWidget(l);
        mainLay->addWidget(p);
    } else {
        int idx=mainLay->indexOf(progressSub.back());
        mainLay->insertWidget(idx, p);
        mainLay->insertWidget(idx, l);
    }
    labSubMessage<<l;
    progressSub<<p;
}
