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



#ifndef SHIFTREGISTER_H
#define SHIFTREGISTER_H
#include <string>
#include <sstream>
#include <cstdlib>


#ifndef __LINUX__
# if defined(linux)
#  define __LINUX__
# endif
#endif

#ifdef __LINUX__
#include "malloc.h"
#include "stdlib.h"
#endif

#ifndef QF_ALIGNMENT_BYTES
#define QF_ALIGNMENT_BYTES 32
#endif






/*! \brief malloc() for use herein (aligned on some systems!)
    \ingroup tools_math_array

*/
static inline void* ShiftRegister__malloc(size_t size) {
#ifdef __LINUX__
    #if  !defined(QF_DONT_USE_ALIGNED_MALLOC)
    return aligned_alloc(QF_ALIGNMENT_BYTES, size);
    #else
    return malloc(size);
    #endif
#else
    #if  !defined(QF_DONT_USE_ALIGNED_MALLOC)
    return _aligned_malloc(size, QF_ALIGNMENT_BYTES);
    #else
    return malloc(size);
    #endif
#endif
}

/*! \brief calloc() for use herein (aligned on some systems!)
    \ingroup tools_math_array

*/
static inline void* ShiftRegister__calloc(size_t num, size_t size) {
    void* res=ShiftRegister__malloc(num*size);
    memset(res, 0, num*size);
    return res;
}

/*! \brief free() for use herein (aligned on some systems!)
    \ingroup tools_math_array

*/
static inline void ShiftRegister__free(void* data) {
    if (!data) return;
#ifdef __LINUX__
    #if !defined(QF_DONT_USE_ALIGNED_MALLOC)
    free(data);
    #else
    free(data);
    #endif
#else
    #if !defined(QF_DONT_USE_ALIGNED_MALLOC)
    _aligned_free(data);
    #else
    free(data);
    #endif
#endif
}

/*! \defgroup tools_containers_shiftreg Shift Register (Ring-Buffer / FIFO)
    \ingroup tools_containers
 */

/** \brief this class represents a linear shift register with a fixed number of items
 *         in it. It supports the basic shift register (FIFO) operations + some more and
 *         is speed optimized.
 *  \ingroup tools_containers_shiftreg
 *
 * This class is implemented as a template, so you can implement shift registers with different data types and sizes.
 * For all methods to work the type of the class/data type used for the template has to support these operations:
 *   - \c C=0; (clear() method)
 *   - \c C+C (addToExternal() function)
 *   - \c C+C*C (multiplyAddToExternal() function)
 * .
 *
 * The class is a template which allows to implement shift registers with different lengths.
 *
 * The FIFO is internally represented by an array which has a ring closure by means
 * of modulo arithmetics. Basically the class contains two data members: The array istelf
 * (called data) and the position where to write the item that is pushed next.
 *
 * Here is some example code thet implements a shift register with 5 entries, consisting of
 * doubles:
 * \code
 * #include <iostream>
 * #include <cstdio>
 * #include "shiftregister.h"
 *
 * using namespace std;
 *
 * int main()
 * {
 *     ShiftRegister<double> sr(5);
 *     for (int i=0; i<16; i++) {
 *         cout<<sr.to_string();
 *         cout<<"    peek="<<sr.peek();
 *         cout<<"    push_right("<<i+1<<")";
 *         sr.push_right(i+1);
 *         sr.multiplyAddToExternal(d, 2.0);
 *         cout<<"    "<<sr.to_string()<<std::endl;
 *     }
 *     cout<<endl<<endl;
 *     for (int i=0; i<5; i++) {
 *         cout<<d[i]<<", ";
 *     }
 *     return 0;
 * }
 * \endcode
 */
template <typename C>
class ShiftRegister {
    public:


