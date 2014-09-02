/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#ifndef QFESPIMB040CONFIGCOMBOBOX_H
#define QFESPIMB040CONFIGCOMBOBOX_H

#include <QComboBox>
#include <QIcon>
#include <QString>
#include "qtriple.h"
#include "qenhancedcombobox.h"

#include "qfespimb040opticssetup.h"

/*! \brief a combobox which displays a set of configuration files from a given directory
    \ingroup qf3ext_spimb040

    the displayed name is read from the property \c name from the config file,
    when opened using QSettings. if also the property \c icon is set, the file
    referenced therein will be used as the combobox item icon. Otherwise no icon, or
    the QIcon, specified in \a icon are used

  */
class QFESPIMB040ConfigComboBox : public QEnhancedComboBox {
        Q_OBJECT
    public:
        explicit QFESPIMB040ConfigComboBox(QWidget *parent = 0);
        virtual ~QFESPIMB040ConfigComboBox();

        QString currentConfigFilename() const;
        QString currentConfig() const;
        void setIcon(QIcon icon);
        void init(QString baseDir, QString extension=QString("ini"));
        void updateItems();
        void updateItems(const QString& newCurrent);
        QStringList getConfigs() const;
    signals:
        /** \brief called when the user selects a new configuration.
         *
         *  \param filename contains the full filename including the absolute path.
         */
        void currentConfigChanged(QString filename);

        /** \brief emitted when the list of lightpath configs changes */
        void configsChanged(QFESPIMB040OpticsSetupItems configs);
    public slots:
        /** \brief select the given configuration from a given filename
         *
         *  \param filename the config file to load: either a filename relative to the given main path
         *                  or an absolute filename (then it is not guaranteed that the file can be selected!!!)
         */
        void setCurrentConfigFilename(QString filename);
        /** \brief select the given configuration from a given name
         *
         *  \param config the name of the configuration to select
         */
        void setCurrentConfig(QString config);
    protected:
        QIcon icon;
        QString baseDir;
        QString m_ext;
        bool updating;
    protected slots:
        void cmbCurrentIndexChanged(int index);
        QString getConfigName(QString filename) const;
        QIcon getConfigIcon(QString filename) const;

};

#endif // QFESPIMB040CONFIGCOMBOBOX_H
