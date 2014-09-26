/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-25 17:54:41 +0200 (Do, 25 Sep 2014) $  (revision $Rev: 3506 $)

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

#include "qfcompleterproperties.h"

QFCompleterProperties::QFCompleterProperties(QObject *parent):
    QCompleter(parent)
{
    m_project=NULL;
    m_stringlist=new QStringListModel(this);
    setModel(m_stringlist);
    setCompletionMode(QCompleter::PopupCompletion);
}

QFCompleterProperties::~QFCompleterProperties()
{
    setProject(NULL);
}

void QFCompleterProperties::setProject(QFProject *project)
{
    if (m_project) {
        disconnect(m_project, SIGNAL(propertiesChanged()), this, SLOT(propertiesChanged()));
    }
    this->m_project=project;
    QStringList sl;
    if (m_project) {
        sl=m_project->getAllPropertyNames();
        connect(m_project, SIGNAL(propertiesChanged()), this, SLOT(propertiesChanged()));
    }
    m_stringlist->setStringList(sl);
}

void QFCompleterProperties::propertiesChanged()
{
    QStringList sl;
    if (m_project) {
        sl=m_project->getAllPropertyNames();
    }
    m_stringlist->setStringList(sl);
}