    protected:
        /** \brief this array represents the data in the shift register */
        C* data;
        /** \brief points to the current writing position */
        size_t currentWriting;
        /** \brief the number of items in the shift register */
        size_t items;
    public:
        /** Default constructor */
        explicit inline ShiftRegister(size_t items){
            this->items=items;
            data=(C*)ShiftRegister__malloc(items*sizeof(C));
            clear();
        };

        /** Default constructor */
        explicit inline ShiftRegister(const ShiftRegister& cop){
            this->items=cop.items;
            data=(C*)ShiftRegister__malloc(items*sizeof(C));
            currentWriting=cop.currentWriting;
            for (size_t i=0; i<items; i++) {
                data[i]=cop.data[i];
            }
        };

        inline ShiftRegister& operator=(const ShiftRegister& cop){
            this->items=cop.items;
            ShiftRegister__free(data);
            data=(C*)ShiftRegister__malloc(items*sizeof(C));
            currentWriting=cop.currentWriting;
            size_t i;
            //#pragma omp simd
            for (i=0; i<items; i++) {
                data[i]=cop.data[i];
            }
            return *this;
        };

        /** Default destructor */
        virtual ~ShiftRegister(){
            ShiftRegister__free(data);
        };

        /** \brief returns the i-th item in the register ("parallel read-out") */
        inline C& operator[](size_t i) {
            return data[(items+currentWriting-i-1)%items];
        };

        /** \brief clears the register (i.e. sets all entries to 0) */
        inline void clear() {
            size_t i;
            //#pragma omp simd
            for (i=0; i<items; i++) {
                data[i]=0;
            }
            currentWriting=0;
        };

        /** \brief add the contents of the internal array to the contents of an external array, i.e.
         *         data[i]=data[i]+shiftReg[i].
         *
         * This method is needed for some correlator algorithms.
         * It also provides the possibility to start from an arbitrary position \a start inside the
         * register contents and to only work through the next \a count items!
         */
        template <typename EC>
        inline void addToExternal(EC* edata, size_t start, size_t count) {
            size_t i;
            //#pragma omp simd
            for (i=0; i<count; i++) {
                edata[i]=edata[i]+(EC)operator[](start+i);
            }
        };

        /** \brief add the contents of the internal array to the contents of an external array, i.e.
         *         data[i]=data[i]+shiftReg[i].
         *
         * This method is needed for some correlator algorithms.
         */
        template <typename EC>
        inline void addToExternal(EC* edata) {
            size_t i;
            //#pragma omp simd
            for (i=0; i<items; i++) {
                edata[i]=edata[i]+(EC)operator[](i);
            }
        };

        /** \brief add the contents of the internal array, multiplied by a constant, to the contents
         *         of an external array, i.e. \c data[i]=data[i]+shiftReg[i].
         *
         * This method is needed for some correlator algorithms.
         * It also provides the possibility to start from an arbitrary position \a start inside the
         * register contents and to only work through the next \a count items!
         */
        template <typename EC>
        inline void multiplyAddToExternal(EC* edata, C con, size_t start, size_t count) {
            size_t i;
            //#pragma omp simd
            for (i=0; i<count; i++) {
                edata[i]=edata[i]+(EC)(con*operator[](start+i));
            }
        };

        /** \brief add the contents of the squared difference between the internal array and a constant, to the contents
         *         of an external array, i.e. \c data[i]=data[i]+(shiftReg[i]-constant)^2.
         *
         */
        template <typename EC>
        inline void squareDistanceAddToExternal(EC* edata, C con, size_t start, size_t count) {
            size_t i;
            //#pragma omp simd
            for (i=0; i<count; i++) {
                EC r=(EC)(con-operator[](start+i));
                edata[i]=edata[i]+r*r;
            }
        };
        /** \brief add the contents of the internal array, multiplied by a constant, to the contents
         *         of an external array, i.e. \c data[i]=data[i]+shiftReg[i].
         *
         * This method is needed for some correlator algorithms.
         */
        template <typename EC>
        inline void multiplyAddToExternal(EC* edata, C con) {
            size_t i;
            //#pragma omp simd
            for (i=0; i<items; i++) {
                edata[i]=edata[i]+(EC)(con*operator[](i));
            }
        };
        /** \brief add the contents of the squared difference between the internal array and a constant, to the contents
         *         of an external array, i.e. \c data[i]=data[i]+(shiftReg[i]-constant)^2.
         */
        template <typename EC>
        inline void squareDistanceAddToExternal(EC* edata, C con) {
            size_t i;
            //#pragma omp simd
            for (i=0; i<items; i++) {
                EC r=(EC)(con-operator[](i));
                edata[i]=edata[i]+r*r;
            }
        };


