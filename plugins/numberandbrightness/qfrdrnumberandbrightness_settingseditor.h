/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate: 2014-09-02 17:40:55 +0200 (Di, 02 Sep 2014) $  (revision $Rev: 3430 $)

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
#ifndef qfrdrnumberandbrightness_settingseditor_H
#define qfrdrnumberandbrightness_settingseditor_H

#include <QWidget>
#include "qfrawdataeditor.h"
#include <QLabel>
#include "qfplotter.h"
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>
#include "qvisiblehandlesplitter.h"
#include <QSplitter>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QAbstractTableModel>
#include "datacutslider.h"
#include "qt/jkqtfastplotter.h"
#include "jkqtptools.h"
#include "qfrdroverviewimagedisplay.h"

namespace Ui {
    class QFRDRNumberAndBrightnessSettingsEditor;
}

/*! \brief editor for QFRawDataRecord
    \ingroup qf3rdrdp_number_and_brightness
*/
class QFRDRNumberAndBrightnessSettingsEditor : public QFRawDataEditor
{
        Q_OBJECT

    public:
        explicit QFRDRNumberAndBrightnessSettingsEditor(QFPluginServices* services,  QFRawDataPropertyEditor* propEditor, QWidget *parent = 0);
        ~QFRDRNumberAndBrightnessSettingsEditor();
       /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);

    protected:
        void connectAllWidgets(bool en);
    protected slots:
        void recalcNB();
        void writeData();
        void writeDataToAllRDR();
        void writeData(QFRawDataRecord* current, bool notImageProp=false);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();

        void on_cmbDetector_currentIndexChanged(int index);
    protected:
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

    private:
        Ui::QFRDRNumberAndBrightnessSettingsEditor *ui;
};

#endif // qfrdrnumberandbrightness_settingseditor_H
