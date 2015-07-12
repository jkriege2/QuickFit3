/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef TICKTOCK_H
#define TICKTOCK_H

#ifndef __WINDOWS__
#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#define __WINDOWS__
#endif
#endif

#ifdef __WINDOWS__
  #include "highrestimer.h"
#else
  #include <time.h>
#endif
#include<stdio.h>

#ifdef QT_CORE_LIB
#include <QDebug>
#endif


/** \brief this class provides a time measurement service
 * \ingroup tool_classes
 *
 * This provides timing services as tools for other classes simply make this the parent class
 * of another class and you can call tick() and tock(). After the call of tock() the protected member
 * runtime will contain the time between the last tick() and tock() in seconds.
 *
 * On Windows this implementation uses the class HighResTimer to get a high-resolution measurement of the
 * time. On other systems it uses the standard C library timing routines.
 */
class  TickTock
{
    public:
        /** Default constructor */
        TickTock() { runtime=0; intAvgTimingTicTock=0; }
        /** Default destructor */
        virtual ~TickTock() {}

        /** \return duration in seconds */
        inline double get_duration() { return runtime; };
    protected:

        /** \brief run-time of the run() method in seconds */
        double runtime;
        #ifdef __WINDOWS__
            /** \brief timer class for windows (high resolution) */
            HighResTimer ht;
        #else
            /** \brief used for runtim measurement */
            clock_t start;

            /** \brief used for runtim measurement */
            clock_t end;
        #endif

        /** \brief call this at the beginning of the runtime meausrement */
        inline void tick() {
            #ifdef __WINDOWS__
                ht.start();
            #else
                start=clock();
            #endif
        };

        /** \brief call this at the end of the runtime measurement. the time since the last call of tick() will be stored in runtime */
        inline void tock() {
            #ifdef __WINDOWS__
                runtime = ht.get_time()/1e6;
            #else
                end = clock();
                runtime = ((double) (end - start)) / CLOCKS_PER_SEC;
            #endif
        };

        /** \brief call this at the end of the runtime measurement. the time since the last call of tick() will be stored in runtime */
        inline void tockPrint(const char* message=NULL) {
            tock();
#ifdef QT_CORE_LIB
            if (message) {
                qDebug()<<"tock ["<<message<<"]: "<<(get_duration()*1000.0)<<"ms";
            } else {
                qDebug()<<"tock: "<<(get_duration()*1000.0)<<"ms";
            }
#else
            if (message) {
                printf("tock [%s]: %.4lfms\n", message, get_duration()*1000.0);
            } else {
                printf("tock: %.4lfms\n", get_duration()*1000.0);
            }
#endif
        };

        /** \brief call this for averaging the times in runtime. the time since the last call of tick() will be stored in runtime */
        inline void tockAvg(double& averageTime) {
            tock();
            double t=get_duration()*1000.0;
            const double alpha=0.8;
            averageTime=averageTime*alpha+(1.0-alpha)*t;
        };

        /** \brief call this for averaging the times in runtime. the time since the last call of tick() will be stored in runtime */
        inline void tockAvg() {
            tockAvg(intAvgTimingTicTock);
        }

        /** \brief call this at the end of the runtime measurement. the time since the last call of tick() will be stored in runtime */
        inline void tockAvgPrint(double& averageTime, const char* message=NULL) {
            tock();
            double t=get_duration()*1000.0;
            const double alpha=0.8;
            averageTime=averageTime*alpha+(1.0-alpha)*t;
#ifdef QT_CORE_LIB
            if (message) {
                qDebug()<<"tock ["<<message<<"]: "<<t<<"ms   (avg="<<averageTime<<"ms)";
            } else {
//                qDebug()<<"tock: "<<t<<"ms   (avg="<<averageTime<<"ms)";
            }
#else
            if (message) {
                printf("tock [%s]: %8.4lfms   (avg=%.4lfms)\n", message,t,averageTime);
            } else {
                printf("tock: %8.4lfms  (avg=%.4lfms)\n", t,averageTime);
            }
#endif
        };
        /** \brief call this at the end of the runtime measurement. the time since the last call of tick() will be stored in runtime */
        inline void tockAvgPrint(const char* message=NULL) {
            tockAvgPrint(intAvgTimingTicTock, message);
        }
    private:
        double intAvgTimingTicTock;
};


/** \brief implementation of TickTock with public access
 * \ingroup tool_classes
 */
class QFLIB_EXPORT PublicTickTock: public TickTock {
    public:
        /** Default constructor */
        PublicTickTock(): TickTock() {}
        /** Default destructor */
        virtual ~PublicTickTock() {}

        inline void tick() {
            TickTock::tick();
        }

        inline void tock() {
            TickTock::tock();
        }

        inline void tockPrint(const char* message=NULL) {
            TickTock::tockPrint(message);
        }

        inline void tockAvg() {
            TickTock::tockAvg();
        }

        inline void tockAvgPrint(const char* message=NULL) {
            TickTock::tockAvgPrint(message);
        }

};

#endif // TICKTOCK_H
