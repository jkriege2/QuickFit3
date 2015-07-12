/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#ifndef QFESpectraVieweFluorophoreEditor_H
#define QFESpectraVieweFluorophoreEditor_H

#include <QDialog>
#include "spectrummanager.h"
#include "qftablemodel.h"

namespace Ui {
    class QFESpectraVieweFluorophoreEditor;
}

class QFESpectraViewerFluorophoreEditor : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFESpectraViewerFluorophoreEditor(QWidget *parent = 0);
        ~QFESpectraViewerFluorophoreEditor();

        void setFromData(const QString& ID, const SpectrumManager::FluorophoreData& data, SpectrumManager *manager=NULL);
        QString addDataAndSpectrum(QSettings &database, SpectrumManager *manager=NULL);
        void addSpectrum(SpectrumManager *manager=NULL, QString *filenameOut=NULL, QString *filenameOut_fl=NULL, int* spec_abs=NULL, int* spec_fl=NULL);
        void addSpectrumFl(SpectrumManager *manager=NULL, QString *filenameOut_fl=NULL, int* spec_fl=NULL);
        void addSpectrumAbs(SpectrumManager *manager=NULL, QString *filenameOut=NULL, int* spec_abs=NULL);
        SpectrumManager::FluorophoreData getData() const;
        QString getID() const;
        bool hasSpectrumChanged() const;

    protected slots:
        void on_btnClearSpectrum_clicked();
        void on_btnPasteSpectrum_clicked();
        void on_btnLoadSpectrumFile_clicked();
        void on_btnDeleteRow_clicked();
        void on_btnDeleteColumn_clicked();
        void on_btnClearSpectrum_2_clicked();
        void on_btnPasteSpectrum_2_clicked();
        void on_btnLoadSpectrumFile_2_clicked();
        void on_btnDeleteRow_2_clicked();
        void on_btnDeleteColumn_2_clicked();
        void setSpectrumChanged(bool changed=true);
        void setSpectrumChanged2(bool changed=true);

    private:
        Ui::QFESpectraVieweFluorophoreEditor *ui;
        SpectrumManager *manager;
        int oldspectrum;
        int oldspectrum_2;
        bool spectrumchanged;
        bool spectrumchanged_2;
        QFTableModel modSpectrum;
        QFTableModel modSpectrum_2;
};

#endif // QFESpectraVieweFluorophoreEditor_H
