#ifndef QFPREPROCESSORMAGIC_H
#define QFPREPROCESSORMAGIC_H

#include "qfpreprocessormagic_private.h"


#define QFPPM_DEC(x) QFPPM_DEC_I(x)
#define QFPPM_DEC_I(x) QFPPM_DEC_ ## x

#define QFPPM_INC(x) QFPPM_INC_I(x)
#define QFPPM_INC_I(x) QFPPM_INC_ ## x

#define QFPPM_COUNT_START 0







#endif // QFPREPROCESSORMAGIC_H
