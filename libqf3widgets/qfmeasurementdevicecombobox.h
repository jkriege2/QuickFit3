/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QFMeasurementDeviceComboBox_H
#define QFMeasurementDeviceComboBox_H

#include <QComboBox>
#include "qfextensionmanager.h"
#include "qfextensionmeasurementdevice.h"
#include "qfextension.h"
#include "libwid_imexport.h"
#include "qenhancedcombobox.h"
#include "qfmanyfilessettings.h"

/*! \brief QComboBox which is filled with all available measurement devices
    \ingroup qf3lib_widgets

    If this QComboBox is initialized with a valid QFExtensionManager object, it will be automatically
    filled with all available cameras.
 */
class QFWIDLIB_EXPORT QFMeasurementDeviceComboBox : public QEnhancedComboBox {
        Q_OBJECT
    public:
        QFMeasurementDeviceComboBox(QFExtensionManager* extManager, QWidget* parent=NULL);
        QFMeasurementDeviceComboBox(QWidget* parent=NULL);
        virtual ~QFMeasurementDeviceComboBox();
        /** \brief init the combobox with all cameras registered in the QFExtensionManager */
        void init(QFExtensionManager* extManager);
        /** \brief save the currently selected camera to a QSettings object */
        void storeSettings(QSettings& settings, QString prefix=QString("")) const;
        void storeSettings(QFManyFilesSettings& settings, QString prefix=QString("")) const;
        /** \brief load the currently selected camera from a QSettings object */
        void loadSettings(QSettings& settings, QString prefix=QString(""));
        void loadSettings(QFManyFilesSettings& settings, QString prefix=QString(""));
        /** \brief return the currently selected camera extension as QFExtension */
        QFExtension* currentExtension() const;
        QObject* currentExtensionObject() const;
        /** \brief return the currently selected camera extension as QFExtensionMeasurementAndControlDevice */
        QFExtensionMeasurementAndControlDevice* currentExtensionDevice() const;
        /** \brief return the ID of the current camera device inside a QFExtensionMeasurementAndControlDevice */
        int currentDeviceID() const;
        void setCurrentItem(QFExtension* extension_in, int device);
    public slots:
        /** \brief update the filled combobox */
        void findExtensions();

    protected:
        /** \brief list of all available QFExtensionMeasurementAndControlDevice plugins, initialized by findExtensions() */
        QList<QObject*> devices;

        /** \brief return the ID of the current camera device inside cameras */
        int currentExtensionID() const;

        QFExtensionManager* m_extManager;
    protected slots:
        void comboIndexChanged(int index);
    private:
};

#endif // QFMeasurementDeviceComboBox_H
