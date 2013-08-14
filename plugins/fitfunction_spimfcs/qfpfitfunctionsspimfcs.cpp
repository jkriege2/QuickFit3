#include "qfpfitfunctionsspimfcs.h"
#include "qffitfunctionsspimfcsdiff.h"
#include "qffitfunctionsspimfcsdiffe2.h"
#include "qffitfunctionsspimfcsadiff.h"
#include "qffitfunctionsspimfccsdiffflow.h"
#include "qffitfunctionsspimfccsdiff.h"
#include "qffitfunctionsspimfccsdiff2color.h"
#include "qffitfunctionsspimfcsdiffc.h"
#include "qffitfunctionsspimfcsdiffce2.h"
#include "qffitfunctionsspimfcsdiffe2_newveff.h"
#include "qffitfunctionsspimfcsadiffe2.h"
#include "qffitfunctionsspimfcsadiffce2.h"
#include "qffitfunctionsspimfccsfwdiff2colorccf.h"
#include "qffitfunctionsspimfccsfwdiff2coloracfg.h"
#include "qffitfunctionsspimfccsfwdiff2coloracfr.h"
#include "qffitfunctionsspimfccsfwfactordiff2colorccf.h"
#include "qffitfunctionsspimfccsfwfactordiff2coloracfg.h"
#include "qffitfunctionsspimfccsfwfactordiff2coloracfr.h"
#include "qffitfunctionsspimfccsfw2cdiff2colorccf.h"
#include "qffitfunctionsspimfccsfw2cdiff2coloracfg.h"
#include "qffitfunctionsspimfccsfw2cdiff2coloracfr.h"
#include "qffitfunctionsspimfccsfwadiff2colorccf.h"
#include "qffitfunctionsspimfccsfwadiff2coloracfg.h"
#include "qffitfunctionsspimfccsfwadiff2coloracfr.h"
#include "qftools.h"

QStringList QFPFitFunctionsSPIMFCS::getIDs() const {
    QStringList res;
    res<<"fcs_spim_diff";
    res<<"fcs_spim_diffc";
    res<<"fcs_spim_adiff";

    res<<"fcs_spim_diffe2";
    res<<"fcs_spim_diffe2_newveff";

    res<<"fcs_spim_diffce2";
    res<<"fcs_spim_adiffe2";
    res<<"fcs_spim_adiffce2";

    res<<"fccs_spim_diff";
    res<<"fccs_spim_diffflow";
    res<<"fccs_spim_diff2color";

    res<<"fccs_spim_fw_diff2colorccf";
    res<<"fccs_spim_fw_diff2coloracfg";
    res<<"fccs_spim_fw_diff2coloracfr";
    res<<"fccs_spim_fw_factordiff2colorccf";
    res<<"fccs_spim_fw_factordiff2coloracfg";
    res<<"fccs_spim_fw_factordiff2coloracfr";
    res<<"fccs_spim_fw_2cdiff2colorccf";
    res<<"fccs_spim_fw_2cdiff2coloracfg";
    res<<"fccs_spim_fw_2cdiff2coloracfr";
    res<<"fccs_spim_fw_adiff2colorccf";
    res<<"fccs_spim_fw_adiff2coloracfg";
    res<<"fccs_spim_fw_adiff2coloracfr";
    return res;
}

