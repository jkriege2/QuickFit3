/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFESPIMB040IMAGESTACKCONFIGWIDGET2_H
#define QFESPIMB040IMAGESTACKCONFIGWIDGET2_H

#include <QSettings>
#include <QAction>
#include <QToolButton>
#include <QWidget>
#include "../interfaces/qfextensionlinearstage.h"
#include "qfespimb040filenametool.h"
#include "qfespimb040opticssetup.h"
#include "qfespimb040acquisitiondescription.h"
#include "qfespimb040experimentdescription.h"
#include "qfespimb040acquisitiontools.h"

class QFESPIMB040MainWindow; // forward
class QFPluginServices; // forward
class QFExtension;

namespace Ui {
    class QFESPIMB040ImageStackConfigWidget2;
}


/*! \brief widget that allows to configure an image stack acquisition
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040ImageStackConfigWidget2 : public QWidget, public QFESPIMB040FilenameTool {
        Q_OBJECT

    public:
        explicit QFESPIMB040ImageStackConfigWidget2(QFESPIMB040AcquisitionTools* acqTools, QFPluginLogService* log, QWidget* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetupBase* opticsSetup, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription, QString configDirectory);
        ~QFESPIMB040ImageStackConfigWidget2();

        /** \brief return the filename for the currently selected camera configuration */
        QString currentConfigFilename(int camera) const;
        /** \brief return the name (not the full filename with path) for the currently selected camera configuration */
        QString currentConfigName(int camera) const;



    public:
        void loadSettings(QSettings& settings, QString prefix);
        void storeSettings(QSettings& settings, QString prefix) const;


    protected:
        bool saveMeasurements() const;

        int images() const;
        QString prefix1() ;
        QString prefix2() ;
        bool use1() const;
        bool use2() const;

        bool useStage2() const;
        bool useStage3() const;

        bool stage2Cycling() const;
        bool stage3Cycling() const;

        int delay() const;
        QFExtensionLinearStage* stage() const;
        QFExtension* stageExtension() const;
        int currentAxisID() const;
        int stackCount() const;
        double stackStart() const;
        double stackDelta() const;


        QFExtensionLinearStage* stage2() const;
        QFExtension* stageExtension2() const;
        int currentAxisID2() const;
        int stackCount2() const;
        double stackStart2() const;
        double stackDelta2() const;


        QFExtensionLinearStage* stage3() const;
        QFExtension* stageExtension3() const;
        int currentAxisID3() const;
        int stackCount3() const;
        double stackStart3() const;
        double stackDelta3() const;

        bool lightpath1Activated() const;
        bool lightpath2Activated() const;
        bool lightpath3Activated() const;

        QString lightpath1() const;
        QString lightpath1Filename() const;
        QString lightpath2() const;
        QString lightpath2Filename() const;
        QString lightpath3() const;
        QString lightpath3Filename() const;
    signals:
        void doStack();

    public slots:
        void lightpathesChanged(QFESPIMB040OpticsSetupItems lightpathes);
        void performStack();
    protected slots:
        void on_btnAcquire_clicked();
        void on_btnAcquireNext_clicked();
        void on_chkUse1_clicked(bool enabled);
        void on_chkUse2_clicked(bool enabled);

        void on_spinStart_valueChanged(double value);
        void on_spinDelta_valueChanged(double value);
        void on_spinSteps_valueChanged(int value);
        void actGetCurrent_clicked();
        void actGetCurrentAround_clicked();
        void on_btnConfig_clicked();
        void on_btnConnect_clicked();

        void on_chkStage2_clicked(bool enabled);
        void on_chkStage3_clicked(bool enabled);

        void on_spinStart2_valueChanged(double value);
        void on_spinDelta2_valueChanged(double value);
        void on_spinSteps2_valueChanged(int value);
        void actGetCurrent2_clicked();
        void actGetCurrentAround2_clicked();
        void on_btnConfig2_clicked();
        void on_btnConnect2_clicked();

        void on_spinStart3_valueChanged(double value);
        void on_spinDelta3_valueChanged(double value);
        void on_spinSteps3_valueChanged(int value);
        void actGetCurrent3_clicked();
        void actGetCurrentAround3_clicked();
        void on_btnConfig3_clicked();
        void on_btnConnect3_clicked();

        void on_chkStackRelative_toggled(bool checked);

        void on_btnSaveTemplate_clicked();
        void on_btnLoadTemplate_clicked();

        void on_btnTestAcq1_pressed();
        void on_btnTestAcq1_released();
        void on_btnTestAcq2_pressed();
        void on_btnTestAcq2_released();

        void on_btnTestAcqLP1_pressed();
        void on_btnTestAcqLP1_released();

        void on_btnTestAcqLP2_pressed();
        void on_btnTestAcqLP2_released();

        void on_btnTestAcqLP3_pressed();
        void on_btnTestAcqLP3_released();


        void checkStage();
        void updateLabel();

        void updateReplaces();
    private:
        Ui::QFESPIMB040ImageStackConfigWidget2 *ui;
        QFPluginServices* m_pluginServices;
        QFESPIMB040OpticsSetupBase* opticsSetup;
        QFESPIMB040AcquisitionDescription* acqDescription;
        QFESPIMB040ExperimentDescription* expDescription;
        QFPluginLogService* log;
        QFESPIMB040AcquisitionTools* acqTools;

        QAction* actGetCurrent;
        QAction* actGetCurrentAround;
        QAction* actGetCurrent2;
        QAction* actGetCurrentAround2;
        QAction* actGetCurrent3;
        QAction* actGetCurrentAround3;
        QActionGroup* actGetCurrentG;
        QActionGroup* actGetCurrentG2;
        QActionGroup* actGetCurrentG3;

        bool lastGetC, lastGetC2, lastGetC3;

};

#endif // QFESPIMB040IMAGESTACKCONFIGWIDGET2_H
