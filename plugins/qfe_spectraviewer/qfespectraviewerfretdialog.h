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

#ifndef QFESpectraViewerFRETDialog_H
#define QFESpectraViewerFRETDialog_H

#include <QDialog>
#include "qftablemodel.h"
#include "qfespectraviewerdialog.h"
#include "spectrummanager.h"
namespace Ui {
    class QFESpectraViewerFRETDialog;
}

class QFESpectraViewerFRETDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFESpectraViewerFRETDialog(QWidget *parent = 0);
        ~QFESpectraViewerFRETDialog();

        void init(const QList<QFESpectraViewerPlotItem>& plotItems, SpectrumManager* manager);
    protected slots:
        void updateFRET();
        
    protected:
        QList<SpectrumManager::FluorophoreData> fluorophores;
        SpectrumManager *manager;
    private:
        Ui::QFESpectraViewerFRETDialog *ui;
        QFTableModel model;
};

#endif // QFESpectraViewerFRETDialog_H
