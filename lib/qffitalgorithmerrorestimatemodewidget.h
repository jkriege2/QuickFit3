#ifndef QFFITALGORITHMERRORESTIMATEMODEWIDGET_H
#define QFFITALGORITHMERRORESTIMATEMODEWIDGET_H

#include <QWidget>
#include "qffitalgorithm.h"
#include "qfproperties.h"
#include "lib_imexport.h"

namespace Ui {
    class QFFitAlgorithmErrorEstimateModeWidget;
}

class QFLIB_EXPORT QFFitAlgorithmErrorEstimateModeWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit QFFitAlgorithmErrorEstimateModeWidget(QWidget *parent = 0);
        ~QFFitAlgorithmErrorEstimateModeWidget();

        QFFitAlgorithm::FitParameterErrorEstimates getMode() const;
        void setMode(QFFitAlgorithm::FitParameterErrorEstimates mode);

        void setBootstrappingRepeats(int repeats);
        int getBootstrappingRepeats() const;
        void setBootstrappingDistortion(double value);
        double getBootstrappingDistortion() const;
        void setBootstrappingFraction(double value);
        double getBootstrappingFraction() const;

        QString toString() const;

        void saveSettings(QFProperties* props, const QString& prefix=QString()) const;
        void readSettings(QFProperties* props, const QString& prefix=QString());
    signals:
        void parametersChanged();
        void modeChanged(QFFitAlgorithm::FitParameterErrorEstimates mode);
    protected slots:
        void on_cmbMode_currentIndexChanged(int index);
        void on_spinBSRepeats_valueChanged(int value);
        void on_spinBSDistortion_valueChanged(double value);
        void on_spinBSFraction_valueChanged(double value);
    private:
        Ui::QFFitAlgorithmErrorEstimateModeWidget *ui;
};

#endif // QFFITALGORITHMERRORESTIMATEMODEWIDGET_H
