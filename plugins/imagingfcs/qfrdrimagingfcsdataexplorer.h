#ifndef QFRDRIMAGINGFCSDATAEXPLORER_H
#define QFRDRIMAGINGFCSDATAEXPLORER_H

#include <QDialog>

namespace Ui {
    class QFRDRImagingFCSDataExplorer;
}

class QFRDRImagingFCSDataExplorer : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRImagingFCSDataExplorer(QWidget *parent = 0);
        ~QFRDRImagingFCSDataExplorer();

        void setBleachDecay(double frames);
        double getBleachDecay() const;
        
    private:
        Ui::QFRDRImagingFCSDataExplorer *ui;
};

#endif // QFRDRIMAGINGFCSDATAEXPLORER_H
