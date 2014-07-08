#ifndef VERSION_H
#define VERSION_H

#include <QtGlobal>
#include "compiledate.h"
#include "svnversion.h"
#include "compiler.h"

#define VERSION_FULL "3.0"
#define QF_VERSION VERSION_FULL

#define VERSION_STATUS "beta"

#define QF_THANKS_TO "Jan Buchholz, Tabea Elbel, Dr. Nicolas Dross, Dr. György Vámosi, Dr. Vera Böhm, Prof. Jörg Langowski, Dr. Katalin Tòth"

#define QF_COPYRIGHT QString("&copy; 2010-%1 by Jan W. Krieger").arg(QDate::currentDate().year())

#define QF_AUTHOR "Jan W. Krieger"

#define QF_EMAIL "j.krieger@dkfz.de"

#define QF_MAILLIST "quickfit@dkfz.de"

#define QF_MAILLIST_REQUEST "mailto:quickfit-request@dkfz.de?subject=subscribe&amp;body=subscribe"

#define QF_WEBLINK "http://www.dkfz.de/Macromol/quickfit/"

#define QF_UPDATEXMLURL "http://www.dkfz.de/Macromol/quickfit/update.xml"
#define QF_UPDATESPECIALXMLURL "http://www.dkfz.de/Macromol/quickfit/update.xml"
//#define QF_UPDATESPECIALXMLURL "http://www.dkfz.de/Macromol/quickfit/updatespecial.xml"

#define QF_LICENSE QT_TR_NOOP("<b>This is POSTCARD-ware, so if you like the program and use it, please let us know by sending a nice postcard from your place on the world to:</b><br>DKFZ (German Cancer Research Center)<br>Dept. B040 (Biophysics of Macromolecules)<br>z.H. Jan Krieger<br>Im Neuenheimer Feld 580<br>D-69120 Heidelberg<br>GERMANY")

#define QF_CITATION_HTML QString("Jan Wolfgang Krieger, J&ouml;rg Langowski (2010-%6): <b>QuickFit %1 (compiled: %2, SVN: %3): A data evaluation application for biophysics</b>, <i>[web page] <a href=\"%4\">%4</a> [Accessed on %5]</i>").arg(QF_VERSION).arg(COMPILEDATE).arg(SVNVERSION).arg(QF_WEBLINK).arg(QDate::currentDate().toString("yyyy/MM/d")).arg(QDate::currentDate().year())
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
    "}").arg(QF_VERSION).arg(COMPILEDATE).arg(SVNVERSION).arg(QF_WEBLINK).arg(QDate::currentDate().toString("yyyy/MM/dd")).arg(QDate::currentDate().year())


#endif //VERSION_H
