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

QStringList QFPFitFunctionsFCCS::getIDs() const {
    QStringList res;
    res<<"fccs_diff3d";
    res<<"fccs_fw_diff2colorccf";
    res<<"fccs_fw_diff2coloracfg";
    res<<"fccs_fw_diff2coloracfr";
    res<<"fccs_fw_diff2colortcccf";
    res<<"fccs_fw_diff2colortcacfg";
    res<<"fccs_fw_diff2colortcacfr";
    res<<"fccs_fw_adiff2colorccf";
    res<<"fccs_fw_adiff2coloracfg";
    res<<"fccs_fw_adiff2coloracfr";
    return res;
}

QFFitFunction* QFPFitFunctionsFCCS::get(QString id, QObject* parent) const  {
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
    }
    return NULL;
}


int QFPFitFunctionsFCCS::getGlobalFitConfigCount() const
{
    return 4;
}

QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig QFPFitFunctionsFCCS::getGlobalFitConfig(int i) const
{
    QString menulabel=tr("configure for confocal FCCS ...");
    QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig res;
    if (i==0) { // fccs_fw_diff2coloracfg, fccs_fw_diff2coloracfr, fccs_fw_diff2colorccf
        res.groupLabel=menulabel;
        res.menuEntryLabel=tr("... 2-color, 1-component normal diffusion");
        res.models<<"fccs_fw_diff2coloracfg"<<"fccs_fw_diff2coloracfr"<<"fccs_fw_diff2colorccf";
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
        res.globalParams << constructQListWithMultipleItems(QStringList("pixel_width"), 3);
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

    } else if (i==1) { // fccs_spim_fw_diff2coloracfg, fccs_spim_fw_diff2coloracfr, fccs_spim_fw_diff2colorccf
        res.groupLabel=menulabel;
        res.menuEntryLabel=tr("... 2-color, 1-component normal diffusion, Da=Db=Dab");
        res.models<<"fccs_fw_diff2coloracfg"<<"fccs_fw_diff2coloracfr"<<"fccs_fw_diff2colorccf";
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
        res.globalParams << constructQListWithMultipleItems(QStringList("pixel_width"), 3);
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

    } else if (i==2) { // fccs_spim_fw_diff2coloracfg, fccs_spim_fw_diff2coloracfr, fccs_spim_fw_diff2colorccf
        res.groupLabel=menulabel;
        res.menuEntryLabel=tr("... 2-color, 1-component normal diffusion, Da=Db, Dab");
        res.models<<"fccs_fw_diff2coloracfg"<<"fccs_fw_diff2coloracfr"<<"fccs_fw_diff2colorccf";
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
        res.globalParams << constructQListWithMultipleItems(QStringList("pixel_width"), 3);
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







     } else    if (i==3) { // fccs_fw_diff2colortcacfg, fccs_fw_diff2colortcacfr, fccs_fw_diff2colortcccf
        res.groupLabel=menulabel;
        res.menuEntryLabel=tr("... 2-color, 2-component normal diffusion");
        res.models<<"fccs_fw_diff2colortcacfg"<<"fccs_fw_diff2colortcacfr"<<"fccs_fw_diff2colortcccf";
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
        res.globalParams << constructQListWithMultipleItems(QStringList("pixel_width"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("count_rate1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("count_rate2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
    } else if (i==4) { // fccs_fw_adiff2coloracfg, fccs_fw_adiff2coloracfr, fccs_fw_adiff2colorccf
        res.groupLabel=menulabel;
        res.menuEntryLabel=tr("... 2-color, 1-component anomalous diffusion");
        res.models<<"fccs_fw_adiff2coloracfg"<<"fccs_fw_adiff2coloracfr"<<"fccs_fw_adiff2colorccf";
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
        res.globalParams << constructQListWithMultipleItems(QStringList("pixel_width"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("count_rate1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("count_rate2"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
    }
    return res;
}

Q_EXPORT_PLUGIN2(fitfunctions_fccs, QFPFitFunctionsFCCS)
