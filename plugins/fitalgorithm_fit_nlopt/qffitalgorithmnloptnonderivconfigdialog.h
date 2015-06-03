/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:40:44 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3508 $)

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

#ifndef QFFitAlgorithmNLOptNonDerivConfigDialog_H
#define QFFitAlgorithmNLOptNonDerivConfigDialog_H

#include <QDialog>
#include "qffitalgorithm.h"

namespace Ui {
    class QFFitAlgorithmNLOptNonDerivConfigDialog;
}

class QFFitAlgorithmNLOptNonDerivConfigDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFFitAlgorithmNLOptNonDerivConfigDialog(QWidget *parent = 0);
        ~QFFitAlgorithmNLOptNonDerivConfigDialog();
        
        /** \brief initialize the dialog from the parameters in the given algorithm */
        void init(const QFFitAlgorithm* algorithm);
        /** \brief store the parameters to the given algorithm */
        void storeParams(QFFitAlgorithm* algorithm) const;
    private:
        Ui::QFFitAlgorithmNLOptNonDerivConfigDialog *ui;
};

#endif // QFFitAlgorithmNLOptNonDerivConfigDialog_H
