#ifndef QFRDRIMAGINGFCSPIXELFROMOBJECTIVE_H
#define QFRDRIMAGINGFCSPIXELFROMOBJECTIVE_H

#include <QDialog>

namespace Ui {
    class QFRDRImagingFCSPixelFromObjective;
}

class QFRDRImagingFCSPixelFromObjective : public QDialog
{
        Q_OBJECT

    public:
        explicit QFRDRImagingFCSPixelFromObjective(QWidget *parent = 0);
        ~QFRDRImagingFCSPixelFromObjective();
        double getPixelSize() const;
    protected slots:
        void updateM();

    private:
        Ui::QFRDRImagingFCSPixelFromObjective *ui;
};

#endif // QFRDRIMAGINGFCSPIXELFROMOBJECTIVE_H
