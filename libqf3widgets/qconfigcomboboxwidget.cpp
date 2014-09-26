/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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


#include "qconfigcomboboxwidget.h"
#include <QHBoxLayout>
#include <QInputDialog>
#include <QCheckBox>
#include <QRadioButton>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include "qfdoubleedit.h"

QConfigComboboxWidget::QConfigComboboxWidget(QString filename, QWidget *parent) :
    QWidget(parent)
{
    settings=NULL;
    createWidgets();
    setFilename(filename);
}

QConfigComboboxWidget::QConfigComboboxWidget(QWidget *parent) :
    QWidget(parent)
{
    settings=NULL;
    createWidgets();
}

QConfigComboboxWidget::~QConfigComboboxWidget()
{
    if (settings) delete settings;
}

void QConfigComboboxWidget::registerWidget(const QString &id, QWidget *widget)
{
    widgets[id]=widget;

    bool saved=false;
    /*for (int i=0; i<widgetFunctions.size(); i++) {
        if (widgetFunctions[i].registerWidget) saved=(*widgetFunctions[i].registerWidget)(widget, this);
        if (saved) break;
    }*/
    if (!saved) {
        QLineEdit* edt=qobject_cast<QLineEdit*>(widget);
        if (edt) {
            connect(edt, SIGNAL(textChanged(QString)), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QConfigComboboxWidgetSaveable* edt=dynamic_cast<QConfigComboboxWidgetSaveable*>(widget);
        if (edt) {
            connect(widget, SIGNAL(widgetContentsChanged()), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QCheckBox* edt=qobject_cast<QCheckBox*>(widget);
        if (edt) {
            connect(edt, SIGNAL(toggled(bool)), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QRadioButton* edt=qobject_cast<QRadioButton*>(widget);
        if (edt) {
            connect(edt, SIGNAL(toggled(bool)), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QSpinBox* edt=qobject_cast<QSpinBox*>(widget);
        if (edt) {
            connect(edt, SIGNAL(valueChanged(int)), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QDoubleSpinBox* edt=qobject_cast<QDoubleSpinBox*>(widget);
        if (edt) {
            connect(edt, SIGNAL(valueChanged(double)), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QFDoubleEdit* edt=qobject_cast<QFDoubleEdit*>(widget);
        if (edt) {
            connect(edt, SIGNAL(valueChanged(double)), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QSlider* edt=qobject_cast<QSlider*>(widget);
        if (edt) {
            connect(edt, SIGNAL(valueChanged(int)), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QPlainTextEdit* edt=qobject_cast<QPlainTextEdit*>(widget);
        if (edt) {
            connect(edt, SIGNAL(textChanged()), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QComboBox* edt=qobject_cast<QComboBox*>(widget);
        if (edt) {
            connect(edt, SIGNAL(currentIndexChanged(QString)), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
}

void QConfigComboboxWidget::registerWidget(const QString &id, const QConfigComboboxWidget::WidgetFunctions &widget)
{
     widgets[id]=widget.widget;
     widgetFunctions.append(widget);
     widget.registerWidget(widget.widget, this);
}

void QConfigComboboxWidget::unregisterWidget(const QString &id)
{
    QWidget* wid=widgets[id];

    bool saved=false;
    for (int i=0; i<widgetFunctions.size(); i++) {
        if (widgetFunctions[i].widget==wid) {
            if (widgetFunctions[i].unregisterWidget) saved=(*widgetFunctions[i].unregisterWidget)(wid, this);
            if (saved) break;
        }
    }
    if (!saved) {
        QConfigComboboxWidgetSaveable* edt=dynamic_cast<QConfigComboboxWidgetSaveable*>(wid);
        if (edt) {
            disconnect(wid, SIGNAL(widgetContentsChanged()), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
     }
    if (!saved) {
        QLineEdit* edt=qobject_cast<QLineEdit*>(wid);
        if (edt) {
            disconnect(edt, SIGNAL(textChanged(QString)), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QCheckBox* edt=qobject_cast<QCheckBox*>(wid);
        if (edt) {
            disconnect(edt, SIGNAL(toggled(bool)), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QRadioButton* edt=qobject_cast<QRadioButton*>(wid);
        if (edt) {
            disconnect(edt, SIGNAL(toggled(bool)), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QSpinBox* edt=qobject_cast<QSpinBox*>(wid);
        if (edt) {
            disconnect(edt, SIGNAL(valueChanged(int)), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QDoubleSpinBox* edt=qobject_cast<QDoubleSpinBox*>(wid);
        if (edt) {
            disconnect(edt, SIGNAL(valueChanged(double)), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QFDoubleEdit* edt=qobject_cast<QFDoubleEdit*>(wid);
        if (edt) {
            disconnect(edt, SIGNAL(valueChanged(double)), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QSlider* edt=qobject_cast<QSlider*>(wid);
        if (edt) {
            disconnect(edt, SIGNAL(valueChanged(int)), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QPlainTextEdit* edt=qobject_cast<QPlainTextEdit*>(wid);
        if (edt) {
            disconnect(edt, SIGNAL(textChanged()), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }
    if (!saved) {
        QComboBox* edt=qobject_cast<QComboBox*>(wid);
        if (edt) {
            disconnect(edt, SIGNAL(currentIndexChanged(QString)), this, SLOT(widgetContentsChanged()));
            saved=true;
        }
    }

    widgets.remove(id);

}

void QConfigComboboxWidget::unregisterWidget(QWidget *widget)
{
    QString key=widgets.key(widget, "");
    if (widgets.contains(key)) unregisterWidget(key);
}

void QConfigComboboxWidget::unregisterWidgets()
{
    QMapIterator<QString, QWidget*> w(widgets);

    while (w.hasNext()) {
        w.next();
        unregisterWidget(w.key());
    }
}

void QConfigComboboxWidget::renameEditor(const QString &id_old, const QString &id)
{
    setEditorName(widgets.value(id_old, NULL), id);
}

void QConfigComboboxWidget::setEditorName(QWidget *wid, const QString &id)
{
    if (wid) {
        QMapIterator<QString, QWidget*> it(widgets);
        QStringList ids;
        while (it.hasNext()) {
            it.next();
            if (it.value()==wid) ids<<it.key();
        }
        for (int i=0; i<ids.size(); i++) {
            widgets.remove(ids[i]);
        }
        widgets[id]=wid;
        combobox->setCurrentIndex(-1);
    }
}


int QConfigComboboxWidget::currentConfig() const
{
    return combobox->currentIndex();
}

QString QConfigComboboxWidget::currentConfigName() const
{
    return combobox->currentText();
}

QString QConfigComboboxWidget::currentFilename() const
{
    return filename;
}

void QConfigComboboxWidget::setCurrentConfig(const QString &name)
{
    combobox->setCurrentIndex(combobox->findText(name));
}

void QConfigComboboxWidget::setCurrentConfig(int index)
{
    combobox->setCurrentIndex(index);
}

void QConfigComboboxWidget::setFilename(const QString &filename)
{
    this->filename=filename;
    if (settings) delete settings;
    settings=new QSettings(filename, QSettings::IniFormat);
    loadAllConfig();
}

void QConfigComboboxWidget::saveCurrentConfig()
{
    QString name;
    bool ok=false;
    QStringList items;
    for (int i=0; i<combobox->count(); i++) items<<combobox->itemText(i);
    name=QInputDialog::getItem(this, tr("new config name ..."), tr("enter a new config name:"), items, -1, true, &ok);
    if (ok) saveWidgets(name);
}

void QConfigComboboxWidget::deleteCurrentConfig()
{
    deleteConfig(currentConfigName());
}

void QConfigComboboxWidget::currentConfigChanged(int i)
{
    if (i>=0) {
        loadWidgets(currentConfigName());
    }
}

void QConfigComboboxWidget::saveWidgets(const QString &id)
{
    if (!settings) return;
    QStringList groups=settings->childGroups();
    int idx=-1;
    int maxID=0;
    QRegExp rx("group(\\d+)");
    for (int i=0; i<groups.size(); i++) {
        if (settings->value(groups[i]+QString("/name"), "").toString()==id) {
            idx=i;
        }
        rx.indexIn(groups[i]);
        bool iok=false;
        rx.cap(1).toInt(&iok);
        if (iok && rx.cap(1).toInt()>=0) {
            maxID=qMax(maxID, rx.cap(1).toInt());
        }
    }
    QString sid=QString("group%1").arg(maxID+1);
    if (idx>=0) sid=groups[idx];
    settings->setValue(sid+"/name", id);
    QMapIterator<QString, QWidget*> w(widgets);

    while (w.hasNext()) {
        w.next();
        bool saved=false;
        for (int i=0; i<widgetFunctions.size(); i++) {
            if (widgetFunctions[i].widget==w.value()) {
                if (widgetFunctions[i].saveWidget) saved=(*widgetFunctions[i].saveWidget)(w.value(), *settings, sid+QString("/%1/").arg(w.key()));
                if (saved) break;
            }
        }
        if (!saved) {
            QConfigComboboxWidgetSaveable* edt=dynamic_cast<QConfigComboboxWidgetSaveable*>(w.value());
            if (edt) {
                edt->saveWidget(settings, sid+QString("/%1/").arg(w.key()));
                saved=true;
            }
        }
        if (!saved) {
            QLineEdit* edt=qobject_cast<QLineEdit*>(w.value());
            if (edt) {
                settings->setValue(sid+QString("/%1/text").arg(w.key()), edt->text());
                saved=true;
            }
        }
        if (!saved) {
            QCheckBox* edt=qobject_cast<QCheckBox*>(w.value());
            if (edt) {
                settings->setValue(sid+QString("/%1/checked").arg(w.key()), edt->isChecked());
                saved=true;
            }
        }
        if (!saved) {
            QRadioButton* edt=qobject_cast<QRadioButton*>(w.value());
            if (edt) {
                settings->setValue(sid+QString("/%1/checked").arg(w.key()), edt->isChecked());
                saved=true;
            }
        }
        if (!saved) {
            QSpinBox* edt=qobject_cast<QSpinBox*>(w.value());
            if (edt) {
                settings->setValue(sid+QString("/%1/value").arg(w.key()), edt->value());
                saved=true;
            }
        }
        if (!saved) {
            QDoubleSpinBox* edt=qobject_cast<QDoubleSpinBox*>(w.value());
            if (edt) {
                settings->setValue(sid+QString("/%1/value").arg(w.key()), edt->value());
                saved=true;
            }
        }
        if (!saved) {
            QFDoubleEdit* edt=qobject_cast<QFDoubleEdit*>(w.value());
            if (edt) {
                settings->setValue(sid+QString("/%1/value").arg(w.key()), edt->value());
                saved=true;
            }
        }
        if (!saved) {
            QSlider* edt=qobject_cast<QSlider*>(w.value());
            if (edt) {
                settings->setValue(sid+QString("/%1/value").arg(w.key()), edt->value());
                saved=true;
            }
        }
        if (!saved) {
            QPlainTextEdit* edt=qobject_cast<QPlainTextEdit*>(w.value());
            if (edt) {
                settings->setValue(sid+QString("/%1/text").arg(w.key()), edt->toPlainText());
                saved=true;
            }
        }
        if (!saved) {
            QComboBox* edt=qobject_cast<QComboBox*>(w.value());
            if (edt) {
                settings->setValue(sid+QString("/%1/text").arg(w.key()), edt->currentText());
                settings->setValue(sid+QString("/%1/item").arg(w.key()), edt->currentIndex());
                saved=true;
            }
        }
    }
    loadAllConfig();
}

void QConfigComboboxWidget::loadWidgets(const QString &id)
{
    if (!settings) return;
    QStringList groups=settings->childGroups();
    int idx=-1;
    for (int i=0; i<groups.size(); i++) {
        if (settings->value(groups[i]+QString("/name"), "").toString()==id) {
            idx=i;
        }
    }
    if (idx>=0) {
        QString sid=groups[idx];
        QMapIterator<QString, QWidget*> w(widgets);

        bool getMain=true;
        while (w.hasNext()) {
            w.next();
            if (w.key().toLower().startsWith("main") || (!getMain && !w.key().toLower().startsWith("main"))) {
                //qDebug()<<"loading "<<w.key()<<w.value();
                bool loaded=false;
                for (int i=0; i<widgetFunctions.size(); i++) {
                    if (widgetFunctions[i].widget==w.value()) {
                        if (widgetFunctions[i].loadWidget) loaded=(*widgetFunctions[i].loadWidget)(w.value(), *settings, sid+QString("/%1/").arg(w.key()));
                        if (loaded) break;
                    }
                }
                if (!loaded) {
                    QConfigComboboxWidgetSaveable* edt=dynamic_cast<QConfigComboboxWidgetSaveable*>(w.value());
                    if (edt) {
                        edt->loadWidget(settings, sid+QString("/%1/").arg(w.key()));
                        loaded=true;
                    }
                }

                if (!loaded) {
                    QLineEdit* edt=qobject_cast<QLineEdit*>(w.value());
                    if (edt) {
                        edt->setText(settings->value(sid+QString("/%1/text").arg(w.key()), edt->text()).toString());
                        loaded=true;
                    }
                }
                if (!loaded) {
                    QCheckBox* edt=qobject_cast<QCheckBox*>(w.value());
                    if (edt) {
                        edt->setChecked(settings->value(sid+QString("/%1/checked").arg(w.key()), edt->isChecked()).toBool());
                        loaded=true;
                    }
                }
                if (!loaded) {
                    QRadioButton* edt=qobject_cast<QRadioButton*>(w.value());
                    if (edt) {
                        edt->setChecked(settings->value(sid+QString("/%1/checked").arg(w.key()), edt->isChecked()).toBool());
                        loaded=true;
                    }
                }
                if (!loaded) {
                    QSpinBox* edt=qobject_cast<QSpinBox*>(w.value());
                    if (edt) {
                        edt->setValue(settings->value(sid+QString("/%1/value").arg(w.key()), edt->value()).toInt());
                        loaded=true;
                    }
                }
                if (!loaded) {
                    QDoubleSpinBox* edt=qobject_cast<QDoubleSpinBox*>(w.value());
                    if (edt) {
                        edt->setValue(settings->value(sid+QString("/%1/value").arg(w.key()), edt->value()).toDouble());
                        loaded=true;
                    }
                }
                if (!loaded) {
                    QFDoubleEdit* edt=qobject_cast<QFDoubleEdit*>(w.value());
                    if (edt) {
                        edt->setValue(settings->value(sid+QString("/%1/value").arg(w.key()), edt->value()).toDouble());
                        loaded=true;
                    }
                }
                if (!loaded) {
                    QSlider* edt=qobject_cast<QSlider*>(w.value());
                    if (edt) {
                        edt->setValue(settings->value(sid+QString("/%1/value").arg(w.key()), edt->value()).toInt());
                        loaded=true;
                    }
                }
                if (!loaded) {
                    QPlainTextEdit* edt=qobject_cast<QPlainTextEdit*>(w.value());
                    if (edt) {
                        edt->setPlainText(settings->value(sid+QString("/%1/text").arg(w.key()), edt->toPlainText()).toString());
                        loaded=true;
                    }
                }
                if (!loaded) {
                    QComboBox* edt=qobject_cast<QComboBox*>(w.value());
                    if (edt) {
                        if (edt->isEditable()) edt->setEditText(settings->value(sid+QString("/%1/text").arg(w.key()), edt->currentText()).toString());
                        else edt->setCurrentIndex(settings->value(sid+QString("/%1/item").arg(w.key()), edt->currentIndex()).toInt());
                        loaded=true;
                    }
                }
            }
            if (getMain && !w.hasNext()) {
                w=QMapIterator<QString, QWidget*>(widgets);
                getMain=false;
            }
        }
    }

    emit configChanged();

}

void QConfigComboboxWidget::deleteConfig(const QString &id)
{
    if (!settings) return;
    QStringList groups=settings->childGroups();
    int idx=-1;
    for (int i=0; i<groups.size(); i++) {
        if (settings->value(groups[i]+QString("/name"), "").toString()==id) {
            idx=i;
        }
    }
    if (idx>=0) {
        settings->beginGroup(groups[idx]);
        settings->remove("");
        settings->endGroup();
    }
    loadAllConfig();
}

void QConfigComboboxWidget::createWidgets()
{
    QHBoxLayout* hbl=new QHBoxLayout();
    setLayout(hbl);
    combobox=new QComboBox(this);
    connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentConfigChanged(int)));
    hbl->addWidget(combobox, 1);
    actSave=new QAction(QIcon(":/libqf3widgets/config_add.png"), tr("save config"), this);
    connect(actSave, SIGNAL(triggered()), this, SLOT(saveCurrentConfig()));
    actDelete=new QAction(QIcon(":/libqf3widgets/config_delete.png"), tr("delete config"), this);
    connect(actDelete, SIGNAL(triggered()), this, SLOT(deleteCurrentConfig()));
    QToolButton* btn;
    btn=new QToolButton(this);
    btn->setDefaultAction(actSave);
    hbl->addWidget(btn, 0);
    btn=new QToolButton(this);
    btn->setDefaultAction(actDelete);
    hbl->addWidget(btn, 0);
}

void QConfigComboboxWidget::loadAllConfig()
{
    if (!settings) return;
    disconnect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentConfigChanged(int)));
    int oldidx=combobox->currentIndex();
    combobox->clear();
    QStringList groups=settings->childGroups();
    for (int i=0; i<groups.size(); i++) {
        combobox->addItem(settings->value(groups[i]+QString("/name"), "").toString());
    }
    connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentConfigChanged(int)));
    combobox->setCurrentIndex(oldidx);
}

void QConfigComboboxWidget::widgetContentsChanged()
{
    if (!settings) return;
    QString id=combobox->currentText();
    QStringList groups=settings->childGroups();
    int idx=-1;
    for (int i=0; i<groups.size(); i++) {
        if (settings->value(groups[i]+QString("/name"), "").toString()==id) {
            idx=i;
        }
    }

    bool equals=true;
    if (idx>=0){
        QMapIterator<QString, QWidget*> w(widgets);
        QString sid=groups[idx];
        while (w.hasNext()) {
            w.next();

            for (int i=0; i<widgetFunctions.size(); i++) {
                if (widgetFunctions[i].widget==w.value()) {
                    if (widgetFunctions[i].equalsWidget) equals=equals&(*widgetFunctions[i].equalsWidget)(w.value(), *settings, sid+QString("/%1/").arg(w.key()));
                    break;
                }
            }
            {
                QConfigComboboxWidgetSaveable* edt=dynamic_cast<QConfigComboboxWidgetSaveable*>(w.value());
                if (edt) {
                    equals=equals&(!edt->widgetChanged(settings, sid+QString("/%1/").arg(w.key())));
                }
            }
            {

                QLineEdit* edt=qobject_cast<QLineEdit*>(w.value());
                if (edt) {
                    equals=equals&(edt->text()==(settings->value(sid+QString("/%1/text").arg(w.key()), edt->text()).toString()));
                }
            }            {
                QCheckBox* edt=qobject_cast<QCheckBox*>(w.value());
                if (edt) {
                    equals=equals&(edt->isChecked()==settings->value(sid+QString("/%1/checked").arg(w.key()), edt->isChecked()).toBool());
                }
            }
            {
                QRadioButton* edt=qobject_cast<QRadioButton*>(w.value());
                if (edt) {
                    equals=equals&(edt->isChecked()==settings->value(sid+QString("/%1/checked").arg(w.key()), edt->isChecked()).toBool());
                }
            }
            {
                QSpinBox* edt=qobject_cast<QSpinBox*>(w.value());
                if (edt) {
                    equals=equals&(edt->value()==settings->value(sid+QString("/%1/value").arg(w.key()), edt->value()).toInt());
                }
            }
            {
                QDoubleSpinBox* edt=qobject_cast<QDoubleSpinBox*>(w.value());
                if (edt) {
                    equals=equals&(edt->value()==settings->value(sid+QString("/%1/value").arg(w.key()), edt->value()).toDouble());
                }
            }
            {
                QFDoubleEdit* edt=qobject_cast<QFDoubleEdit*>(w.value());
                if (edt) {
                    equals=equals&(edt->value()==settings->value(sid+QString("/%1/value").arg(w.key()), edt->value()).toDouble());
                }
            }
            {
                QSlider* edt=qobject_cast<QSlider*>(w.value());
                if (edt) {
                    equals=equals&(edt->value()==settings->value(sid+QString("/%1/value").arg(w.key()), edt->value()).toInt());
                }
            }
            {
                QPlainTextEdit* edt=qobject_cast<QPlainTextEdit*>(w.value());
                if (edt) {
                    equals=equals&(edt->toPlainText()==settings->value(sid+QString("/%1/text").arg(w.key()), edt->toPlainText()).toString());
                }
            }
            {
                QComboBox* edt=qobject_cast<QComboBox*>(w.value());
                if (edt) {
                    if (edt->isEditable()) equals=equals&(edt->currentText()==settings->value(sid+QString("/%1/text").arg(w.key()), edt->currentText()).toString());
                    equals=equals&(edt->currentIndex()==settings->value(sid+QString("/%1/item").arg(w.key()), edt->currentIndex()).toInt());
                }
            }
            if (!equals) break;
        }
    }
    if (!equals) combobox->setCurrentIndex(-1);
}
