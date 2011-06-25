#ifndef QFCAMERACOMBOBOX_H
#define QFCAMERACOMBOBOX_H

#include <QComboBox>
#include "qfextensionmanager.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensioncamera.h"
#include "qfextension.h"

/*! \brief QComboBox which is filled with all available cameras
    \ingroup qf3ext_spimb040


 */
class QFCameraComboBox : public QComboBox {
        Q_OBJECT
    public:
        QFCameraComboBox(QFExtensionManager* extManager, QWidget* parent=NULL);
        virtual ~QFCameraComboBox();

        QFExtension* currentExtension() const;
        QFExtensionCamera* currentExtensionCamera() const;
        int currentCameraID() const;
        int currentExtensionID() const;
    public slots:
        /** \brief update the filled combobox */
        void findExtensions();
    signals:
         /** \brief emitted when the user selects a new camera */
         void cameraChanged(QFExtension* extension, QFExtensionCamera* cam, int camIdx);
    protected:
        /** \brief list of all available QFExtensionCamera plugins, initialized by findExtensions() */
        QList<QObject*> cameras;

        QFExtensionManager* m_extManager;
    protected slots:
        void camIndexChanged(int index);
    private:
};

#endif // QFCAMERACOMBOBOX_H
