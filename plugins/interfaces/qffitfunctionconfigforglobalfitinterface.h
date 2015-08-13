/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
	
	

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

#ifndef QFFITFUNCTIONCONFIGFORGLOBALFITINTERFACE_H
#define QFFITFUNCTIONCONFIGFORGLOBALFITINTERFACE_H

#include <QStringList>
#include <QString>
#include "qfplugin.h"
#include <cfloat>
#include <stdint.h>
/*! \brief interface that allows to automatically config for a global fit
    \ingroup qf3rdrdp_fcs

    The data is returned as a GlobalFitConfig struct, which defines which models to use and which parameters to link in them:
    The member globalParams lists all global parameters and contains a list of a stringlist.
    The first inner list enumerates the files/models
    and the second gives the parameters that should be linked within that file:
      <center>\c [globalParam, File] -> {parameters to link in the given file}</center>

    So, if the first global parameter should link the parameter "D" and the second parameter "C" in every of three models,
    the contents of globalParams should be:
    \code
      globalParams[0] = { {D}, {D}, {D} };
      globalParams[1] = { {C}, {C}, {C} };
    \endcode
    If in addition, the first parameter should also link D1 and D2 in every model, the we have:
    \code
      globalParams[0] = { {D1,D2}, {D1,D2}, {D1,D2} };
      globalParams[1] = { {C}, {C}, {C} };
    \endcode

*/
class QFFitFunctionConfigForGlobalFitInterface
{
    public:
        virtual ~QFFitFunctionConfigForGlobalFitInterface() {}

        struct GlobalFitParameter {
            double value;
            double error;
            double rangeMin;
            double rangeMax;
            bool setRange;

        };

        struct GlobalFitConfig {
            QString shortLabel;
            QString groupLabel;
            QString menuEntryLabel;
            QStringList models;
            QStringList roles;
            QList<QStringList> fixes;
            QList<QMap<QString, GlobalFitParameter> > paramValues;
            QList<QMap<QString, bool> > singleFixes;
            QList<QList<QStringList> > globalParams;
        };

        virtual int getGlobalFitConfigCount() const=0;
        virtual GlobalFitConfig getGlobalFitConfig(int i) const=0;

};

inline QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig QFFitFunctionConfigForGlobalFitInterface_GlobalFitConfig_get(const QFFitFunctionConfigForGlobalFitInterface* intf, const QString& shortlabel) {
    if (intf) {
        for (int i=0; i<intf->getGlobalFitConfigCount(); i++) {
            if (intf->getGlobalFitConfig(i).shortLabel==shortlabel) {
                return intf->getGlobalFitConfig(i);
            }
        }
    }
    return QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig();
}

inline QFFitFunctionConfigForGlobalFitInterface::GlobalFitParameter QFFitFunctionConfigForGlobalFitInterface_GlobalFitParameter_get() {
    QFFitFunctionConfigForGlobalFitInterface::GlobalFitParameter p;
    p.value=0;
    p.error=0;
    p.rangeMin=-DBL_MIN;
    p.rangeMax=DBL_MAX;
    p.setRange=true;
    return p;
}

inline QFFitFunctionConfigForGlobalFitInterface::GlobalFitParameter QFFitFunctionConfigForGlobalFitInterface_GlobalFitParameter_get(double val, double err, double rangeMin, double rangeMax) {
    QFFitFunctionConfigForGlobalFitInterface::GlobalFitParameter p;
    p.value=val;
    p.error=err;
    p.rangeMax=rangeMax;
    p.rangeMin=rangeMin;
    p.setRange=true;
    return p;
}
inline QFFitFunctionConfigForGlobalFitInterface::GlobalFitParameter QFFitFunctionConfigForGlobalFitInterface_GlobalFitParameter_get(double val, double err) {
    QFFitFunctionConfigForGlobalFitInterface::GlobalFitParameter p;
    p.value=val;
    p.error=err;
    p.rangeMin=-DBL_MIN;
    p.rangeMax=DBL_MAX;
    p.setRange=false;
    return p;
}

Q_DECLARE_INTERFACE( QFFitFunctionConfigForGlobalFitInterface,
                     "www.dkfz.de.b040.quickfit3.plugin.QFFitFunctionConfigForGlobalFitInterface/1.0")



#endif // QFFITFUNCTIONCONFIGFORGLOBALFITINTERFACE_H
