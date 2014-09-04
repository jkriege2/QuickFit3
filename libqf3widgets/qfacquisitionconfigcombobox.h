/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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


#ifndef QFACQUISITIONCONFIGCOMBOBOX_H
#define QFACQUISITIONCONFIGCOMBOBOX_H

#include <QtGui>
#include "qfextensionmanager.h"
#include "qfextensionacquisition.h"
#include "qfextension.h"
#include "qfacquisitioncombobox.h"
#include "libwid_imexport.h"
#include "qenhancedcombobox.h"

/*! \brief Interface that allows to stop/resume an acquisition
    \ingroup qf3lib_widgets
 */
class QFWIDLIB_EXPORT QFAcquisitionConfigComboBoxStartResume {
public:
   ~QFAcquisitionConfigComboBoxStartResume() {};

   virtual void stop()=0;
   virtual void resume()=0;
};


/*! \brief This class is used to notify all QFAcquisitionConfigComboBox in the application to update their contents,
           if e.g. a configuration is deleted or added
    \ingroup qf3lib_widgets

 */
class QFWIDLIB_EXPORT QFAcquisitionConfigComboBoxNotifier: public QObject {
        Q_OBJECT
    public:
        QFAcquisitionConfigComboBoxNotifier(QObject* parent);
    public slots:
        void emitUpdate();
    signals:
        void doUpdate();
};





/*! \brief QComboBox which is filled with all available acquisition plugin config from the plugin config directory
    \ingroup qf3lib_widgets

    Using the QFAcquisitionConfigComboBoxNotifier object, this class will keep contact to all its siblings and update
    it's contents whenever it is altered (using editCurrent(),  deleteCurrent(), saveAsCurrent() or renameCurrent() ).
    It is also connected to QFAcquisitionComboBox and loads new acquisition plugin configuration files if the current acquisition plugin in this
    box changes.

 */
class QFWIDLIB_EXPORT QFAcquisitionConfigComboBox : public QEnhancedComboBox {
        Q_OBJECT
    public:
        QFAcquisitionConfigComboBox(QWidget* parent=NULL, QString configDirectory=QString(""));
        virtual ~QFAcquisitionConfigComboBox();

        void init(QString configDirectory);

        /** \brief connect the given combobox to this combobox and initialize this with the current item of combo */
        void connectTo(QFAcquisitionComboBox* combo);

        /** \brief return the filename for the currently selected acquisition plugin configuration */
        QString currentConfigFilename() const { if (currentIndex()<0) return ""; else return itemData(currentIndex()).toString(); };

        /** \brief select a given acquisition plugin configuration */
        void setCurrentConfig(QString name);

        void setStopResume(QFAcquisitionConfigComboBoxStartResume* stopresume);
    public slots:
        /*! \brief may be used to connect this widget to a QFAcquisitionComboBox

            If called it updates the widget's contents.
         */
        void deviceChanged(QFExtension* extension, QFExtensionAcquisition* cam, int camIdx);

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
        QFExtensionAcquisition* m_cam;
        QFAcquisitionConfigComboBoxStartResume* m_stopresume;
        int m_camIdx;
        static QFAcquisitionConfigComboBoxNotifier* m_notifier;

    private:
};





/*! \brief this widget contains a QFAcquisitionConfigComboBox and some buttons to edit it's contents
    \ingroup qf3lib_widgets

 */
class QFWIDLIB_EXPORT QFAcquisitionConfigEditorWidget : public QWidget {
        Q_OBJECT
    public:
    QFAcquisitionConfigEditorWidget(QWidget* parent=NULL, QString configDirectory=QString(""));
        virtual ~QFAcquisitionConfigEditorWidget();

        void init(QString configDirectory);
        /** \brief connect the given combobox to this combobox and initialize this with the current item of combo */
        void connectTo(QFAcquisitionComboBox* combo) { combobox->connectTo(combo); };

        /** \brief return the filename for the currently selected camera configuration */
        QString currentConfigFilename() const { return combobox->currentConfigFilename(); };
        /** \brief return the name (not the full filename with path) for the currently selected camera configuration */
        QString currentConfigName() const { return combobox->currentText(); };
        /** \brief select a given camera configuration */
        void setCurrentConfig(QString name) { combobox->setCurrentConfig(name); }
        /** \brief this QAction triggers the configuration of the currently selected camera settings, provided for convenience, if you need this functionality at additional buttons in your GUI */
        QAction* configAction() const { return actConfig; }

        void setStopResume(QFAcquisitionConfigComboBoxStartResume* stopresume) {
            combobox->setStopResume(stopresume);
        };

    public slots:
        /*! \brief may be used to connect this widget to a QFAcquisitionComboBox

            If called it updates the widget's contents.
         */
        void cameraChanged(QFExtension* extension, QFExtensionAcquisition* cam, int camIdx) {
            combobox->deviceChanged(extension, cam, camIdx);
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
        QFAcquisitionConfigComboBox* combobox;
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

#endif // QFACQUISITIONCONFIGCOMBOBOX_H
