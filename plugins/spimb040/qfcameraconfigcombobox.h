#ifndef QFCAMERACONFIGCOMBOBOX_H
#define QFCAMERACONFIGCOMBOBOX_H

#include <QtGui>
#include "qfextensionmanager.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensioncamera.h"
#include "qfextension.h"
#include "qfcameracombobox.h"

/*! \brief This class is used to notify all QFCameraConfigComboBox in the application to update their contents,
           if e.g. a configuration is deleted or added
    \ingroup qf3ext_spimb040

 */
class QFCameraConfigComboBoxNotifier: public QObject {
        Q_OBJECT
    public:
        QFCameraConfigComboBoxNotifier(QObject* parent);
    public slots:
        void emitUpdate();
    signals:
        void doUpdate();
};





/*! \brief QComboBox which is filled with all available camera config from the plugin config directory
    \ingroup qf3ext_spimb040

 */
class QFCameraConfigComboBox : public QComboBox {
        Q_OBJECT
    public:
        QFCameraConfigComboBox(QString configDirectory, QWidget* parent=NULL);
        virtual ~QFCameraConfigComboBox();

        /** \brief connect the given combobox to this combobox and initialize this with the current item of combo */
        void connectTo(QFCameraComboBox* combo);

        QString currentConfigFilename() const { if (currentIndex()<0) return ""; else return itemData(currentIndex()).toString(); };
        void setCurrentConfig(QString name);
    public slots:
        /*! \brief may be used to connect this widget to a QFCameraComboBox

            If called it updates the widget's contents.
         */
        void cameraChanged(QFExtension* extension, QFExtensionCamera* cam, int camIdx);

        /*! \brief reread the contents from current extension */
        void rereadConfigFiles();

        /*! \brief edit the current configuration */
        void editCurrent();

        /*! \brief delete the current configuration */
        void deleteCurrent();

        /*! \brief save the current configuration under a new name */
        void saveAsCurrent();
        /*! \brief rename the current configuration under a new name */
        void renameCurrent();
    protected slots:
        void directoryChanged(const QString& path);
    protected:
        QString m_configDirectory;
        QFExtension* m_extension;
        QFExtensionCamera* m_cam;
        int m_camIdx;
        static QFCameraConfigComboBoxNotifier* m_notifier;

    private:
};





/*! \brief this widget contains a QFCameraConfigComboBox and some buttons to edit it's contents
    \ingroup qf3ext_spimb040

 */
class QFCameraConfigEditorWidget : public QWidget {
        Q_OBJECT
    public:
        QFCameraConfigEditorWidget(QString configDirectory, QWidget* parent=NULL);
        virtual ~QFCameraConfigEditorWidget();

        /** \brief connect the given combobox to this combobox and initialize this with the current item of combo */
        void connectTo(QFCameraComboBox* combo) { combobox->connectTo(combo); };

        QString currentConfigFilename() const { return combobox->currentConfigFilename(); };
        QString currentConfigName() const { return combobox->currentText(); };
        void setCurrentConfig(QString name) { combobox->setCurrentConfig(name); }

        QAction* configAction() const { return actConfig; }
    public slots:
        /*! \brief may be used to connect this widget to a QFCameraComboBox

            If called it updates the widget's contents.
         */
        void cameraChanged(QFExtension* extension, QFExtensionCamera* cam, int camIdx) {
            combobox->cameraChanged(extension, cam, camIdx);
        }

        /*! \brief reread the contents from current extension */
        void rereadConfigFiles() {
            combobox->rereadConfigFiles();
        }

    protected:
        QFCameraConfigComboBox* combobox;
        /** \brief button to configure preview camera */
        QToolButton* btnConfig;
        /** \brief button to delete the current preview configuration */
        QToolButton* btnDelete;
        /** \brief button to save the current preview configuration as a different name */
        QToolButton* btnSaveAs;
        /** \brief button to rename the current preview configuration as a different name */
        QToolButton* btnRename;


        QAction* actConfig;
        QAction* actDelete;
        QAction* actSaveAs;
        QAction* actRename;
};

#endif // QFCAMERACONFIGCOMBOBOX_H
