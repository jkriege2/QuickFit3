/*
  Name: sdfffunctiontools.h
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

/** \file sdfffunctiontools.h
 * \ingroup hlseqgen_sdff_f_tm
 *
 * This file contains some macros that make programming functions for SDFF easier. They implement argument type checking, ...
 */

#ifndef SDFFFUNCTIONTOOLS_H
#define SDFFFUNCTIONTOOLS_H

#include <string>
#include "tools.h"
#include "sequencererror.h"
#include "sdfftools.h"
#include "sdffparser.h"

/** \brief throws a runtime error inside a SDFF external function
 * \ingroup hlseqgen_sdff_f_tm
 *
 * \param num error number constant from sequencererror.h
 * \param message the error message string (you should create it with format() from geterrormessage() ).
 * \param where a string that specifies the function that caused the error
 */
#define RUN_TIME_ERROR(num, message, where) { throw sequencer::exception(num, format("runtime-error at line %d and time %lf:\n  %s", parser->get_current_line(), parser->get_current_time(), message.c_str()), where, parser->get_current_line(), parser->get_current_time()); }

/** \brief throws a runtime error inside a SDFF external function
 * \ingroup hlseqgen_sdff_f_tm
 *
 * \param num error number constant from sequencererror.h. The error message is created from
 *            this parameter.
 * \param where a string that specifies the function that caused the error
 */
#define RUN_TIME_ERRORN(num, where) { throw sequencer::exception(num, format("runtime-error at line %d and time %lf:\n  %s", parser->get_current_line(), parser->get_current_time(), get_errormessage(num).c_str()), where, parser->get_current_line(), parser->get_current_time()); }



/** \brief checks whether the number of function arguments is between \a minparams and \a maxparams. This throws a runtime-error!
 *         \a fname is a string that specifies the function that throws the error.
 * \ingroup hlseqgen_sdff_f_tm
 */
#define SDFFFUNC_CHECK_ARGCOUNT_MINMAX_RT(minparams, maxparams, fname) \
      if ((paramcount)<(minparams) || (paramcount)>(maxparams)) {\
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISSMIMA_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISSMIMA_NUM), std::string(fname).c_str(), paramcount, (minparams), (maxparams)), std::string(fname)); \
      }

/** \brief checks whether the number of function arguments is at least \a minparams. This throws a runtime-error!
 *         \a fname is a string that specifies the function that throws the error.
 * \ingroup hlseqgen_sdff_f_tm
 */
#define SDFFFUNC_CHECK_ARGCOUNT_ATLEAST_RT(minparams, fname) \
      if ((paramcount)<(minparams)) {\
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), std::string(fname).c_str(), paramcount, (minparams)), std::string(fname));\
      }

/** \brief checks whether the number of function arguments is exactly \a params. This throws a runtime-error!
 *         \a fname is a string that specifies the function that throws the error.
 * \ingroup hlseqgen_sdff_f_tm
 */
#define SDFFFUNC_CHECK_ARGCOUNT_RT(params, fname) \
      if ((paramcount)!=(params)) {\
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), std::string(fname).c_str(), paramcount, (params)), std::string(fname));\
      }

/** \brief checks whether the number of function arguments is between \a minparams and \a maxparams. This adds a parse-time error and then calls <code>return false;</code>!
 *         \a fname is a string that specifies the function that throws the error.
 * \ingroup hlseqgen_sdff_f_tm
 */
#define SDFFFUNC_CHECK_ARGCOUNT_MINMAX(minparams, maxparams, fname) \
      if ((paramcount)<(minparams) || (paramcount)>(maxparams)) {\
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISSMIMA_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISSMIMA_NUM), std::string(fname).c_str(), paramcount, (minparams), (maxparams)), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISSMIMA_NUM), std::string(fname).c_str()), parser->get_current_filename(), parser->get_current_line()); \
        return false; \
      }

/** \brief checks whether the number of function arguments is at least \a minparams. This adds a parse-time error and then calls <code>return false;</code>!
 *         \a fname is a string that specifies the function that throws the error.
 * \ingroup hlseqgen_sdff_f_tm
 */
#define SDFFFUNC_CHECK_ARGCOUNT_ATLEAST(minparams, fname) \
      if ((paramcount)<(minparams)) {\
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), std::string(fname).c_str(), paramcount, (minparams)), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISSAL_NUM), std::string(fname).c_str()), parser->get_current_filename(), parser->get_current_line()); \
        return false; \
      }

/** \brief checks whether the number of function arguments is exactly \a params. This adds a parse-time error and then calls <code>return false;</code>!
 *         \a fname is a string that specifies the function that throws the error.
 * \ingroup hlseqgen_sdff_f_tm
 */
