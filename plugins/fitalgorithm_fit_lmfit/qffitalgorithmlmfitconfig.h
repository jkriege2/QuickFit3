#ifndef QFFITALGORITHMLMFITCONFIG_H
#define QFFITALGORITHMLMFITCONFIG_H

#include <QDialog>

namespace Ui {
    class QFFitAlgorithmLMFitConfigDialog;
}

/*! \brief dialog class for lmfit Fit Algorithm
    \ingroup qf3fitalgp_lmfit
*/
class QFFitAlgorithmLMFitConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QFFitAlgorithmLMFitConfigDialog(QWidget *parent = 0, bool IRLS=false);
    ~QFFitAlgorithmLMFitConfigDialog();

    void init(double ftol, double xtol, double gtol, double epsilon, double stepbound, int max_iterations);
    void init(double ftol, double xtol, double gtol, double epsilon, double stepbound, int max_iterations, double irls_parameter, int irls_iterations);

    double ftol() const;
    double xtol() const;
    double gtol() const;
    double epsilon() const;
    double stepbound() const;
    int max_iterations() const;
    int irls_iterations() const;
    double irls_parameter() const;
private:
    Ui::QFFitAlgorithmLMFitConfigDialog *ui;
};

#endif // QFFITALGORITHMLMFITCONFIG_H
