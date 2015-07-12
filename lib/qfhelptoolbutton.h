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

#ifndef QFHELPTOOLBUTTON_H
#define QFHELPTOOLBUTTON_H

#include <QToolButton>
#include "lib_imexport.h"


class QFLIB_EXPORT QFHelpToolButton : public QToolButton
{
        Q_OBJECT
    public:
        explicit QFHelpToolButton(QWidget *parent = 0);
        explicit QFHelpToolButton(const QString& plugin, const QString& filename, QWidget *parent = 0);
        explicit QFHelpToolButton(const QString& filename, QWidget *parent = 0);
        /** \brief link to a help page of a QuickFit plugin (located in plugin help dir) */
        void setHelp(const QString& plugin, const QString& filename);
        /** \brief link to a help page of QuickFit (located in main help dir) */
        void setHelp(const QString& filename);
    signals:

    public slots:
        void showHelp();

    protected:
        QString m_plugin;
        QString m_filename;
    private:
        void initWidget();

};

#endif // QFHELPTOOLBUTTON_H
