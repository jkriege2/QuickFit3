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

#include "qfpfitfunctionsfccs.h"
#include "qffitfunctionsfccsfwadiff2coloracfg.h"
#include "qffitfunctionsfccsfwadiff2coloracfr.h"
#include "qffitfunctionsfccsfwadiff2colorccf.h"
#include "qffitfunctionsfccsfwdiff2coloracfg.h"
#include "qffitfunctionsfccsfwdiff2coloracfr.h"
#include "qffitfunctionsfccsfwdiff2colorccf.h"
#include "qffitfunctionsfccsfwdiff2colortcacfg.h"
#include "qffitfunctionsfccsfwdiff2colortcacfr.h"
#include "qffitfunctionsfccsfwdiff2colortcccf.h"
#include "qftools.h"
#include "qffitfunctionsfccsfwdiff2colortcsepacfg.h"
#include "qffitfunctionsfccsfwdiff2colortcsepacfr.h"
#include "qffitfunctionsfccsfwdiff2colortcsepccf.h"
#include "qffitfunctionsfccsfwdiff2colorsepacfg.h"
#include "qffitfunctionsfccsfwdiff2colorsepacfr.h"
#include "qffitfunctionsfccsfwdiff2colorsepccf.h"

QStringList QFPFitFunctionsFCCS::getIDs() const {
    QStringList res;
    res<<"fccs_diff3d";
    res<<"fccs_fw_diff2colorccf";        // OK
    res<<"fccs_fw_diff2coloracfg";
    res<<"fccs_fw_diff2coloracfr";

    res<<"fccs_fw_diff2colorsepccf";
    res<<"fccs_fw_diff2colorsepacfg";
    res<<"fccs_fw_diff2colorsepacfr";

    res<<"fccs_fw_diff2colortcccf";      // OK
    res<<"fccs_fw_diff2colortcacfg";
    res<<"fccs_fw_diff2colortcacfr";

    res<<"fccs_fw_diff2colortcsepccf";
    res<<"fccs_fw_diff2colortcsepacfg";
    res<<"fccs_fw_diff2colortcsepacfr";

    res<<"fccs_fw_adiff2colorccf";         // OK
    res<<"fccs_fw_adiff2coloracfg";
    res<<"fccs_fw_adiff2coloracfr";
    return res;
}

QFFitFunction* QFPFitFunctionsFCCS::get(const QString &id) const  {
    if (id=="fccs_diff3d") {
        return new QFFitFunctionFCCSNormalDiff3D();
    } else if (id=="fccs_fw_diff2coloracfg") {
        return new QFFitFunctionsFCCSFWDiff2ColorACFG();
    } else if (id=="fccs_fw_diff2coloracfr") {
        return new QFFitFunctionsFCCSFWDiff2ColorACFR();
    } else if (id=="fccs_fw_diff2colorccf") {
        return new QFFitFunctionsFCCSFWDiff2ColorCCF();
    } else if (id=="fccs_fw_diff2colortcacfg") {
        return new QFFitFunctionsFCCSFWDiff2ColorTCACFG();
    } else if (id=="fccs_fw_diff2colortcacfr") {
        return new QFFitFunctionsFCCSFWDiff2ColorTCACFR();
    } else if (id=="fccs_fw_diff2colortcccf") {
        return new QFFitFunctionsFCCSFWDiff2ColorTCCCF();
    } else if (id=="fccs_fw_adiff2coloracfg") {
        return new QFFitFunctionsFCCSFWADiff2ColorACFG();
    } else if (id=="fccs_fw_adiff2coloracfr") {
        return new QFFitFunctionsFCCSFWADiff2ColorACFR();
    } else if (id=="fccs_fw_adiff2colorccf") {
        return new QFFitFunctionsFCCSFWADiff2ColorCCF();

    } else if (id=="fccs_fw_diff2colortcsepacfg") {
        return new QFFitFunctionsFCCSFWDiff2ColorTCSepACFG();
    } else if (id=="fccs_fw_diff2colortcsepacfr") {
        return new QFFitFunctionsFCCSFWDiff2ColorTCSepACFR();
    } else if (id=="fccs_fw_diff2colortcsepccf") {
        return new QFFitFunctionsFCCSFWDiff2ColorTCSepCCF();

    } else if (id=="fccs_fw_diff2colorsepacfg") {
        return new QFFitFunctionsFCCSFWDiff2ColorSepACFG();
    } else if (id=="fccs_fw_diff2colorsepacfr") {
        return new QFFitFunctionsFCCSFWDiff2ColorSepACFR();
    } else if (id=="fccs_fw_diff2colorsepccf") {
        return new QFFitFunctionsFCCSFWDiff2ColorSepCCF();

    }
    return NULL;
}