       /** \brief add the contents of the internal array to the contents of an external array, i.e.
         *         data[i]=data[i]+shiftReg[i].
         *
         * This method is needed for some correlator algorithms.
         * It also provides the possibility to start from an arbitrary position \a start inside the
         * register contents and to only work through the next \a count items!
         */
        inline void addToExternalDouble(double* edata, size_t start, size_t count) {
            size_t i;
            //#pragma omp simd
            for (i=0; i<count; i++) {
                edata[i]=edata[i]+operator[](start+i);
            }
        };

        /** \brief add the contents of the internal array to the contents of an external array, i.e.
         *         data[i]=data[i]+shiftReg[i].
         *
         * This method is needed for some correlator algorithms.
         */
        inline void addToExternalDouble(double* edata) {
            size_t i;
            //#pragma omp simd
            for (i=0; i<items; i++) {
                edata[i]=edata[i]+operator[](i);
            }
        };

        /** \brief add the contents of the internal array, multiplied by a constant, to the contents
         *         of an external array, i.e. \c data[i]=data[i]+shiftReg[i].
         *
         * This method is needed for some correlator algorithms.
         * It also provides the possibility to start from an arbitrary position \a start inside the
         * register contents and to only work through the next \a count items!
         */
        inline void multiplyAddToExternalDouble(double* edata, C con, size_t start, size_t count) {
            size_t i;
            //#pragma omp simd
            for (i=0; i<count; i++) {
                edata[i]=edata[i]+con*operator[](start+i);
            }
        };

        /** \brief add the contents of the internal array, multiplied by a constant, to the contents
         *         of an external array, i.e. \c data[i]=data[i]+shiftReg[i].
         *
         * This method is needed for some correlator algorithms.
         */
        inline void multiplyAddToExternalDouble(double* edata, C con) {
            size_t i;
            //#pragma omp simd
            for (i=0; i<items; i++) {
                edata[i]=edata[i]+con*operator[](i);
            }
        };


        /** \brief push one item into the ShiftReg from left (begin) and remove the last (right-most) one
         *
         * The deleted item at the right-hand end of the SR is returned
         */
        inline C push_right(C newData) {
            C a=data[currentWriting];
            data[currentWriting]=newData;
            currentWriting=(currentWriting+1)%items;
            return a;
        };

        /** \brief push one item into the ShiftReg from right (end) and remove the first (left-most) one
         *
         * The deleted item at the left-hand end of the SR is returned
         */
        inline C push_left(C newData) {
            size_t cw1=(items+currentWriting-1)%items;
            C a=data[cw1];
            data[cw1]=newData;
            currentWriting=cw1;
            return a;
        };

        /** \brief read the last item in the FIFO (i.e. the next item to be discarded) */
        inline C peek() {
            return operator[](items-1);
        };

        /** \brief returns the content s of the shoft register as a string */
        inline std::string to_string() {
            std::ostringstream ost;
            ost<<"[ ";
            for (size_t i=0; i<items; i++) {
                if (i>0) ost<<", ";
                ost<<this->operator[](i);
            }
            ost<<" ]";
            return ost.str();
        };










};

#endif // SHIFTREGISTER_H
