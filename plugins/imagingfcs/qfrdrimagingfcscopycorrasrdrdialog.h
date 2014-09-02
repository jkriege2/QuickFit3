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

#ifndef QFRDRIMAGINGFCSCOPYCORRASRDRDIALOG_H
#define QFRDRIMAGINGFCSCOPYCORRASRDRDIALOG_H

#include <QDialog>

namespace Ui {
class QFRDRImagingFCSCopyCorrAsRDRDialog;
}

class QFRDRImagingFCSCopyCorrAsRDRDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QFRDRImagingFCSCopyCorrAsRDRDialog(bool isFCCS, QWidget *parent = 0);
    ~QFRDRImagingFCSCopyCorrAsRDRDialog();
    bool getCopyACF() const;
    bool getCopyAllRuns() const;
    bool getCreateSelection() const;
    QString getSelectionName() const;
    
private:
    Ui::QFRDRImagingFCSCopyCorrAsRDRDialog *ui;
};

#endif // QFRDRIMAGINGFCSCOPYCORRASRDRDIALOG_H
