#ifndef DLGESTIMATEFOCALVOLUME_H
#define DLGESTIMATEFOCALVOLUME_H

#include <QDialog>
#include "ui_estimatefocalvolumedlg.h"
#include "programoptions.h"


class dlgEstimateFocalVolume : public QDialog, private Ui::dlgEstimateFocalVolume {
        Q_OBJECT
    public:
        dlgEstimateFocalVolume(ProgramOptions* settings, QWidget* parent=NULL);
        virtual ~dlgEstimateFocalVolume();

        void init(double particles, double particles_error, bool has_nparticles, double tauD, double tauD_error, bool has_tauD, double gamma, double gamma_error, bool has_gamma);
        double get_wxy() { return wxy; };
        double get_wxyerror() { return wxy_error; }

    protected:
        double particles;
        double particles_error;
        bool has_nparticles;
        double tauD;
        double tauD_error;
        bool has_tauD;
        double gamma;
        double gamma_error;
        bool has_gamma;
        double wxy;
        double wxy_error;
        ProgramOptions* settings;

        void calc_from_C();
        void calc_from_D();
    protected slots:
        void on_dlgEstimateFocalVolume_accepted();

        void on_spinC_valueChanged(double d);
        void on_spinCError_valueChanged(double d);
        void on_spinD_valueChanged(double d);
        void on_spinDError_valueChanged(double d);
    private:
};

#endif // DLGESTIMATEFOCALVOLUME_H