int QFPFitFunctionsFCCS::getGlobalFitConfigCount() const
{
    return 6;
}

QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig QFPFitFunctionsFCCS::getGlobalFitConfig(int i) const
{
    QString menulabel=tr("configure for confocal 2-color-FCCS ...");
    QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig res;
    int c=0;
    if (i==c++) { // fccs_fw_diff2coloracfg, fccs_fw_diff2coloracfr, fccs_fw_diff2colorccf
        res.groupLabel=menulabel;
        res.menuEntryLabel=tr("... 3D normal diffusion, species A+B+AB, c/D per species");
        res.models<<"fccs_fw_diff2coloracfg"<<"fccs_fw_diff2coloracfr"<<"fccs_fw_diff2colorccf";
        res.roles<<"acf0"<<"acf1"<<"fccs";
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_ab"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_coeff_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_coeff_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_coeff_ab"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("crosstalk"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_x"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_y"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_z"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_width1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_width2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_height1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_height2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_struct_fac1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_struct_fac2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("count_rate1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("count_rate2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);

        res.globalParams << constructQListWithMultipleItems(QStringList("n_nonfluorescent"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_tau_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_theta_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_tau_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_theta_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_tau_ab"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_theta_ab"), 3);

    } else if (i==c++) { // fccs_spim_fw_diff2coloracfg, fccs_spim_fw_diff2coloracfr, fccs_spim_fw_diff2colorccf
        res.groupLabel=menulabel;
        res.menuEntryLabel=tr("... 3D normal diffusion, species A+B+AB, c/D per species, Da=Db=Dab");
        res.models<<"fccs_fw_diff2coloracfg"<<"fccs_fw_diff2coloracfr"<<"fccs_fw_diff2colorccf";
        res.roles<<"acf0"<<"acf1"<<"fccs";
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_ab"), 3);
        res.globalParams << constructQListWithMultipleItems(constructQStringListFromItems("diff_coeff_a", "diff_coeff_b", "diff_coeff_ab"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("crosstalk"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_x"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_y"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_z"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_width1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_width2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_height1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_height2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_struct_fac1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_struct_fac2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("count_rate1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("count_rate2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);

        res.globalParams << constructQListWithMultipleItems(QStringList("n_nonfluorescent"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_tau_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_theta_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_tau_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_theta_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_tau_ab"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_theta_ab"), 3);


    } else if (i==c++) { // fccs_spim_fw_diff2coloracfg, fccs_spim_fw_diff2coloracfr, fccs_spim_fw_diff2colorccf
        res.groupLabel=menulabel;
        res.menuEntryLabel=tr("... 3D normal diffusion, species A+B+AB, c/D per species, Da=Db, Dab");
        res.models<<"fccs_fw_diff2coloracfg"<<"fccs_fw_diff2coloracfr"<<"fccs_fw_diff2colorccf";
        res.roles<<"acf0"<<"acf1"<<"fccs";
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_ab"), 3);
        res.globalParams << constructQListWithMultipleItems(constructQStringListFromItems("diff_coeff_a","diff_coeff_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_coeff_ab"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("crosstalk"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_x"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_y"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_z"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_width1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_width2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_height1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_height2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_struct_fac1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_struct_fac2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("count_rate1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("count_rate2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);

        res.globalParams << constructQListWithMultipleItems(QStringList("n_nonfluorescent"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_tau_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_theta_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_tau_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_theta_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_tau_ab"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_theta_ab"), 3);


    } else    if (i==c++) { // fccs_fw_diff2colorsepacfg, fccs_fw_diff2colorsepacfr, fccs_fw_diff2colorsepccf
       res.groupLabel=menulabel;
       res.menuEntryLabel=tr("... 3D normal diffusion, species A+B+AB, c per species, D per channel");
       res.models<<"fccs_fw_diff2colorsepacfg"<<"fccs_fw_diff2colorsepacfr"<<"fccs_fw_diff2colorsepccf";
       res.roles<<"acf0"<<"acf1"<<"fccs";

       res.globalParams << constructQListWithMultipleItems(QStringList("concentration_a"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("concentration_b"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("concentration_ab"), 3);

       res.globalParams << constructQListWithMultipleItems(QStringList("crosstalk"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_x"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_y"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_z"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_width1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_width2"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_height1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_height2"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_struct_fac1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_struct_fac2"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("count_rate1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("count_rate2"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);




    } else    if (i==c++) { // fccs_fw_diff2colortcacfg, fccs_fw_diff2colortcacfr, fccs_fw_diff2colortcccf
       res.groupLabel=menulabel;
       res.menuEntryLabel=tr("... 3D 2-comp. normal diffusion, species A+B+AB, c/D1/D2 per species");
       res.models<<"fccs_fw_diff2colortcacfg"<<"fccs_fw_diff2colortcacfr"<<"fccs_fw_diff2colortcccf";
       res.roles<<"acf0"<<"acf1"<<"fccs";

       res.globalParams << constructQListWithMultipleItems(QStringList("concentration_a"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("concentration_b"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("concentration_ab"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("diff_coeff_a"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("diff_coeff_b"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("diff_coeff_ab"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("diff_coeff2_a"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("diff_coeff2_b"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("diff_coeff2_ab"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("diff_rho2_a"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("diff_rho2_b"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("diff_rho2_ab"), 3);


       res.globalParams << constructQListWithMultipleItems(QStringList("n_nonfluorescent"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_tau_a"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_theta_a"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_tau_b"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_theta_b"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_tau_ab"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_theta_ab"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("crosstalk"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_x"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_y"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_z"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_width1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_width2"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_height1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_height2"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_struct_fac1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_struct_fac2"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("count_rate1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("count_rate2"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);


    } else    if (i==c++) { // fccs_fw_diff2colortcsepacfg, fccs_fw_diff2colortcsepacfr, fccs_fw_diff2colortcsepccf
       res.groupLabel=menulabel;
       res.menuEntryLabel=tr("... 3D 2-comp. normal diffusion, species A+B+AB, c per species, D1/D2 per channel");
       res.models<<"fccs_fw_diff2colortcsepacfg"<<"fccs_fw_diff2colortcsepacfr"<<"fccs_fw_diff2colortcsepccf";
       res.roles<<"acf0"<<"acf1"<<"fccs";

       res.globalParams << constructQListWithMultipleItems(QStringList("concentration_a"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("concentration_b"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("concentration_ab"), 3);

       res.globalParams << constructQListWithMultipleItems(QStringList("crosstalk"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_x"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_y"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_z"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_width1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_width2"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_height1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_height2"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_struct_fac1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("focus_struct_fac2"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("count_rate1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("count_rate2"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
    } else if (i==c++) { // fccs_fw_adiff2coloracfg, fccs_fw_adiff2coloracfr, fccs_fw_adiff2colorccf
        res.groupLabel=menulabel;
        res.menuEntryLabel=tr("... 3D anomalous diffusion, species A+B+AB, c/Gamma/alpha per species");
        res.models<<"fccs_fw_adiff2coloracfg"<<"fccs_fw_adiff2coloracfr"<<"fccs_fw_adiff2colorccf";
        res.roles<<"acf0"<<"acf1"<<"fccs";
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_ab"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_acoeff_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_acoeff_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_acoeff_ab"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_alpha_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_alpha_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_alpha_ab"), 3);

        res.globalParams << constructQListWithMultipleItems(QStringList("n_nonfluorescent"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_tau_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_theta_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_tau_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_theta_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_tau_ab"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("nonfl_theta_ab"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("crosstalk"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_x"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_y"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_distance_z"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_width1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_width2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_height1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_height2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_struct_fac1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("focus_struct_fac2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("pixel_width"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("count_rate1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("count_rate2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
    }
    return res;
}

Q_EXPORT_PLUGIN2(fitfunctions_fccs, QFPFitFunctionsFCCS)
