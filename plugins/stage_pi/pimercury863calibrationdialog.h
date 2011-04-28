#ifndef PIMERCURY863CALIBRATIONDIALOG_H
#define PIMERCURY863CALIBRATIONDIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include "pimercury863stage.h"


class PIMercury863CalibrationDialog : public QDialog {
        Q_OBJECT
    public:
        /** Default constructor */
        PIMercury863CalibrationDialog(QWidget* parent, PIMercury863Stage* stage);
        /** Default destructor */
        virtual ~PIMercury863CalibrationDialog();
    protected slots:
        void getJoystickValues();
        void acceptCal();
        void calibrate();
        void center();
        void resetCal();
    protected:
        PIMercury863Stage* stage;
        QProgressBar* prgX;
        QProgressBar* prgY;
        QProgressBar* prgZ;
        QPushButton* btnOK;
        QPushButton* btnCancel;
        QPushButton* btnCentered;
        QPushButton* btnCalibrated;
        QPushButton* btnResetCal;
        QLabel* labX;
        QLabel* labY;
        QLabel* labZ;
        QLabel* labMinX;
        QLabel* labMinY;
        QLabel* labMinZ;
        QLabel* labMaxX;
        QLabel* labMaxY;
        QLabel* labMaxZ;

        int calX[255];
        int calY[255];
        int calZ[255];

        int maxX, maxY, maxZ;
        int minX, minY, minZ;
        int zeroX, zeroY, zeroZ;

        bool accepted;

    private:
};

#endif // PIMERCURY863CALIBRATIONDIALOG_H
