/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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





#ifndef QFSTAGECOMBOBOX_H
#define QFSTAGECOMBOBOX_H

#include <QComboBox>
#include "qfextensionmanager.h"
#include "qfextensionlinearstage.h"
#include "qfextension.h"
#include "libwid_imexport.h"
#include "qenhancedcombobox.h"
#include "qfmanyfilessettings.h"
/*! \brief QComboBox which is filled with all available stages
    \ingroup qf3lib_widgets


 */
class QFWIDLIB_EXPORT QFStageComboBox : public QEnhancedComboBox {
        Q_OBJECT
    public:
        /** \briefclass cosntructor which does NOT initialize the contents (call init() separately!!!) */
        QFStageComboBox(QWidget* parent=NULL);
        /** \briefclass cosntructor which initializes the contents from the given extManager */
        QFStageComboBox(QFExtensionManager* extManager, QWidget* parent);
        virtual ~QFStageComboBox();
        /** \brief save the currently selected stage to a QSettings object */
        void storeSettings(QSettings& settings, QString prefix=QString("")) const;
        void storeSettings(QFManyFilesSettings& settings, QString prefix=QString("")) const;
        /** \brief load the currently selected stage from a QSettings object */
        void loadSettings(QSettings& settings, QString prefix=QString(""));
        void loadSettings(QFManyFilesSettings& settings, QString prefix=QString(""));

        /** \brief initialize the available stages from the given QFExtensionManager */
        void init(QFExtensionManager* extManager);
        /** \brief currently selected stage as QFExtension */
        QFExtension* currentExtension() const;
        QObject* currentExtensionObject() const;
        /** \brief currently selected stage as QFExtensionLinearStage */
        QFExtensionLinearStage* currentExtensionLinearStage() const;
        /** \brief currently selected stage axis ID */
        int currentAxisID() const;
    public slots:
        /** \brief update the filled combobox */
        void findExtensions();
    protected:
        /** \brief list of all available QFExtensionLinearStage plugins, initialized by findExtensions() */
        QList<QObject*> stages;

        /** \brief ID of the current item in stages */
        int currentExtensionID() const;

        QFExtensionManager* m_extManager;
    private:
};

#endif // QFSTAGECOMBOBOX_H
