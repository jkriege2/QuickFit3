/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#include "qfhelptoolbutton.h"
#include "qfpluginservices.h"

QFHelpToolButton::QFHelpToolButton(QWidget *parent) :
    QToolButton(parent)
{
    setHelp("quickfit.html");
    initWidget();
}

QFHelpToolButton::QFHelpToolButton(const QString &plugin, const QString &filename, QWidget *parent) :
    QToolButton(parent)
{
    setHelp(plugin, filename);
    initWidget();
}

QFHelpToolButton::QFHelpToolButton(const QString &filename, QWidget *parent) :
    QToolButton(parent)
{
    setHelp(filename);
    initWidget();
}

void QFHelpToolButton::setHelp(const QString &plugin, const QString &filename)
{
    m_plugin=plugin;
    m_filename=filename;
}

void QFHelpToolButton::setHelp(const QString &filename)
{
    m_plugin="";
    m_filename=filename;
}

void QFHelpToolButton::showHelp()
{
    if (m_plugin.isEmpty()) QFPluginServices::getInstance()->displayMainHelpWindow(m_filename);
    else  QFPluginServices::getInstance()->displayPluginHelpWindow(m_plugin, m_filename);
}

void QFHelpToolButton::initWidget()
{
    connect(this, SIGNAL(clicked()), this, SLOT(showHelp()));
    setText("");
    setIcon(QIcon(":/lib/help.png"));
    setToolTip(tr("show online-help ..."));
}
