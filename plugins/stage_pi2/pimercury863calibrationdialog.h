#ifndef PIMERCURY863CALIBRATIONDIALOG_H
#define PIMERCURY863CALIBRATIONDIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <stdint.h>
#include <QCheckBox>

class QFExtensionLinearStagePI2; // forward


class PIMercury863CalibrationDialog : public QDialog {
        Q_OBJECT
    public:
        /** Default constructor */
        PIMercury863CalibrationDialog(QWidget* parent, QFExtensionLinearStagePI2* stage, int axis);
        /** Default destructor */
        virtual ~PIMercury863CalibrationDialog();
    protected slots:
        void getJoystickValues();
        void acceptCal();
        void calibrate();
        void center();
        void resetCal();
    protected:
        QFExtensionLinearStagePI2* stage;
        QProgressBar* prgX;
        QPushButton* btnOK;
        QPushButton* btnCancel;
        QPushButton* btnCentered;
        QPushButton* btnCalibrated;
        QPushButton* btnResetCal;
        QLabel* labX;
        QLabel* labMinX;
        QLabel* labMaxX;
        QCheckBox* chkInvert;

        int32_t calX[256];

        int32_t maxX;
        int32_t minX;
        int32_t zeroX;

        bool accepted;

        int axis;

    private:
};

#endif // PIMERCURY863CALIBRATIONDIALOG_H
