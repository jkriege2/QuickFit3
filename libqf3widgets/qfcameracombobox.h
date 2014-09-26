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


#ifndef QFCAMERACOMBOBOX_H
#define QFCAMERACOMBOBOX_H

#include <QComboBox>
#include "qfextensionmanager.h"
#include "qfextensioncamera.h"
#include "qfextension.h"
#include "libwid_imexport.h"
#include "qenhancedcombobox.h"
#include "qfmanyfilessettings.h"
/*! \brief QComboBox which is filled with all available cameras
    \ingroup qf3lib_widgets

    If this QComboBox is initialized with a valid QFExtensionManager object, it will be automatically
    filled with all available cameras.
 */
class QFWIDLIB_EXPORT QFCameraComboBox : public QEnhancedComboBox {
        Q_OBJECT
    public:
        QFCameraComboBox(QFExtensionManager* extManager, QWidget* parent=NULL);
        QFCameraComboBox(QWidget* parent=NULL);
        virtual ~QFCameraComboBox();
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
        /** \brief return the currently selected camera extension as QFExtensionCamera */
        QFExtensionCamera* currentExtensionCamera() const;
        QObject* currentCameraQObject() const;
        /** \brief return the ID of the current camera device inside a QFExtensionCamera */
        int currentCameraID() const;
    public slots:
        /** \brief update the filled combobox */
        void findExtensions();
    signals:
         /** \brief emitted when the user selects a new camera */
         void cameraChanged(QFExtension* extension, QFExtensionCamera* cam, int camIdx);
    protected:
        /** \brief list of all available QFExtensionCamera plugins, initialized by findExtensions() */
        QList<QObject*> cameras;

        /** \brief return the ID of the current camera device inside cameras */
        int currentExtensionID() const;

        QFExtensionManager* m_extManager;
    protected slots:
        void camIndexChanged(int index);
    private:
};

#endif // QFCAMERACOMBOBOX_H
