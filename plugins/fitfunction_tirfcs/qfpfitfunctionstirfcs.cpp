#include "qfpfitfunctionstirfcs.h"

#include "qffitfunctionstirfcsdiffe2.h"
#include "qffitfunctionstirfcsadiffe2.h"
#include "qffitfunctionstirfccsdiffflowe2.h"
#include "qftools.h"

QStringList QFPFitFunctionsTIRFCS::getIDs() const {
    QStringList res;
     res<<"fcs_tir_diffe2";
     res<<"fcs_tir_adiffe2";
     res<<"fccs_tir_diff_flowe2";
    return res;
}

QFFitFunction* QFPFitFunctionsTIRFCS::get(QString id, QObject* parent) const  {
    if (id=="fcs_tir_diffe2") {
        return new QFFitFunctionsTIRFCSDiffE2();
    } else if (id=="fcs_tir_adiffe2") {
        return new QFFitFunctionsTIRFCSADiffE2();
    } else if (id=="fccs_tir_diff_flowe2") {
        return new QFFitFunctionsTIRFCCSDiffFlowE2();
    }
    return NULL;
}

int QFPFitFunctionsTIRFCS::getGlobalFitConfigCount() const
{
    return 0;
}

QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig QFPFitFunctionsTIRFCS::getGlobalFitConfig(int i) const
{
	QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig res;
/*    int c=0;
    QString tirfccslabel=tr("configure for TIR-FCCS ...");
    
    if (i==c++) { // fccs_tir_fw_diff2coloracfg, fccs_tir_fw_diff2coloracfr, fccs_tir_fw_diff2colorccf
        res.groupLabel=tirfccslabel;
        res.menuEntryLabel=tr("... normal diffusion, species A+B+AB, c/D per species");
        res.models<<"fccs_tir_fw_diff2coloracfg"<<"fccs_tir_fw_diff2coloracfr"<<"fccs_tir_fw_diff2colorccf";
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


    } else if (i==c++) { // fccs_tir_fw_diff2coloracfg, fccs_tir_fw_diff2coloracfr, fccs_tir_fw_diff2colorccf
        res.groupLabel=tirfccslabel;
        res.menuEntryLabel=tr("... normal diffusion, species A+B+AB, c/D per species, Da=Db=Dab");
        res.models<<"fccs_tir_fw_diff2coloracfg"<<"fccs_tir_fw_diff2coloracfr"<<"fccs_tir_fw_diff2colorccf";
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

    } else if (i==c++) { // fccs_tir_fw_diff2coloracfg, fccs_tir_fw_diff2coloracfr, fccs_tir_fw_diff2colorccf
        res.groupLabel=tirfccslabel;
        res.menuEntryLabel=tr("... normal diffusion, species A+B+AB, c/D per species, Da=Db, Dab");
        res.models<<"fccs_tir_fw_diff2coloracfg"<<"fccs_tir_fw_diff2coloracfr"<<"fccs_tir_fw_diff2colorccf";
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



    } else if (i==c++) { // fccs_tir_fw_factordiff2coloracfg, fccs_tir_fw_factordiff2coloracfr, fccs_tir_fw_factordiff2colorccf
       res.groupLabel=tirfccslabel;
       res.menuEntryLabel=tr("... normal diffusion, species A+B+AB, c/D per species, Da, Db=Fb*Da, Dab=Fab*Da");
       res.models<<"fccs_tir_fw_factordiff2coloracfg"<<"fccs_tir_fw_factordiff2coloracfr"<<"fccs_tir_fw_factordiff2colorccf";
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



    } else    if (i==c++) { // fccs_tir_fw_sepdiff2coloracfg, fccs_tir_fw_sepdiff2coloracfr, fccs_tir_fw_sepdiff2colorccf
       res.groupLabel=tirfccslabel;
       res.menuEntryLabel=tr("... normal diffusion, species A+B+AB, c per species, D per channel");
       res.models<<"fccs_tir_fw_sepdiff2coloracfg"<<"fccs_tir_fw_sepdiff2coloracfr"<<"fccs_tir_fw_sepdiff2colorccf";

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
       res.globalParams << constructQListWithMultipleItems(QStringList("pixel_width"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("count_rate1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("count_rate2"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);



    } else    if (i==c++) { // fccs_tir_fw_2cdiff2coloracfg, fccs_tir_fw_2cdiff2coloracfr, fccs_tir_fw_2cdiff2colorccf
       res.groupLabel=tirfccslabel;
       res.menuEntryLabel=tr("... 2-comp. normal diffusion, species A+B+AB, c/D1/D2 per species");
       res.models<<"fccs_tir_fw_2cdiff2coloracfg"<<"fccs_tir_fw_2cdiff2coloracfr"<<"fccs_tir_fw_2cdiff2colorccf";
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


    } else    if (i==c++) { // fccs_tir_fw_2csepdiff2coloracfg, fccs_tir_fw_2csepdiff2coloracfr, fccs_tir_fw_2csepdiff2colorccf
       res.groupLabel=tirfccslabel;
       res.menuEntryLabel=tr("... 2-comp. normal diffusion, species A+B+AB, c per species, D1/D2 per channel");
       res.models<<"fccs_tir_fw_2csepdiff2coloracfg"<<"fccs_tir_fw_2csepdiff2coloracfr"<<"fccs_tir_fw_2csepdiff2colorccf";

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
       res.globalParams << constructQListWithMultipleItems(QStringList("pixel_width"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("count_rate1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("count_rate2"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);

    } else    if (i==c++) { // fccs_tir_fw_2csep2ddiffxy2coloracfg, fccs_tir_fw_2csep2ddiffxy2coloracfr, fccs_tir_fw_2csep2ddiffxy2colorccf
       res.groupLabel=tirfccslabel;
       res.menuEntryLabel=tr("... 2-comp. 2D diffusion (xy), species A+B+AB, c per species, D1/D2 per channel");
       res.models<<"fccs_tir_fw_2csep2ddiffxy2coloracfg"<<"fccs_tir_fw_2csep2ddiffxy2coloracfr"<<"fccs_tir_fw_2csep2ddiffxy2colorccf";

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
       res.globalParams << constructQListWithMultipleItems(QStringList("pixel_width"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("count_rate1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("count_rate2"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);


    } else    if (i==c++) { // fccs_tir_fw_2csep2ddiffxz2coloracfg, fccs_tir_fw_2csep2ddiffxz2coloracfr, fccs_tir_fw_2csep2ddiffxz2colorccf
       res.groupLabel=tirfccslabel;
       res.menuEntryLabel=tr("... 2-comp. 2D diffusion (xz), species A+B+AB, c per species, D1/D2 per channel");
       res.models<<"fccs_tir_fw_2csep2ddiffxz2coloracfg"<<"fccs_tir_fw_2csep2ddiffxz2coloracfr"<<"fccs_tir_fw_2csep2ddiffxz2colorccf";

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
       res.globalParams << constructQListWithMultipleItems(QStringList("pixel_width"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("count_rate1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("count_rate2"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);
       res.globalParams << constructQListWithMultipleItems(QStringList("background1"), 3);

    } else if (i==c++) { // fccs_tir_fw_adiff2coloracfg, fccs_tir_fw_adiff2coloracfr, fccs_tir_fw_adiff2colorccf
        res.groupLabel=tirfccslabel;
        res.menuEntryLabel=tr("... anomalous diffusion, species A+B+AB, c/Gamma/alpha per species");
        res.models<<"fccs_tir_fw_adiff2coloracfg"<<"fccs_tir_fw_adiff2coloracfr"<<"fccs_tir_fw_adiff2colorccf";
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


    }*/
    return res;
}

Q_EXPORT_PLUGIN2(fitfunctions_tirfcs, QFPFitFunctionsTIRFCS)
