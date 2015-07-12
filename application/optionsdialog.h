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

#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include<QtGlobal>
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "ui_optionsdialog.h"
#include "../lib/programoptions.h"
#include "qfpluginoptionsdialog.h"

/*! \brief options dialog wrapper class for QuickFit
    \ingroup qf3app
*/
class OptionsDialog : public QDialog, public Ui::OptionsDlg {
        Q_OBJECT
    public:
        /** Default constructor */
        OptionsDialog(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~OptionsDialog();
        /** \brief display options dialog and store to the given ProgramOptions object */
        void open(ProgramOptions* options);

        void setPlugins(const QList<QFPluginOptionsDialogInterface*>& plugins);
    protected slots:
        void stylesheetChanged( const QString & text );
        //void on_cmbStylesheet_highlighted( const QString & text );
        void styleChanged( const QString & text );
        //void on_cmbStyle_highlighted( const QString & text );
        void on_btnHelp_clicked();
        void on_listWidget_currentRowChanged(int currentRow );
        void on_spinMath_valueChanged(int value);
        void updateFontExample();

    private:
        ProgramOptions* m_options;
        QList<QFPluginOptionsWidget*> m_plugins;
};

#endif // OPTIONSDIALOG_H
