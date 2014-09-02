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

#ifndef QFESPIMB040EXPERIMENTDESCRIPTION_H
#define QFESPIMB040EXPERIMENTDESCRIPTION_H

#include <QWidget>
#include <QSettings>

namespace Ui {
    class QFESPIMB040ExperimentDescription;
}
/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg): experiment description widget
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040ExperimentDescription : public QWidget {
        Q_OBJECT
    public:
        explicit QFESPIMB040ExperimentDescription(QWidget *parent = 0);
        ~QFESPIMB040ExperimentDescription();
        /** \brief load settings */
        void loadSettings(QSettings& settings, QString prefix);
        /** \brief save settings */
        void storeSettings(QSettings& settings, QString prefix) const;

        QString getID() const;

        /*! \brief return a map containing the experiment description
         */
        QMap<QString, QVariant> getDescription() const;
        QString getTitle() const;
        QString getExperimenter() const;
protected slots:
        void updateTime();
        void on_btnClearAll_clicked();
        void on_btnSaveTemplate_clicked();
        void on_btnLoadTemplate_clicked();
    private:
        Ui::QFESPIMB040ExperimentDescription *ui;
};

#endif // QFESPIMB040EXPERIMENTDESCRIPTION_H
