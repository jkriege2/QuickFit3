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



#ifndef QFRDRIMAGESTACKINTERFACE_H
#define QFRDRIMAGESTACKINTERFACE_H




#include <QtPlugin>
#include <stdint.h>
#include <QString>
#include <QList>

/*! \brief represents a common data interface for access to image stacks (1D, 2D, 3D, 4D)
    \ingroup qf3rdrdp_imaging_fcs

    The functions in this interface give access to a 4D image stack of type double. The
    axes are named:
      - x [0..width-1]
      - y [0..height-1]
      - time t [0..frames-1]
      - channel [0..channels-1]
    .
    Each of the axes may be transformed into a real coordinate value by multiplying with a given
    factor. The name of the resultant unit can also be extracted from this interface.


    The data is stored in row-major format, so the layout lookslike this:
    \verbatim
+-------------------------------------+           ****
| row 1               [0 ... width]   | I         *
| row 2               [0 ... width]   | M         * C
|  .                                  | A         * H
|  .                                  | G         * A
|  .                                  | E         * N
| row height-1        [0 ... width]   | 0         * N
+-------------------------------------+           * E
| row 1               [0 ... width]   | I         * L
| row 2               [0 ... width]   | M         *
|  .                                  | A         * 0
|  .                                  | G         *
|  .                                  | E         *
| row height-1        [0 ... width]   | 1         *
+-------------------------------------+           *
.                                     .           *
.                                     .           *
.                                     .           *
+-------------------------------------+           *
| row 1               [0 ... width-1] | I         *
| row 2               [0 ... width-1] | M         *
|  .                                  | A         *
|  .                                  | G         *
|  .                                  | E         *
| row height-1        [0 ... width-1] | frames-1  *
+-------------------------------------+           ****

+-------------------------------------+           ****
| row 1               [0 ... width-1] | I         *
| row 2               [0 ... width-1] | M         * C
|  .                                  | A         * H
|  .                                  | G         * A
|  .                                  | E         * N
| row height-1        [0 ... width-1] | 0         * N
+-------------------------------------+           * E
| row 1               [0 ... width-1] | I         * L
| row 2               [0 ... width-1] | M         *
|  .                                  | A         * 1
|  .                                  | G         *
|  .                                  | E         *
| row height-1        [0 ... width-1] | 1         *
+-------------------------------------+           *
.                                     .           *
.                                     .           *
.                                     .           *
+-------------------------------------+           *
| row 1               [0 ... width-1] | I         *
| row 2               [0 ... width-1] | M         *
|  .                                  | A         *
|  .                                  | G         *
|  .                                  | E         *
| row height-1        [0 ... width-1] | frames-1  *
+-------------------------------------+           ****

+-------------------------------------+           ****
.                                     .           .
.                                     .           .
.                                     .           .
.                                     .           .
    \endverbatim

*/
class QFRDRImageStackInterface {
    public:
        virtual ~QFRDRImageStackInterface() {}

        /** \brief returns the number of image stacks available through this interface */
        virtual int getImageStackCount() const =0;

        /** \brief returns the number of available preview images */
        virtual uint32_t getImageStackFrames(int stack) const =0;
        /** \brief returns the width of the given image */
        virtual int getImageStackWidth(int stack) const=0;
        /** \brief returns the height of the given image */
        virtual int getImageStackHeight(int stack) const=0;
        /** \brief returns the height of the given image */
        virtual int getImageStackChannels(int stack) const=0;
        /** \brief returns a pointer to the given frame in the video */
        virtual double* getImageStack(int stack, uint32_t frame=0, uint32_t channel=0) const=0;

        /** \brief returns a factor that scales the image stack x-dimension [0..width-1] to real coordinates */
        virtual double getImageStackXUnitFactor(int stack) const=0;
        /** \brief returns the unit name of the image stack x-dimension */
        virtual QString getImageStackXUnitName(int stack) const=0;
        /** \brief returns the name of the image stack x-dimension */
        virtual QString getImageStackXName(int stack) const=0;

        /** \brief returns a factor that scales the image stack y-dimension [0..height-1] to real coordinates */
        virtual double getImageStackYUnitFactor(int stack) const=0;
        /** \brief returns the unit name of the image stack y-dimension */
        virtual QString getImageStackYUnitName(int stack) const=0;
        /** \brief returns the name of the image stack y-dimension */
        virtual QString getImageStackYName(int stack) const=0;

        /** \brief returns a factor that scales the image stack t-dimension [0..frames-1] to real coordinates */
        virtual double getImageStackTUnitFactor(int stack) const=0;
        /** \brief returns the unit name of the image stack t-dimension */
        virtual QString getImageStackTUnitName(int stack) const=0;
        /** \brief returns the name of the image stack t-dimension */
        virtual QString getImageStackTName(int stack) const=0;

        /** \brief returns a factor that scales the image stack channel-dimension [0..channels-1] to real coordinates */
        virtual double getImageStackCUnitFactor(int stack) const=0;
        /** \brief returns the unit name of the image stack channel-dimension */
        virtual QString getImageStackCUnitName(int stack) const=0;
        /** \brief returns the name of the image stack channel-dimension */
        virtual QString getImageStackCName(int stack) const=0;

        /** \brief returns the name of the given channel (e.g. "red", "green", ...) */
        virtual QString getImageStackChannelName(int stack, int channel) const=0;

        /** \brief returns the name of the given timepoint (e.g. "1s", "1.23s", "z=45µm", ...) */
        virtual QString getImageStackTimepointName(int stack, int t) const=0;


        /** \brief returns a description for the video */
        virtual QString getImageStackDescription(int stack) const=0;

};

Q_DECLARE_INTERFACE( QFRDRImageStackInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRImageStackInterface/1.0")

#endif // QFRDRIMAGESTACKINTERFACE_H
