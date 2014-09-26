/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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


#ifndef QFACQUISITIONCOMBOBOX_H
#define QFACQUISITIONCOMBOBOX_H

#include <QComboBox>
#include "qfextensionmanager.h"
#include "qfextensionacquisition.h"
#include "qfextension.h"
#include "libwid_imexport.h"
#include "qenhancedcombobox.h"

/*! \brief QComboBox which is filled with all available acquisition devices
    \ingroup qf3lib_widgets

    If this QComboBox is initialized with a valid QFExtensionManager object, it will be automatically
    filled with all available acquisition plugins.
 */
class QFWIDLIB_EXPORT QFAcquisitionComboBox : public QEnhancedComboBox {
        Q_OBJECT
    public:
        QFAcquisitionComboBox(QFExtensionManager* extManager, QWidget* parent=NULL);
        QFAcquisitionComboBox(QWidget* parent=NULL);
        virtual ~QFAcquisitionComboBox();
        /** \brief init the combobox with all acquisition devices registered in the QFExtensionManager */
        void init(QFExtensionManager* extManager);
        /** \brief save the currently selected acquisition device to a QSettings object */
        void storeSettings(QSettings& settings, QString prefix=QString("")) const;
        /** \brief load the currently selected acquisition device from a QSettings object */
        void loadSettings(QSettings& settings, QString prefix=QString(""));
        /** \brief return the currently selected acquisition device extension as QFExtension */
        QFExtension* currentExtension() const;
        /** \brief return the currently selected acquisition device extension as QFExtensionAcquisition */
        QFExtensionAcquisition* currentExtensionAcquisition() const;
        QObject* currentAcquisitionQObject() const;
        /** \brief return the ID of the current acquisition device device inside a QFExtensionAcquisition */
        int currentAcquisitionID() const;
    public slots:
        /** \brief update the filled combobox */
        void findExtensions();
    signals:
         /** \brief emitted when the user selects a new acquisition device */
         void deviceChanged(QFExtension* extension, QFExtensionAcquisition* cam, int camIdx);
    protected:
        /** \brief list of all available QFExtensionAcquisition plugins, initialized by findExtensions() */
        QList<QObject*> devices;

        /** \brief return the ID of the current acquisition device device inside acquisition devices */
        int currentExtensionID() const;

        QFExtensionManager* m_extManager;
    protected slots:
        void deviceIndexChanged(int index);
    private:
};

#endif // QFACQUISITIONCOMBOBOX_H
