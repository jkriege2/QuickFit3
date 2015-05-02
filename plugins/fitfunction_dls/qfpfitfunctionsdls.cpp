/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "qfpfitfunctionsdls.h"
#include "qffitfunctiondlsg2ind.h"
#include "qffitfunctiondlsg2.h"
#include "qffitfunctiondlsg2indext.h"
#include "qffitfunctiondlsg2_gaussiantaudist.h"
#include "qffitfunctiondlsg2_loggaussiantaudist.h"

QFPFitFunctionsDLS::QFPFitFunctionsDLS(QObject *parent):
    QObject(parent)
{

}

QStringList QFPFitFunctionsDLS::getIDs() const {
    QStringList res;
    res<<"dls_g2"<<"dls_g2_ind"<<"dls_g2_ind_ext"<<"dls_g2_int_taunormdist"<<"dls_g2_int_taulognormdist";
    return res;
}

QFFitFunction* QFPFitFunctionsDLS::get(const QString & id) const  {
    //Q_UNUSED(parent);
    if (id=="dls_g2") {
        return new QFFitFunctionDLSG2();
    } else if (id=="dls_g2_int_taunormdist") {
        return new QFFitFunctionDLSG2NormTauDist();
    } else if (id=="dls_g2_int_taulognormdist") {
        return new QFFitFunctionDLSG2LogNormTauDist();
    } else if (id=="dls_g2_ind") {
        return new QFFitFunctionDLSG2inD();
    } else if (id=="dls_g2_ind_ext") {
        return new QFFitFunctionDLSG2inDExt();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunctions_dls, QFPFitFunctionsDLS)
