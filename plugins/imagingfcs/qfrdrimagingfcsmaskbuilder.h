#ifndef QFRDRIMAGINGFCSMASKBUILDER_H
#define QFRDRIMAGINGFCSMASKBUILDER_H

#include <QDialog>

namespace Ui {
    class QFRDRImagingFCSMaskBuilder;
}

class QFRDRImagingFCSMaskBuilder : public QDialog
{
        Q_OBJECT

    public:
        explicit QFRDRImagingFCSMaskBuilder(const QStringList &parameters, const QStringList &parameterNames, QWidget *parent = 0);
        ~QFRDRImagingFCSMaskBuilder();

        bool use1(QString& parameter, int& mode, double& thresh1, double& thresh2);
        bool use2(QString& parameter, int& mode, double& thresh1, double& thresh2);
        bool use3(QString& parameter, int& mode, double& thresh1, double& thresh2);
        bool use4(QString& parameter, int& mode, double& thresh1, double& thresh2);
        bool use5(QString& parameter, int& mode, double& thresh1, double& thresh2);
        bool onlyMasked();

    private:
        Ui::QFRDRImagingFCSMaskBuilder *ui;
};

#endif // QFRDRIMAGINGFCSMASKBUILDER_H
