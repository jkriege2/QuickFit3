#ifndef VERSION_H
#define VERSION_H

#include <QtGlobal>
#include "compiledate.h"
#include "gitversion.h"
#include "compiler.h"
#include "qfversion.h"

#define VERSION_FULL "3.0"
#define QF_VERSION VERSION_FULL

#define VERSION_STATUS "beta"

#define QF_THANKS_TO "Jan Buchholz, Dr. Tabea Elbel, Dr. Nicolas Dross, Dr. Gy\xF6rgy V\xE1mosi, Dr. Vera B\xF6hm, Prof. Dr. J\xF6rg Langowski, Dr. Katalin T\xF2th, PD Dr. Christoh Garbe, Prof. Dr. Thorsten Wohland"

#define QF_COPYRIGHT QString("&copy; 2010-%1 by Jan W. Krieger").arg(QDate::currentDate().year())

#define QF_AUTHOR "Jan W. Krieger"

#define QF_EMAIL "j.krieger@dkfz.de"

#define QF_MAILLIST "quickfit@dkfz.de"

#define QF_MAILLIST_REQUEST "mailto:quickfit-request@dkfz.de?subject=subscribe&amp;body=subscribe"

#define QF_WEBLINK "http://www.dkfz.de/Macromol/quickfit/"
#define QF_SOURCEWEBLINK "https://github.com/jkriege2/QuickFit3"

//#define QF_UPDATEXMLURL "http://www.dkfz.de/Macromol/quickfit/update.xml"
#define QF_UPDATEXMLURL "http://raw.githubusercontent.com/jkriege2/QuickFit3/master/update.xml"
#define QF_UPDATESPECIALXMLURL "http://www.dkfz.de/Macromol/quickfit/update.xml"
//#define QF_UPDATESPECIALXMLURL "http://www.dkfz.de/Macromol/quickfit/updatespecial.xml"

#define QF_LICENSE QT_TR_NOOP("<b>parts GNU Lesser General Public License (LGPL >=2.1) and parts GNU General Public License (GPL >=3.0) </b><br><br><small><i>Also: This is POSTCARD-ware, so if you like the program and use it, please let us know by sending a nice postcard from your place on the world to:</i><br>DKFZ (German Cancer Research Center)<br>Dept. B040 (Biophysics of Macromolecules): Jan W.  Krieger<br>Im Neuenheimer Feld 580<br>D-69120 Heidelberg,  GERMANY</small>")

#define QF_CITATION_HTML QString("Jan Wolfgang Krieger, J&ouml;rg Langowski (2010-%6): <b>QuickFit %1 (compiled: %2, SVN: %3): A data evaluation application for biophysics</b>, <i>[web page] <a href=\"%4\">%4</a> [Accessed on %5]</i>").arg(QF_VERSION).arg(qfInfoCompileDate()).arg(qfInfoSVNVersion()).arg(QF_WEBLINK).arg(QDate::currentDate().toString("yyyy/MM/d")).arg(QDate::currentDate().year())
#define QF_CITATION_BIBTEX QString("@misc{quickfit3,\n" \
    "  author={Jan Wolfgang Krieger and J{\\\"o}rg Langowski},\n" \
    "  title={{QuickFit %1} (compiled: %2, {SVN:} %3): A data evaluation application for biophysics},\n" \
    "  howpublished={[web page] \\verb+%4+},\n" \
    "  year={2010--%6},\n" \
    "  note={[Accessed on %5]},\n" \
    "}\n\n" \
    "@misc{quickfit3,\n" \
    "  author={Jan Wolfgang Krieger and J{\\\"o}rg Langowski},\n" \
    "  title={{QuickFit %1} (compiled: %2, {SVN:} %3): A data evaluation application for biophysics},\n" \
    "  url={%4},\n" \
    "  year={2010--%6},\n" \
    "  note={accessed on %5},\n" \
    "}").arg(QF_VERSION).arg(qfInfoCompileDate()).arg(qfInfoSVNVersion()).arg(QF_WEBLINK).arg(QDate::currentDate().toString("yyyy/MM/dd")).arg(QDate::currentDate().year())


#endif //VERSION_H
