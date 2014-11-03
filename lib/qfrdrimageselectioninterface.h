/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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



#ifndef QFRDRIMAGESELECTIONINTERFACE_H
#define QFRDRIMAGESELECTIONINTERFACE_H

#include <QtPlugin>
#include <QString>
#include <QSet>

/*! \brief interface for a selection on an image (i.e. a binary image saying which pixel is selected and which not)
    \ingroup qf3rdrdp_fcs


*/
class QFRDRImageSelectionInterface {
    public:
        virtual ~QFRDRImageSelectionInterface() {};
        /** \brief returns the number of stored selections */
        virtual int getImageSelectionCount() const=0;
        /** \brief returns the name of a stored selections */
        virtual QString getImageSelectionName(int selection) const=0;
        /** \brief return the given stored selection
         *
         *  \param selection specifies the selection to load (has to be between 0 and getImageSelectionCount() )
         */
        virtual bool* loadImageSelection(int selection) const=0;
        /** \brief store the current selection under the given name (append it) */
        virtual void addImageSelection(bool* selection, const QString& name=QString(""))=0;
        /** \brief delete the specified selection */
        virtual void deleteImageSelection(int selection)=0;
        /** \brief set the name of the specified selection */
        virtual void setImageSelectionName(int selection, const QString& name)=0;

        /** \brief returns the width (in pixels) of an image selection */
        virtual int getImageSelectionWidth() const=0;
        /** \brief returns the height (in pixels) of an image selection */
        virtual int getImageSelectionHeight() const=0;

};

Q_DECLARE_INTERFACE( QFRDRImageSelectionInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRImageSelectionInterface/1.1")

#endif // QFRDRIMAGESELECTIONINTERFACE_H
