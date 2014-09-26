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

#include "qfpfitfunctions2ffcs.h"
#include "qffitfunction2ffccsnormaldiff3d.h"
#include "qffitfunction2ffccsnormaldiff3dflow.h"
#include "qffitfunction2ffccsnormaldiff2d.h"
#include "qffitfunction2ffccsnormaldiff2dflow.h"
#include "qftools.h"

QStringList QFPFitFunctions2FFCS::getIDs() const {
    QStringList res;
    res<<"fccs_2f_diff3d"<<"fccs_2f_diff3d_wz"<<"fccs_2f_diff3dflow"<<"fccs_2f_diff3dflow_wz"
       <<"fccs_2f_diff2d"<<"fccs_2f_diff2dflow";
    return res;
}

QFFitFunction* QFPFitFunctions2FFCS::get(QString id, QObject* parent) const  {
    if (id=="fccs_2f_diff3d") {
        return new QFFitFunctionFCCSNormalDiff3D2Focus(true);
    } else if (id=="fccs_2f_diff2d") {
        return new QFFitFunctionFCCSNormalDiff2D2Focus();
    } else if (id=="fccs_2f_diff3d_wz") {
        return new QFFitFunctionFCCSNormalDiff3D2Focus(false);
    } else if (id=="fccs_2f_diff3dflow") {
        return new QFFitFunctionFCCSNormalDiff3DFlow2Focus(true);
    } else if (id=="fccs_2f_diff2dflow") {
        return new QFFitFunctionFCCSNormalDiff2DFlow2Focus();
    } else if (id=="fccs_2f_diff3dflow_wz") {
        return new QFFitFunctionFCCSNormalDiff3DFlow2Focus(false);
    }
    return NULL;
}

int QFPFitFunctions2FFCS::getGlobalFitConfigCount() const
{
    return 2;
}

QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig QFPFitFunctions2FFCS::getGlobalFitConfig(int i) const
{
    QString menulabel=tr("configure for confocal FCCS ...");
    QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig res;
    int c=0;
    if (i==c++) {
        res.groupLabel=menulabel;
        res.menuEntryLabel=tr("... 2D diffusion+flow, ACF + 4 neighbors");
        res.models<<"fccs_2f_diff2dflowfccs_2f_diff2dflow"<<"fccs_2f_diff2dflow"<<"fccs_2f_diff2dflow"<<"fccs_2f_diff2dflow"<<"fccs_2f_diff2dflow";
        res.roles<<"acf";
        res.roles<<"acf"<<"dccf"<<"dccf"<<"dccf"<<"dccf";
        res.globalParams << constructQListWithMultipleItems(QStringList("n_particle"), 5);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_coeff1"), 5);
        res.globalParams << constructQListWithMultipleItems(QStringList("vflowx"), 5);
        res.globalParams << constructQListWithMultipleItems(QStringList("vflowy"), 5);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_height"), 5);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_width"), 5);

    } else if (i==c++) {
        res.groupLabel=menulabel;
        res.menuEntryLabel=tr("... 3D diffusion+flow, ACF + 4 neighbors");
        res.models<<"fccs_2f_diff3dflow"<<"fccs_2f_diff3dflow"<<"fccs_2f_diff3dflow"<<"fccs_2f_diff3dflow"<<"fccs_2f_diff3dflow";
        res.roles<<"acf"<<"dccf"<<"dccf"<<"dccf"<<"dccf";
        res.globalParams << constructQListWithMultipleItems(QStringList("n_particle"), 5);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_coeff1"), 5);
        res.globalParams << constructQListWithMultipleItems(QStringList("vflowx"), 5);
        res.globalParams << constructQListWithMultipleItems(QStringList("vflowy"), 5);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_height"), 5);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_width"), 5);
    }
    return res;
}

Q_EXPORT_PLUGIN2(fitfunctions_2ffcs, QFPFitFunctions2FFCS)