QFFitFunction* QFPFitFunctionsSPIMFCS::get(QString id, QObject* parent) const  {
    if (id=="fcs_spim_diff") {
        return new QFFitFunctionsSPIMFCSDiff();
    } else if (id=="fcs_spim_diffc") {
        return new QFFitFunctionsSPIMFCSDiffC();
    } else if (id=="fcs_spim_diffce2") {
        return new QFFitFunctionsSPIMFCSDiffCE2();
    } else if (id=="fcs_spim_diffe2") {
        return new QFFitFunctionsSPIMFCSDiffE2();
    } else if (id=="fcs_spim_diffe2_newveff") {
        return new QFFitFunctionsSPIMFCSDiffE2NewVeff();
    } else if (id=="fcs_spim_adiff") {
        return new QFFitFunctionsSPIMFCSADiff();
    } else if (id=="fcs_spim_adiffe2") {
        return new QFFitFunctionsSPIMFCSADiffE2();
    } else if (id=="fcs_spim_adiffce2") {
        return new QFFitFunctionsSPIMFCSADiffCE2();
    } else if (id=="fccs_spim_diff") {
        return new QFFitFunctionsSPIMFCCSDiff();
    } else if (id=="fccs_spim_diffflow") {
        return new QFFitFunctionsSPIMFCCSDiffFlow();
    } else if (id=="fccs_spim_diff2color") {
        return new QFFitFunctionsSPIMFCCSDiff2Color();
    } else if (id=="fccs_spim_fw_diff2coloracfg") {
        return new QFFitFunctionsSPIMFCCSFWDiff2ColorACFG();
    } else if (id=="fccs_spim_fw_diff2coloracfr") {
        return new QFFitFunctionsSPIMFCCSFWDiff2ColorACFR();
    } else if (id=="fccs_spim_fw_diff2colorccf") {
        return new QFFitFunctionsSPIMFCCSFWDiff2ColorCCF();
    } else if (id=="fccs_spim_fw_factordiff2coloracfg") {
        return new QFFitFunctionsSPIMFCCSFWFactorDiff2ColorACFG();
    } else if (id=="fccs_spim_fw_factordiff2coloracfr") {
        return new QFFitFunctionsSPIMFCCSFWFactorDiff2ColorACFR();
    } else if (id=="fccs_spim_fw_factordiff2colorccf") {
        return new QFFitFunctionsSPIMFCCSFWFactorDiff2ColorCCF();
    } else if (id=="fccs_spim_fw_2cdiff2coloracfg") {
        return new QFFitFunctionsSPIMFCCSFW2CDiff2ColorACFG();
    } else if (id=="fccs_spim_fw_2cdiff2coloracfr") {
        return new QFFitFunctionsSPIMFCCSFW2CDiff2ColorACFR();
    } else if (id=="fccs_spim_fw_2cdiff2colorccf") {
        return new QFFitFunctionsSPIMFCCSFW2CDiff2ColorCCF();
    } else if (id=="fccs_spim_fw_adiff2coloracfg") {
        return new QFFitFunctionsSPIMFCCSFWADiff2ColorACFG();
    } else if (id=="fccs_spim_fw_adiff2coloracfr") {
        return new QFFitFunctionsSPIMFCCSFWADiff2ColorACFR();
    } else if (id=="fccs_spim_fw_adiff2colorccf") {
        return new QFFitFunctionsSPIMFCCSFWADiff2ColorCCF();
    }
    return NULL;
}

int QFPFitFunctionsSPIMFCS::getGlobalFitConfigCount() const
{
    return 8;
}

QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig QFPFitFunctionsSPIMFCS::getGlobalFitConfig(int i) const
{

    int c=0;
    QString spimfccslabel=tr("configure for SPIM-FCCS ...");
    QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig res;
    if (i==c++) { // fccs_spim_fw_diff2coloracfg, fccs_spim_fw_diff2coloracfr, fccs_spim_fw_diff2colorccf
        res.groupLabel=spimfccslabel;
        res.menuEntryLabel=tr("... 2-color, 1-component normal diffusion");
        res.models<<"fccs_spim_fw_diff2coloracfg"<<"fccs_spim_fw_diff2coloracfr"<<"fccs_spim_fw_diff2colorccf";
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


    } else if (i==c++) { // fccs_spim_fw_diff2coloracfg, fccs_spim_fw_diff2coloracfr, fccs_spim_fw_diff2colorccf
        res.groupLabel=spimfccslabel;
        res.menuEntryLabel=tr("... 2-color, 1-component normal diffusion, Da=Db=Dab");
        res.models<<"fccs_spim_fw_diff2coloracfg"<<"fccs_spim_fw_diff2coloracfr"<<"fccs_spim_fw_diff2colorccf";
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

    } else if (i==c++) { // fccs_spim_fw_diff2coloracfg, fccs_spim_fw_diff2coloracfr, fccs_spim_fw_diff2colorccf
        res.groupLabel=spimfccslabel;
        res.menuEntryLabel=tr("... 2-color, 1-component normal diffusion, Da=Db, Dab");
        res.models<<"fccs_spim_fw_diff2coloracfg"<<"fccs_spim_fw_diff2coloracfr"<<"fccs_spim_fw_diff2colorccf";
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_ab"), 3);
        res.globalParams << constructQListWithMultipleItems(constructQStringListFromItems("diff_coeff_a", "diff_coeff_b"), 3);
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

    } else if (i==c++) { // fccs_spim_fw_diff2coloracfg, fccs_spim_fw_diff2coloracfr, fccs_spim_fw_diff2colorccf
        res.groupLabel=spimfccslabel;
        res.menuEntryLabel=tr("... 2-color, 1-component normal diffusion, one independent D for each channels");
        res.models<<"fccs_spim_fw_diff2coloracfg"<<"fccs_spim_fw_diff2coloracfr"<<"fccs_spim_fw_diff2colorccf";
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_ab"), 3);

        QStringList Ds;
        Ds<<"diff_coeff_a"<<"diff_coeff_b"<<"diff_coeff_ab";
        res.globalParams << constructQListFromItems(Ds,QStringList(),QStringList());
        res.globalParams << constructQListFromItems(QStringList(),Ds,QStringList());
        res.globalParams << constructQListFromItems(QStringList(),QStringList(),Ds);

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



    } else if (i==c++) { // fccs_spim_fw_factordiff2coloracfg, fccs_spim_fw_factordiff2coloracfr, fccs_spim_fw_factordiff2colorccf
       res.groupLabel=spimfccslabel;
       res.menuEntryLabel=tr("... 2-color, 1-component normal diffusion, Da, Db=Fb*Da, Dab=Fab*Da");
       res.models<<"fccs_spim_fw_factordiff2coloracfg"<<"fccs_spim_fw_factordiff2coloracfr"<<"fccs_spim_fw_factordiff2colorccf";
       res.globalParams << constructQListWithMultipleItems(QStringList("concentration_a"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("concentration_b"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("concentration_ab"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("diff_coeff_a"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("diff_coefffactor_b"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("diff_coefffactor_ab"), 3);
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







    } else    if (i==c++) { // fccs_spim_fw_2cdiff2coloracfg, fccs_spim_fw_2cdiff2coloracfr, fccs_spim_fw_2cdiff2colorccf
       res.groupLabel=spimfccslabel;
       res.menuEntryLabel=tr("... 2-color, 2-component normal diffusion");
       res.models<<"fccs_spim_fw_2cdiff2coloracfg"<<"fccs_spim_fw_2cdiff2coloracfr"<<"fccs_spim_fw_2cdiff2colorccf";
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
    } else    if (i==c++) { // fccs_spim_fw_2cdiff2coloracfg, fccs_spim_fw_2cdiff2coloracfr, fccs_spim_fw_2cdiff2colorccf
       res.groupLabel=spimfccslabel;
       res.menuEntryLabel=tr("... 2-color, 2-component normal diffusion, two independent D's for each channels");
       res.models<<"fccs_spim_fw_2cdiff2coloracfg"<<"fccs_spim_fw_2cdiff2coloracfr"<<"fccs_spim_fw_2cdiff2colorccf";

       res.globalParams << constructQListWithMultipleItems(QStringList("concentration_a"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("concentration_b"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("concentration_ab"), 3);

       QStringList Ds, D2s, rhos;
       Ds<<"diff_coeff_a"<<"diff_coeff_b"<<"diff_coeff_ab";
       D2s<<"diff_coeff2_a"<<"diff_coeff2_b"<<"diff_coeff2_ab";
       rhos<<"diff_rho2_a"<<"diff_rho2_b"<<"diff_rho2_ab";
       res.globalParams << constructQListFromItems(Ds,QStringList(),QStringList());
       res.globalParams << constructQListFromItems(QStringList(),Ds,QStringList());
       res.globalParams << constructQListFromItems(QStringList(),QStringList(),Ds);
       res.globalParams << constructQListFromItems(D2s,QStringList(),QStringList());
       res.globalParams << constructQListFromItems(QStringList(),D2s,QStringList());
       res.globalParams << constructQListFromItems(QStringList(),QStringList(),D2s);
       res.globalParams << constructQListFromItems(rhos,QStringList(),QStringList());
       res.globalParams << constructQListFromItems(QStringList(),rhos,QStringList());
       res.globalParams << constructQListFromItems(QStringList(),QStringList(),rhos);

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

    } else if (i==c++) { // fccs_spim_fw_adiff2coloracfg, fccs_spim_fw_adiff2coloracfr, fccs_spim_fw_adiff2colorccf
        res.groupLabel=spimfccslabel;
        res.menuEntryLabel=tr("... 2-color, 1-component anomalous diffusion");
        res.models<<"fccs_spim_fw_adiff2coloracfg"<<"fccs_spim_fw_adiff2coloracfr"<<"fccs_spim_fw_adiff2colorccf";
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("concentration_ab"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_acoeff_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_acoeff_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_acoeff_ab"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_alpha_a"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_alpha_b"), 3);
        res.globalParams << constructQListWithMultipleItems(QStringList("diff_alpha_ab"), 3);
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

Q_EXPORT_PLUGIN2(fitfunctions_spimfcs, QFPFitFunctionsSPIMFCS)
