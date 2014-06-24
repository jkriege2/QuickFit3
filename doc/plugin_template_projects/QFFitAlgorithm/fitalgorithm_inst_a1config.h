/*
Copyright (c) 2014
	
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


#ifndef HEADER_A1_CONFIG_H
#define HEADER_A1_CONFIG_H

#include <QDialog>
#include <QLabel>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include "qffitalgorithm.h"

namespace Ui {
    class QFFitAlgorithmInst_A1ConfigDialog;
}


/*! \brief This class represents a configuration dialog for ...
    \ingroup doxygen_GROUPNAME
 */
class QFFitAlgorithmInst_A1ConfigDialog : public QDialog {
        Q_OBJECT
    public:
        /** \brief class constructor */
        QFFitAlgorithmInst_A1ConfigDialog(QWidget *parent = 0);
		~QFFitAlgorithmInst_A1ConfigDialog();

        /** \brief initialize the dialog from the parameters in the given algorithm */
        void init(const QFFitAlgorithm* algorithm);
        /** \brief store the parameters to the given algorithm */
        void storeParams(QFFitAlgorithm* algorithm) const;
    private:
        Ui::QFFitAlgorithmInst_A1ConfigDialog *ui;
};

#endif //HEADER_A1_CONFIG_H
