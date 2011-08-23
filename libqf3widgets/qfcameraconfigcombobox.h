#ifndef QFCAMERACONFIGCOMBOBOX_H
#define QFCAMERACONFIGCOMBOBOX_H

#include <QtGui>
#include "qfextensionmanager.h"
#include "../interfaces/qfextensioncamera.h"
#include "qfextension.h"
#include "qfcameracombobox.h"

/*! \brief Interface that allows to stop/resume an acquisition
    \ingroup qf3lib_widgets
 */
class QFCameraConfigComboBoxStartResume {
public:
   ~QFCameraConfigComboBoxStartResume() {};

   virtual void stop()=0;
   virtual void resume()=0;
};


/*! \brief This class is used to notify all QFCameraConfigComboBox in the application to update their contents,
           if e.g. a configuration is deleted or added
    \ingroup qf3lib_widgets

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
    \ingroup qf3lib_widgets

    Using the QFCameraConfigComboBoxNotifier object, this class will keep contact to all its siblings and update
    it's contents whenever it is altered (using editCurrent(),  deleteCurrent(), saveAsCurrent() or renameCurrent() ).
    It is also connected to QFCameraComboBox and loads new camera configuration files if the current camera in this
    box changes.

 */
class QFCameraConfigComboBox : public QComboBox {
        Q_OBJECT
    public:
        QFCameraConfigComboBox(QString configDirectory, QWidget* parent=NULL);
        virtual ~QFCameraConfigComboBox();

        /** \brief connect the given combobox to this combobox and initialize this with the current item of combo */
        void connectTo(QFCameraComboBox* combo);

        /** \brief return the filename for the currently selected camera configuration */
        QString currentConfigFilename() const { if (currentIndex()<0) return ""; else return itemData(currentIndex()).toString(); };

        /** \brief select a given camera configuration */
        void setCurrentConfig(QString name);

        void setStopResume(QFCameraConfigComboBoxStartResume* stopresume);
    public slots:
        /*! \brief may be used to connect this widget to a QFCameraComboBox

            If called it updates the widget's contents.
         */
        void cameraChanged(QFExtension* extension, QFExtensionCamera* cam, int camIdx);

        /*! \brief reread the contents from current extension */
        void rereadConfigFiles();

        /*! \brief edit the current configuration */
        void editCurrent();

        /*! \brief add (&edit) new configuration */
        void addNew();

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
        QFCameraConfigComboBoxStartResume* m_stopresume;
        int m_camIdx;
        static QFCameraConfigComboBoxNotifier* m_notifier;

    private:
};





/*! \brief this widget contains a QFCameraConfigComboBox and some buttons to edit it's contents
    \ingroup qf3lib_widgets

 */
class QFCameraConfigEditorWidget : public QWidget {
        Q_OBJECT
    public:
        QFCameraConfigEditorWidget(QString configDirectory, QWidget* parent=NULL);
        virtual ~QFCameraConfigEditorWidget();

        /** \brief connect the given combobox to this combobox and initialize this with the current item of combo */
        void connectTo(QFCameraComboBox* combo) { combobox->connectTo(combo); };

        /** \brief return the filename for the currently selected camera configuration */
        QString currentConfigFilename() const { return combobox->currentConfigFilename(); };
        /** \brief return the name (not the full filename with path) for the currently selected camera configuration */
        QString currentConfigName() const { return combobox->currentText(); };
        /** \brief select a given camera configuration */
        void setCurrentConfig(QString name) { combobox->setCurrentConfig(name); }
        /** \brief this QAction triggers the configuration of the currently selected camera settings, provided for convenience, if you need this functionality at additional buttons in your GUI */
        QAction* configAction() const { return actConfig; }

        void setStopResume(QFCameraConfigComboBoxStartResume* stopresume) {
            combobox->setStopResume(stopresume);
        };

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


        void cmbCurrentIndexChanged(int index) {
            emit currentIndexChanged(index);
        }

    signals:
        /** \brief current item changed */
        void currentIndexChanged(int index);

    protected:
        QFCameraConfigComboBox* combobox;
        /** \brief button to configure preview camera */
        QToolButton* btnConfig;
        /** \brief button to delete the current preview configuration */
        QToolButton* btnDelete;
        /** \brief button to add a new configuration */
        QToolButton* btnAdd;
        /** \brief button to save the current preview configuration as a different name */
        QToolButton* btnSaveAs;
        /** \brief button to rename the current preview configuration as a different name */
        QToolButton* btnRename;


        QAction* actAdd;
        QAction* actConfig;
        QAction* actDelete;
        QAction* actSaveAs;
        QAction* actRename;
};

#endif // QFCAMERACONFIGCOMBOBOX_H
