/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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


#ifndef QCONFIGCOMBOBOXWIDGET_H
#define QCONFIGCOMBOBOXWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QAction>
#include <QToolButton>
#include <QSettings>
#include "libwid_imexport.h"

class QFWIDLIB_EXPORT QConfigComboboxWidgetSaveable {
    public:
        virtual bool widgetChanged(QSettings* settings, const QString& prefix)=0;
        virtual bool saveWidget(QSettings* settings, const QString& prefix)=0;
        virtual bool loadWidget(QSettings* settings, const QString& prefix)=0;
};

class QFWIDLIB_EXPORT QConfigComboboxWidget : public QWidget
{
        Q_OBJECT
    public:
        typedef bool (*SaveWidgetFunction)(const QWidget*, QSettings&, const QString& prefix);
        typedef bool (*LoadWidgetFunction)(QWidget*, const QSettings&, const QString& prefix);
        typedef bool (*EqualsWidgetFunction)(const QWidget*, const QSettings&, const QString& prefix);
        typedef bool (*RegisterWidgetFunction)(QWidget*, QConfigComboboxWidget* config);
        typedef bool (*UnregisterWidgetFunction)(QWidget*, QConfigComboboxWidget* config);

        struct WidgetFunctions {
            SaveWidgetFunction saveWidget;
            LoadWidgetFunction loadWidget;
            EqualsWidgetFunction equalsWidget;
            RegisterWidgetFunction registerWidget;
            UnregisterWidgetFunction unregisterWidget;
            QWidget* widget;
        };

        explicit QConfigComboboxWidget(QString filename=QString("configcombobox.ini"), QWidget *parent = 0);
        explicit QConfigComboboxWidget(QWidget *parent = 0);
        virtual ~QConfigComboboxWidget();


        void registerWidget(const QString& id, QWidget* widget);
        void registerWidget(const QString& id, const WidgetFunctions& widget);
        void unregisterWidget(const QString& id);
        void unregisterWidget(QWidget* widget);
        void unregisterWidgets();
        void renameEditor(const QString& id_old, const QString& id);
        void setEditorName(QWidget* wid, const QString& id);
        int currentConfig() const;
        QString currentConfigName() const;
        QString currentFilename() const;
    signals:
        void configChanged();
    public slots:
        void setCurrentConfig(const QString& name);
        void setCurrentConfig(int index);
        void setFilename(const QString& filename);

        void widgetContentsChanged();
    protected slots:
        void saveCurrentConfig();
        void deleteCurrentConfig();
        void currentConfigChanged(int i);

        void saveWidgets(const QString& id);
        void loadWidgets(const QString& id);
        void deleteConfig(const QString& id);

        void createWidgets();
        void loadAllConfig();
    protected:
        QComboBox* combobox;
        QAction* actSave;
        QAction* actDelete;

        QString filename;
        QSettings* settings;
        QMap<QString, QWidget*> widgets;
        QList<WidgetFunctions> widgetFunctions;
        
};

#endif // QCONFIGCOMBOBOXWIDGET_H
