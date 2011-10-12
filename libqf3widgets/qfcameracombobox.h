#ifndef QFCAMERACOMBOBOX_H
#define QFCAMERACOMBOBOX_H

#include <QComboBox>
#include "qfextensionmanager.h"
#include "../interfaces/qfextensioncamera.h"
#include "qfextension.h"
#include "libwid_imexport.h"

/*! \brief QComboBox which is filled with all available cameras
    \ingroup qf3lib_widgets

    If this QComboBox is initialized with a valid QFExtensionManager object, it will be automatically
    filled with all available cameras.
 */
class QFWIDLIB_EXPORT QFCameraComboBox : public QComboBox {
        Q_OBJECT
    public:
        QFCameraComboBox(QFExtensionManager* extManager, QWidget* parent=NULL);
        QFCameraComboBox(QWidget* parent=NULL);
        virtual ~QFCameraComboBox();
        void init(QFExtensionManager* extManager);
        /** \brief return the currently selected camera extension as QFExtension */
        QFExtension* currentExtension() const;
        /** \brief return the currently selected camera extension as QFExtensionCamera */
        QFExtensionCamera* currentExtensionCamera() const;
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
