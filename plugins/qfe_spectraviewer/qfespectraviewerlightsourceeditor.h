/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#ifndef QFESPECTRAVIEWERLIGHTSOURCEEDITOR_H
#define QFESPECTRAVIEWERLIGHTSOURCEEDITOR_H

#include <QDialog>
#include "spectrummanager.h"
#include "qftablemodel.h"

namespace Ui {
    class QFESpectraViewerLightsourceEditor;
}

class QFESpectraViewerLightsourceEditor : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFESpectraViewerLightsourceEditor(QWidget *parent = 0);
        ~QFESpectraViewerLightsourceEditor();

        void setFromData(const QString& ID, const SpectrumManager::LightSourceData& data, SpectrumManager *manager=NULL);
        QString addDataAndSpectrum(QSettings &database, SpectrumManager *manager=NULL);
        int addSpectrum(SpectrumManager *manager=NULL, QString *filenameOut=NULL);
        SpectrumManager::LightSourceData getData() const;
        QString getID() const;
        bool hasSpectrumChanged() const;

    protected slots:
        void on_btnClearSpectrum_clicked();
        void on_btnPasteSpectrum_clicked();
        void on_btnLoadSpectrumFile_clicked();
        void on_btnDeleteRow_clicked();
        void on_btnDeleteColumn_clicked();
        void setSpectrumChanged(bool changed=true);
    private:
        Ui::QFESpectraViewerLightsourceEditor *ui;
        SpectrumManager *manager;
        int oldspectrum;
        bool spectrumchanged;
        QFTableModel modSpectrum;
};

#endif // QFESPECTRAVIEWERLIGHTSOURCEEDITOR_H
