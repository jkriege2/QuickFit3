/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate: 2014-06-24 22:32:50 +0200 (Di, 24 Jun 2014) $  (revision $Rev: 3291 $)

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





/**
 * \defgroup gnuplot_tools tool functions for libtiff
 * \ingroup tools
 *
 * Functions in this group allow to cooperate with <a href="http://www.gnuplot.info">GnuPlot</a> more easily.
 */

/** \file gnuplot_tools.h
 *  \ingroup gnuplot_tools
 *
 */

#ifndef gnuplot_tools_H_INCLUDED
#define gnuplot_tools_H_INCLUDED


#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <string>

/*! \brief write a float image into a binary file
    \ingroup gnuplot_tools

    \param filename name of the binary file to create
    \param image points to the image data, in row-major format
    \param width width of the  image in pixels
    \param height height of the  image in pixels
	\param xvals x-positions for the matrix columns
	\param yvals y-positions for the matrix rows
    \return \c true on success, \c false on failure
*/
bool gnuplotWriteBinaryArray(const char* filename, float* image, int width, int height, float* xvals=NULL, float* yvals=NULL);

/*! \brief write a float image into a binary file
    \ingroup gnuplot_tools

    \param filename name of the binary file to create
    \param image points to the image data, in row-major format
    \param width width of the  image in pixels
    \param height height of the  image in pixels
	\param xvals x-positions for the matrix columns
	\param yvals y-positions for the matrix rows
    \return \c true on success, \c false on failure
*/
bool gnuplotWriteBinaryArray(const char* filename, double* image, int width, int height, double* xvals=NULL, double* yvals=NULL);



#endif // gnuplot_tools_H_INCLUDED
