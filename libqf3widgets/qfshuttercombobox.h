#ifndef QFSHUTTERCOMBOBOX_H
#define QFSHUTTERCOMBOBOX_H

#include <QComboBox>
#include "qfextensionmanager.h"
#include "../interfaces/qfextensionshutter.h"
#include "qfextension.h"
#include "libwid_imexport.h"
#include "qenhancedcombobox.h"
#include "qfmanyfilessettings.h"

/*! \brief QComboBox which is filled with all available cameras
    \ingroup qf3lib_widgets

    If this QComboBox is initialized with a valid QFExtensionManager object, it will be automatically
    filled with all available cameras.
 */
class QFWIDLIB_EXPORT QFShutterComboBox : public QEnhancedComboBox {
        Q_OBJECT
    public:
        QFShutterComboBox(QFExtensionManager* extManager, QWidget* parent=NULL);
        QFShutterComboBox(QWidget* parent=NULL);
        virtual ~QFShutterComboBox();
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
        /** \brief return the currently selected camera extension as QFExtensionShutter */
        QFExtensionShutter* currentExtensionShutter() const;
        /** \brief return the ID of the current camera device inside a QFExtensionShutter */
        int currentShutterID() const;
    public slots:
        /** \brief update the filled combobox */
        void findExtensions();

    protected:
        /** \brief list of all available QFExtensionShutter plugins, initialized by findExtensions() */
        QList<QObject*> shutters;

        /** \brief return the ID of the current camera device inside cameras */
        int currentExtensionID() const;

        QFExtensionManager* m_extManager;
    protected slots:
        void comboIndexChanged(int index);
    private:
};

#endif // QFSHUTTERCOMBOBOX_H