#define SDFFFUNC_CHECK_ARGCOUNT(params, fname) \
      if ((paramcount)!=(params)) {\
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), std::string(fname).c_str(), paramcount, (params)), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGCMISS_NUM), std::string(fname).c_str()), parser->get_current_filename(), parser->get_current_line()); \
        return false; \
      }

/** \brief checks whether the parameter \a param (0 .. N) is a string. This throws a runtime-error!
 * \ingroup hlseqgen_sdff_f_tm
 *
 * \param num The parameter to check (0-based)
 * \param ontrue If the parameter type is correct this macro executes \a ontrue. For \a ontrue, you can use two
 *        predefined variables: \c p contains the specified parameter and \c d contains its value (std::string).
 * \param fname is a string that specifies the function that throws the error.
 * \param description is a description of the checked parameter (if this is \c "" the function outputs in \c string, else it
 *        outputs <code>"string (<description>)"</code>
 */
#define SDFFFUNC_CHECK_STRINGARGTYPE_OTRT(param, ontrue, fname, description) \
      if (parameter[(param)].type!=sdffRTString) { \
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str(), sdff_resulttypetostr(parameter[(param)].type).c_str(), (param)+1, (std::string(description).size()<1)?"string":format("string (%s)", std::string(description).c_str()).c_str()), std::string(fname)); \
      } else { \
          sdff_result p=parameter[(param)]; \
          std::string d=p.stringData; \
          ontrue; \
      }

/** \brief checks whether the parameter \a param (0 .. N) is an integer. This throws a runtime-error!
 * \ingroup hlseqgen_sdff_f_tm
 *
 * \param num The parameter to check (0-based)
 * \param ontrue If the parameter type is correct this macro executes \a ontrue. For \a ontrue, you can use two
 *        predefined variables: \c p contains the specified parameter and \c d contains its value (long long).
 * \param fname is a string that specifies the function that throws the error.
 * \param description is a description of the checked parameter (if this is \c "" the function outputs in \c "integer", else it
 *        outputs <code>"integer (<description>)"</code>
 */
#define SDFFFUNC_CHECK_INTARGTYPE_OTRT(param, ontrue, fname, description) \
      if (parameter[(param)].type!=sdffRTInteger) { \
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str(), sdff_resulttypetostr(parameter[(param)].type).c_str(), (param)+1, (std::string(description).size()<1)?"integer":format("integer (%s)", std::string(description).c_str()).c_str()), std::string(fname)); \
      } else { \
          sdff_result p=parameter[(param)]; \
          long long d=p.intData; \
          ontrue; \
      }

/** \brief checks whether the parameter \a param (0 .. N) is a number (integer or double). This throws a runtime-error!
 * \ingroup hlseqgen_sdff_f_tm
 *
 * \param num The parameter to check (0-based)
 * \param ontrue If the parameter type is correct this macro executes \a ontrue. For \a ontrue, you can use two
 *        predefined variables: \c p contains the specified parameter and \c d contains its value (double).
 * \param fname is a string that specifies the function that throws the error.
 * \param description is a description of the checked parameter (if this is \c "" the function outputs in \c "number", else it
 *        outputs <code>"number (<description>)"</code>
 */
#define SDFFFUNC_CHECK_NUMARGTYPE_OTRT(param, ontrue, fname, description) \
      if ((parameter[(param)].type!=sdffRTInteger) && (parameter[(param)].type!=sdffRTDouble)) { \
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str(), sdff_resulttypetostr(parameter[(param)].type).c_str(), (param)+1, (std::string(description).size()<1)?"number":format("number (%s)", std::string(description).c_str()).c_str()), std::string(fname)); \
      } else { \
          sdff_result p=parameter[(param)]; \
          double d=p.doubleData; \
          if (p.type==sdffRTInteger) d=p.intData; \
          ontrue; \
      }

/** \brief checks whether the parameter \a param (0 .. N) is a boolean. This throws a runtime-error!
 * \ingroup hlseqgen_sdff_f_tm
 *
 * \param num The parameter to check (0-based)
 * \param ontrue If the parameter type is correct this macro executes \a ontrue. For \a ontrue, you can use two
 *        predefined variables: \c p contains the specified parameter and \c d contains its value (bool).
 * \param fname is a string that specifies the function that throws the error.
 * \param description is a description of the checked parameter (if this is \c "" the function outputs in \c boolean, else it
 *        outputs <code>"boolean (<description>)"</code>
 */
#define SDFFFUNC_CHECK_BOOLARGTYPE_OTRT(param, ontrue, fname, description) \
      if (parameter[(param)].type!=sdffRTBoolean) { \
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str(), sdff_resulttypetostr(parameter[(param)].type).c_str(), (param)+1, (std::string(description).size()<1)?"boolean":format("boolean (%s)", std::string(description).c_str()).c_str()), std::string(fname)); \
      } else { \
          sdff_result p=parameter[(param)]; \
          bool d=p.boolData; \
          ontrue; \
      }


