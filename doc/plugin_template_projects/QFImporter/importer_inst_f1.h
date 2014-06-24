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


#ifndef HEADER_F1_H
#define HEADER_F1_H
#include "qfpluginimporters.h"
#include "qfimporter.h"




/*! \brief QFImporter class for fit function
    \ingroup doxygen_GROUPNAME

*/
class QFFitAlgorithmInst_F1: public QFImporter {
    public:
        QFFitAlgorithmInst_F1();
        virtual ~QFFitAlgorithmInst_F1() {}
        /*! \copydoc QFFitFunction::filter()   */
        virtual QString filter() const ;
        /*! \copydoc QFFitFunction::formatName()   */
        virtual QString formatName() const ;
};

#endif // HEADER_F1_H
