

#include <QtCore/QtGlobal>

#if defined(QFLIB_LIBRARY)
#  define QFLIB_EXPORT Q_DECL_EXPORT
#else
#  define QFLIB_EXPORT Q_DECL_IMPORT
#endif