/** \brief checks whether the parameter \a param (0 .. N) is a string. This throws a runtime-error!
 * \ingroup hlseqgen_sdff_f_tm
 *
 * \param num The parameter to check (0-based)
 * \param fname is a string that specifies the function that throws the error.
 * \param description is a description of the checked parameter (if this is \c "" the function outputs in \c string, else it
 *        outputs <code>"string (<description>)"</code>
 */
#define SDFFFUNC_CHECK_STRINGARGTYPE_RT(param, fname, description) \
      if (parameter[(param)].type!=sdffRTString) { \
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str(), sdff_resulttypetostr(parameter[(param)].type).c_str(), (param)+1, (std::string(description).size()<1)?"string":format("string (%s)", std::string(description).c_str()).c_str()), std::string(fname)); \
      };

/** \brief checks whether the parameter \a param (0 .. N) is an integer. This throws a runtime-error!
 * \ingroup hlseqgen_sdff_f_tm
 *
 * \param num The parameter to check (0-based)
 * \param fname is a string that specifies the function that throws the error.
 * \param description is a description of the checked parameter (if this is \c "" the function outputs in \c "integer", else it
 *        outputs <code>"integer (<description>)"</code>
 */
#define SDFFFUNC_CHECK_INTARGTYPE_RT(param, fname, description) \
      if (parameter[(param)].type!=sdffRTInteger) { \
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str(), sdff_resulttypetostr(parameter[(param)].type).c_str(), (param)+1, (std::string(description).size()<1)?"integer":format("integer (%s)", std::string(description).c_str()).c_str()), std::string(fname)); \
      };

/** \brief checks whether the parameter \a param (0 .. N) is a number (integer or double). This throws a runtime-error!
 * \ingroup hlseqgen_sdff_f_tm
 *
 * \param num The parameter to check (0-based)
 * \param fname is a string that specifies the function that throws the error.
 * \param description is a description of the checked parameter (if this is \c "" the function outputs in \c "number", else it
 *        outputs <code>"number (<description>)"</code>
 */
#define SDFFFUNC_CHECK_NUMARGTYPE_RT(param, fname, description) \
      if ((parameter[(param)].type!=sdffRTInteger) && (parameter[(param)].type!=sdffRTDouble)) { \
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str(), sdff_resulttypetostr(parameter[(param)].type).c_str(), (param)+1, (std::string(description).size()<1)?"number":format("number (%s)", std::string(description).c_str()).c_str()), std::string(fname)); \
      };

/** \brief checks whether the parameter \a param (0 .. N) is a boolean. This throws a runtime-error!
 * \ingroup hlseqgen_sdff_f_tm
 *
 * \param num The parameter to check (0-based)
 * \param fname is a string that specifies the function that throws the error.
 * \param description is a description of the checked parameter (if this is \c "" the function outputs in \c boolean, else it
 *        outputs <code>"boolean (<description>)"</code>
 */
#define SDFFFUNC_CHECK_BOOLARGTYPE_RT(param, fname, description) \
      if (parameter[(param)].type!=sdffRTBoolean) { \
        RUN_TIME_ERROR(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str(), sdff_resulttypetostr(parameter[(param)].type).c_str(), (param)+1, (std::string(description).size()<1)?"boolean":format("boolean (%s)", std::string(description).c_str()).c_str()), std::string(fname)); \
      };






/** \brief checks whether the parameter \a param (0 .. N) is a string. This adds a parse-time error and then calls <code>return false;</code>!
 * \ingroup hlseqgen_sdff_f_tm
 *
 * \param num The parameter to check (0-based)
 * \param fname is a string that specifies the function that throws the error.
 * \param description is a description of the checked parameter (if this is \c "" the function outputs in \c string, else it
 *        outputs <code>"string (<description>)"</code>
 */
#define SDFFFUNC_CHECK_STRINGARGTYPE(param, fname, description) \
      if ((parameter[(param)]!=sdffPTString)&&(parameter[(param)]!=sdffPTUnknown)) { \
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str(), sdff_paramtypetostr(parameter[(param)]).c_str(), (param)+1, (std::string(description).size()<1)?"string":format("string (%s)", std::string(description).c_str()).c_str()), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str()), parser->get_current_filename(), parser->get_current_line()); \
        return false; \
      };

/** \brief checks whether the parameter \a param (0 .. N) is an integer. This adds a parse-time error and then calls <code>return false;</code>!
 * \ingroup hlseqgen_sdff_f_tm
 *
 * \param num The parameter to check (0-based)
 * \param fname is a string that specifies the function that throws the error.
 * \param description is a description of the checked parameter (if this is \c "" the function outputs in \c "integer", else it
 *        outputs <code>"integer (<description>)"</code>
 */
