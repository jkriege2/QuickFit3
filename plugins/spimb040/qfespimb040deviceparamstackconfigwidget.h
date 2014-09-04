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

#ifndef QFESPIMB040DeviceParamStackConfigWidget_H
#define QFESPIMB040DeviceParamStackConfigWidget_H

#include <QSettings>
#include <QAction>
#include <QToolButton>
#include <QWidget>
#include "qfextensionlinearstage.h"
#include "qfespimb040filenametool.h"
#include "qtriple.h"
#include "qfespimb040opticssetup.h"
#include "libtiff_tools.h"
#include "qfespimb040acquisitiontools.h"


class QFESPIMB040MainWindow; // forward
class QFPluginServices; // forward
class QFExtension;
//class QFESPIMB040OpticsSetup;

namespace Ui {
    class QFESPIMB040DeviceParamStackConfigWidget;
}


/*! \brief widget that allows to configure an device parameter stack acquisition
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040DeviceParamStackConfigWidget : public QWidget, public QFESPIMB040FilenameTool {
        Q_OBJECT

    public:
        explicit QFESPIMB040DeviceParamStackConfigWidget(QFESPIMB040AcquisitionTools* acqTools, QFPluginLogService* log, QWidget* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetupBase* opticsSetup, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription, QString configDirectory);
        ~QFESPIMB040DeviceParamStackConfigWidget();

        /** \brief return the filename for the currently selected camera configuration */
        QString currentConfigFilename(int camera) const;
        /** \brief return the name (not the full filename with path) for the currently selected camera configuration */
        QString currentConfigName(int camera) const;

        void updateReplaces();

        /*! \brief set the specified \a parameter (interpreted as in the parameter combobox) to the given \a value

           This function can be used when actually performing the stack
         */
        void setDeviceParameter(int parameter, double value);

        double getDeviceParameter(int parameter);

    signals:
        void doStack();

    public slots:
        void lightpathesChanged(QFESPIMB040OpticsSetupItems lightpathes);

        void performStack();

    public:
        void loadSettings(QSettings& settings, QString prefix);
        void storeSettings(QSettings& settings, QString prefix) const;

    protected:

        QString prefix1();
        QString prefix2();
        bool use1() const;
        bool use2() const;
        bool saveMeasurements() const;
        bool previewMode() const;

        double delay() const;

        int numImages() const;

        int stackParameter() const;
        QString stackParameterName() const;
        int stackMode() const;
        QString stackModeName() const;
        double stackStart() const;
        double stackDelta() const;
        double stackEnd() const;
        QList<double> stack() const;

        bool lightpathActivated() const;
        QString lightpathFilename() const;

        QString lightpath() const;
protected slots:
        void on_spinEnd_valueChanged(double value);
        void on_btnAcquire_clicked();
        void on_chkUse1_clicked(bool enabled);
        void on_chkUse2_clicked(bool enabled);

        void on_spinStart_valueChanged(double value);
        void on_spinDelta_valueChanged(double value);
        void on_cmbMode_currentIndexChanged(int index);

        void on_btnSaveTemplate_clicked();
        void on_btnLoadTemplate_clicked();

        void updateLabel();
    private:
        Ui::QFESPIMB040DeviceParamStackConfigWidget *ui;
        QFPluginServices* m_pluginServices;
        QFESPIMB040OpticsSetupBase* opticsSetup;
        QFESPIMB040AcquisitionDescription* acqDescription;
        QFESPIMB040ExperimentDescription* expDescription;

        QFPluginLogService* log;
        QFESPIMB040AcquisitionTools* acqTools;

};

#endif // QFESPIMB040DeviceParamStackConfigWidget_H
