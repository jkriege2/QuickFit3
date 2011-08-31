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
    explicit QFFitAlgorithmLMFitConfigDialog(QWidget *parent = 0);
    ~QFFitAlgorithmLMFitConfigDialog();

    void init(double ftol, double xtol, double gtol, double epsilon, double stepbound, int max_iterations);

    double ftol() const;
    double xtol() const;
    double gtol() const;
    double epsilon() const;
    double stepbound() const;
    int max_iterations() const;

private:
    Ui::QFFitAlgorithmLMFitConfigDialog *ui;
};

#endif // QFFITALGORITHMLMFITCONFIG_H