#define SDFFFUNC_CHECK_INTARGTYPE(param, fname, description) \
      if ((parameter[(param)]!=sdffPTString)&&(parameter[(param)]!=sdffPTUnknown)) { \
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str(), sdff_paramtypetostr(parameter[(param)]).c_str(), (param)+1, (std::string(description).size()<1)?"integer":format("integer (%s)", std::string(description).c_str()).c_str()), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str()), parser->get_current_filename(), parser->get_current_line()); \
        return false; \
      };

/** \brief checks whether the parameter \a param (0 .. N) is a number (integer or double). This adds a parse-time error and then calls <code>return false;</code>!
 * \ingroup hlseqgen_sdff_f_tm
 *
 * \param num The parameter to check (0-based)
 * \param fname is a string that specifies the function that throws the error.
 * \param description is a description of the checked parameter (if this is \c "" the function outputs in \c "number", else it
 *        outputs <code>"number (<description>)"</code>
 */
#define SDFFFUNC_CHECK_NUMARGTYPE(param, fname, description) \
      if ((parameter[(param)]!=sdffPTInteger)&&(parameter[(param)]!=sdffPTDouble)&&(parameter[(param)]!=sdffPTUnknown)) { \
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str(), sdff_paramtypetostr(parameter[(param)]).c_str(), (param)+1, (std::string(description).size()<1)?"(double) number":format("(double) number (%s)", std::string(description).c_str()).c_str()), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str()), parser->get_current_filename(), parser->get_current_line()); \
        return false; \
      };

/** \brief checks whether the parameter \a param (0 .. N) is a number (integer or double) or a string. This adds a parse-time error and then calls <code>return false;</code>!
 * \ingroup hlseqgen_sdff_f_tm
 *
 * \param num The parameter to check (0-based)
 * \param fname is a string that specifies the function that throws the error.
 * \param description is a description of the checked parameter (if this is \c "" the function outputs in \c "number", else it
 *        outputs <code>"number (<description>)"</code>
 */
#define SDFFFUNC_CHECK_NUMORSTRINGARGTYPE(param, fname, description) \
      if ((parameter[(param)]!=sdffPTInteger)&&(parameter[(param)]!=sdffPTDouble)&&(parameter[(param)]!=sdffPTString)&&(parameter[(param)]!=sdffPTUnknown)) { \
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str(), sdff_paramtypetostr(parameter[(param)]).c_str(), (param)+1, (std::string(description).size()<1)?"number or string":format("number or string (%s)", std::string(description).c_str()).c_str()), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str()), parser->get_current_filename(), parser->get_current_line()); \
        return false; \
      };

/** \brief checks whether the parameter \a param (0 .. N) is an integer or a string. This adds a parse-time error and then calls <code>return false;</code>!
 * \ingroup hlseqgen_sdff_f_tm
 *
 * \param num The parameter to check (0-based)
 * \param fname is a string that specifies the function that throws the error.
 * \param description is a description of the checked parameter (if this is \c "" the function outputs in \c "number", else it
 *        outputs <code>"number (<description>)"</code>
 */
#define SDFFFUNC_CHECK_INTORSTRINGARGTYPE(param, fname, description) \
      if ((parameter[(param)]!=sdffPTInteger)&&(parameter[(param)]!=sdffPTString)&&(parameter[(param)]!=sdffPTUnknown)) { \
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str(), sdff_paramtypetostr(parameter[(param)]).c_str(), (param)+1, (std::string(description).size()<1)?"integer or string":format("integer or string (%s)", std::string(description).c_str()).c_str()), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str()), parser->get_current_filename(), parser->get_current_line()); \
        return false; \
      };

/** \brief checks whether the parameter \a param (0 .. N) is a boolean. This adds a parse-time error and then calls <code>return false;</code>!
 * \ingroup hlseqgen_sdff_f_tm
 *
 * \param num The parameter to check (0-based)
 * \param fname is a string that specifies the function that throws the error.
 * \param description is a description of the checked parameter (if this is \c "" the function outputs in \c boolean, else it
 *        outputs <code>"boolean (<description>)"</code>
 */
#define SDFFFUNC_CHECK_BOOLARGTYPE(param, fname, description) \
      if ((parameter[(param)]!=sdffPTBoolean)&&(parameter[(param)]!=sdffPTUnknown)) { \
        parser->add_error_p(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM, format(get_errormessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str(), sdff_paramtypetostr(parameter[(param)]).c_str(), (param)+1, (std::string(description).size()<1)?"boolean":format("boolean (%s)", std::string(description).c_str()).c_str()), format(get_errorsmessage(SEQUENCER_SDFFERROR_FUNCARGTMISS_NUM), std::string(fname).c_str()), parser->get_current_filename(), parser->get_current_line()); \
        return false; \
      };

#endif // SDFFFUNCTIONTOOLS_H


/*@}*/

