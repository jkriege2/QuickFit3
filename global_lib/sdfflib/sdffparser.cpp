/*
  Name: sdffparser.cpp
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

#include "sdffparser.h" // class's header file


namespace sequencer {

std::string sdff_tokentostr(sdff_token t){
  switch(t) {
    case sdffTEND: return "END";               /*!< \brief end token */
    case sdffTPRINT: return "PRINT";             /*!< \brief a semicolon ';' */
    case sdffTKOMMA: return "KOMMA";             /*!< \brief a comma ',' between two function parameters */
    case sdffTSTRING: return "STRING";            /*!< \brief a string delimiter ' or " has been found ... the string is read in*/
    case sdffTNAME: return "NAME";              /*!< \brief a name (consisting of characters) of a variable or function */
    case sdffTDOUBLE: return "DOUBLE";            /*!< \brief a number in scientific notation */
    case sdffTINTEGER: return "INTEGER";           /*!< \brief an integer number */
    case sdffTPLUS: return "PLUS";              /*!< \brief a plus operator '+' */
    case sdffTMINUS: return "MINUS";             /*!< \brief a minus operator '-' */
    case sdffTMUL: return "MUL";               /*!< \brief a multiplication operator '*' */
    case sdffTDIV: return "DIV";               /*!< \brief a division operator '/' */
    case sdffTMODULO: return "MODULO";            /*!< \brief a modulo operator '%' or 'mod' */
    case sdffTASSIGN: return "ASSIGN";            /*!< \brief a variable assignment = */
    case sdffTLPARANTHESIS: return "LPARANTHESIS";      /*!< \brief left brackets '(' */
    case sdffTRPARANTHESIS: return "RPARANTHESIS";      /*!< \brief right brackets ')' */
    case sdffTLBRACKET: return "LBRACKET";          /*!< \brief left brackets '[' */
    case sdffTRBRACKET: return "RBRACKET";          /*!< \brief right brackets ']' */
    case sdffTLBRACE: return "LBRACE";            /*!< \brief left brackets '{' */
    case sdffTRBRACE: return "RBRACE";            /*!< \brief right brackets '}' */
    case sdffTPOWER: return "POWER";             /*!< \brief a power operator '^' */
    case sdffTINVERT: return "INVERT";            /*!< \brief a invert token '~' */
    case sdffTLOGIC_NOT: return "LOGIC_NOT";         /*!< \brief a logical NOT '!' / 'not' */
    case sdffTLOGIC_AND: return "LOGIC_AND";         /*!< \brief a logical AND operator '&' / 'and' */
    case sdffTLOGIC_OR: return "LOGIC_OR";          /*!< \brief a logical OR operator '|' / 'or' */
    case sdffTLOGIC_XOR: return "LOGIC_XOR";         /*!< \brief a logical XOR operator 'xor' */
    case sdffTLOGIC_SHL: return "LOGIC_SHL";
    case sdffTLOGIC_SHR: return "LOGIC_SHR";
    case sdffTLOGIC_NOR: return "LOGIC_NOR";         /*!< \brief a logical NOR operator 'nor' */
    case sdffTLOGIC_NAND: return "LOGIC_NAND";        /*!< \brief a logical NAND operator 'nand' */
    case sdffTLOGIC_TRUE: return "LOGIC_TRUE";        /*!< \brief 'true' */
    case sdffTLOGIC_FALSE: return "LOGIC_FALSE";       /*!< \brief  'false' */
    case sdffTCOMP_EQUAL: return "COMP_EQUAL";        /*!< \brief equals operation '==' */
    case sdffTCOMP_UNEQUAL: return "COMP_UNEQUAL";      /*!< \brief unequal operation '!=' */
    case sdffTCOMP_GREATER: return "COMP_GREATER";      /*!< \brief greater than operation '>' */
    case sdffTCOMP_SMALLER: return "COMP_SMALLER";      /*!< \brief smaller than operation '<' */
    case sdffTCOMP_GEQUAL: return "COMP_GEQUAL";       /*!< \brief greater than or equal operation '>=' */
    case sdffTCOMP_SEQUAL: return "COMP_SEQUAL";       /*!< \brief smaller than or equal operation '<=' */
    case sdffTTDOUBLE: return "TDOUBLE";           /*!< \brief a type specifier 'double' */
    case sdffTTINTEGER: return "TINTEGER";          /*!< \brief a type specifier 'integer' */
    case sdffTTSTRING: return "TSTRING";           /*!< \brief a type specifier 'string' */
    case sdffTTBOOLEAN: return "TBOOLEAN";          /*!< \brief a type specifier 'boolean' */
    case sdffTCONST: return "CONST";             /*!< \brief keyword 'const' */
    case sdffTIF: return "IF";                /*!< \brief keyword 'if' */
    case sdffTELSEIF: return "ELSEIF";                /*!< \brief keyword 'elseif' */
    case sdffTDO: return "DO";                /*!< \brief keyword 'DO' */
    case sdffTREPEAT: return "REPEAT";            /*!< \brief keyword 'repeat' */
    case sdffTWHILE: return "WHILE";             /*!< \brief keyword 'while' */
    case sdffTUNTIL: return "UNTIL";             /*!< \brief keyword 'until' */
    case sdffTELSE: return "ELSE";
    case sdffTPARAMETERS: return "PARAMETERS";        /*!< \brief keyword 'parameters' */
    case sdffTSEQUENCE: return "SEQUENCE";          /*!< \brief keyword 'sequence' */
    case sdffTEVENT: return "EVENT";             /*!< \brief keyword 'event' */
    case sdffTPREEVENT: return "PRE_EVENT";             /*!< \brief keyword 'pre_event' */
    case sdffTLASTEVENT: return "LAST_EVENT";             /*!< \brief keyword 'last_event' */
    case sdffTCHANNEL: return "CHANNEL";             /*!< \brief keyword 'channel' */
    case sdffTCHANNELNAMES: return "CHANNEL_NAMES";             /*!< \brief keyword 'channel_names' */
    case sdffTRELATIVEEVENT: return "RELATIVEEVENT";     /*!< \brief keyword 'relative_event' */
    case sdffTSAFESETTINGS: return "SAFESETTINGS";        /*!< \brief keyword 'safe_settings' */
  }
  return "";
}


std::string asdff_tokentostr(sdff_token t){
  switch(t) {
    case sdffTEND: return "end of input";               /*!< \brief end token */
    case sdffTPRINT: return "print character ';'";             /*!< \brief a semicolon ';' */
    case sdffTKOMMA: return "comma ','";             /*!< \brief a comma ',' between two function parameters */
    case sdffTSTRING: return "string constant";            /*!< \brief a string delimiter ' or " has been found ... the string is read in*/
    case sdffTNAME: return "name";              /*!< \brief a name (consisting of characters) of a variable or function */
    case sdffTDOUBLE: return "double constant";            /*!< \brief a number in scientific notation */
    case sdffTINTEGER: return "integer constant";           /*!< \brief an integer number */
    case sdffTPLUS: return "plus '+'";              /*!< \brief a plus operator '+' */
    case sdffTMINUS: return "minus '-'";             /*!< \brief a minus operator '-' */
    case sdffTMUL: return "multiplication '*'";               /*!< \brief a multiplication operator '*' */
    case sdffTDIV: return "division '/'";               /*!< \brief a division operator '/' */
    case sdffTMODULO: return "modulo '%'/'mod'";            /*!< \brief a modulo operator '%' or 'mod' */
    case sdffTASSIGN: return "assignment '='";            /*!< \brief a variable assignment = */
    case sdffTLPARANTHESIS: return "paranthesis '('";      /*!< \brief left brackets '(' */
    case sdffTRPARANTHESIS: return "paranthesis ')'";      /*!< \brief right brackets ')' */
    case sdffTLBRACKET: return "bracket '['";          /*!< \brief left brackets '[' */
    case sdffTRBRACKET: return "bracket ']'";          /*!< \brief right brackets ']' */
    case sdffTLBRACE: return "brace '{'";            /*!< \brief left brackets '{' */
    case sdffTRBRACE: return "brace '{'";            /*!< \brief right brackets '}' */
    case sdffTPOWER: return "power '^'";             /*!< \brief a power operator '^' */
    case sdffTINVERT: return "invert '~'";            /*!< \brief a invert token '~' */
    case sdffTLOGIC_NOT: return "logical not";         /*!< \brief a logical NOT '!' / 'not' */
    case sdffTLOGIC_AND: return "logical and";         /*!< \brief a logical AND operator '&' / 'and' */
    case sdffTLOGIC_OR: return "logical or";          /*!< \brief a logical OR operator '|' / 'or' */
    case sdffTLOGIC_XOR: return "logical xor";         /*!< \brief a logical XOR operator 'xor' */
    case sdffTLOGIC_NOR: return "logical nor";         /*!< \brief a logical NOR operator 'nor' */
    case sdffTLOGIC_NAND: return "logical nand";        /*!< \brief a logical NAND operator 'nand' */
    case sdffTLOGIC_TRUE: return "true constant";        /*!< \brief 'true' */
    case sdffTLOGIC_FALSE: return "false constant";       /*!< \brief  'false' */
    case sdffTLOGIC_SHL: return "shift left 'shl'";
    case sdffTLOGIC_SHR: return "shift right 'shr'";
    case sdffTCOMP_EQUAL: return "equals '=='";        /*!< \brief equals operation '==' */
    case sdffTCOMP_UNEQUAL: return "unequal '!='";      /*!< \brief unequal operation '!=' */
    case sdffTCOMP_GREATER: return "greater than '>'";      /*!< \brief greater than operation '>' */
    case sdffTCOMP_SMALLER: return "smaller than '<'";      /*!< \brief smaller than operation '<' */
    case sdffTCOMP_GEQUAL: return "greater or equal '>='";       /*!< \brief greater than or equal operation '>=' */
    case sdffTCOMP_SEQUAL: return "smaller or equal '<='";       /*!< \brief smaller than or equal operation '<=' */
    case sdffTTDOUBLE: return "double type declarator";           /*!< \brief a type specifier 'double' */
    case sdffTTINTEGER: return "integer type declarator";          /*!< \brief a type specifier 'integer' */
    case sdffTTSTRING: return "string type declarator";           /*!< \brief a type specifier 'string' */
    case sdffTTBOOLEAN: return "boolean type declarator";          /*!< \brief a type specifier 'boolean' */
    case sdffTCONST: return "const declarator";             /*!< \brief keyword 'const' */
    case sdffTIF: return "keyword if";                /*!< \brief keyword 'if' */
    case sdffTELSEIF: return "keyword elseif";                /*!< \brief keyword 'elseif' */
    case sdffTELSE: return "keyword else";
    case sdffTDO: return "keyword do";                /*!< \brief keyword 'DO' */
    case sdffTREPEAT: return "keyword repeat";            /*!< \brief keyword 'repeat' */
    case sdffTWHILE: return "keyword while";             /*!< \brief keyword 'while' */
    case sdffTUNTIL: return "keyword until";             /*!< \brief keyword 'until' */
    case sdffTPARAMETERS: return "group parameters{...}";        /*!< \brief keyword 'parameters' */
    case sdffTSEQUENCE: return "group sequence{...}";          /*!< \brief keyword 'sequence' */
    case sdffTEVENT: return "group event(..){...}";             /*!< \brief keyword 'event' */
    case sdffTLASTEVENT: return "group last_event{...}";             /*!< \brief keyword 'last_event' */
    case sdffTPREEVENT: return "group pre_event{...}";             /*!< \brief keyword 'pre_event' */
    case sdffTCHANNEL: return "group channel(..){...}";             /*!< \brief keyword 'channel' */
    case sdffTCHANNELNAMES: return "group channel_names{...}";             /*!< \brief keyword 'channel_names' */
    case sdffTRELATIVEEVENT: return "group relative_event(..){...}";     /*!< \brief keyword 'relative_event' */
    case sdffTSAFESETTINGS: return "group safe_settings{...}";        /*!< \brief keyword 'safe_settings' */
  }
  return "";
}


SDFFparser::SDFFparser(sequenceGenerator* sg, channelManager* cm):
    baseImporter(sg, cm)
{
    debug=false;
  mersenne = new MTRand();
  position_mode=biBoth;
  vmanager=new SDFFVariableManager;
  fmanager=new SDFFFunctionManager(this, vmanager);
  add_standard_functions();
  add_standard_constants();
  current_filename="";
  current_time=-1;
  current_line=-1;
  current_channel="";
  parameters=new SDFFlistNode(this);
  sequence=new SDFFlistNode(this);
  safe_settings=new SDFFlistNode(this);
  last_event=NULL;
  pre_event=NULL;
  sdff=NULL;
  emanager=new SDFFelementManager(this, sg);
  cmd_manager=new SDFFHWcommandManager(this);
  inside_channel=false;
  inside_event=false;
  inside_preevent=false;
  inside_lastevent=false;
  log=NULL;
}

SDFFparser::~SDFFparser() {
  delete sequence;
  delete parameters;
  delete safe_settings;
  delete fmanager;
  delete vmanager;
  delete emanager;
  delete mersenne;
}

sdff_channel_alias_data SDFFparser::get_alias_info(size_t n) {
    if (n>=channel_aliases.size()) SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, "SDFFparser::get_alias_info("+inttostr(n)+")");
    size_t indx=0;
    for (std::map<std::string, sdff_channel_alias_data>::iterator i=channel_aliases.begin(); i!=channel_aliases.end(); ++i) {
        if (indx==n) return i->second;
        indx++;
    }
    SEQUENCER_ERRORN(SEQUENCER_ERROR_RANGE_NUM, "SDFFparser::get_alias_info('"+inttostr(n)+"')");
};

sdff_channel_alias_data SDFFparser::get_alias_info_for_channel(std::string channel) {
    for (std::map<std::string, sdff_channel_alias_data>::iterator i=channel_aliases.begin(); i!=channel_aliases.end(); ++i) {
        if (i->second.channel==channel) return i->second;
    }
    SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_NOALIASFORCHANNEL_NUM, "SDFFparser::get_alias_info_for_channel('"+channel+"')", channel.c_str());
}
void SDFFparser::add_standard_functions(){
  fmanager->add_ext("min", sdff_functions::MIN, sdff_functions::MIN_check);
  fmanager->add_ext("max", sdff_functions::MAX, sdff_functions::MAX_check);
  fmanager->add_ext("limit", sdff_functions::LIMIT, sdff_functions::LIMIT_check);
  fmanager->add_ext("delta", sdff_functions::DELTA, sdff_functions::DELTA_check);
  fmanager->add_ext("theta", sdff_functions::THETA, sdff_functions::THETA_check);
  fmanager->add_ext("sign", sdff_functions::SIGN, sdff_functions::SIGN_check);
  fmanager->add_ext("sqrt", sdff_functions::SQRT, sdff_functions::SQRT_check);
  fmanager->add_ext("cbrt", sdff_functions::CBRT, sdff_functions::CBRT_check);
  fmanager->add_ext("sin", sdff_functions::SIN, sdff_functions::SIN_check);
  fmanager->add_ext("cos", sdff_functions::COS, sdff_functions::COS_check);
  fmanager->add_ext("tan", sdff_functions::TAN, sdff_functions::TAN_check);
  fmanager->add_ext("exp", sdff_functions::EXP, sdff_functions::EXP_check);
  fmanager->add_ext("expm1", sdff_functions::EXPM1, sdff_functions::EXPM1_check);
  fmanager->add_ext("logp1", sdff_functions::LOGP1, sdff_functions::LOGP1_check);
  fmanager->add_ext("log", sdff_functions::LOG, sdff_functions::LOG_check);
  fmanager->add_ext("ln", sdff_functions::LOG, sdff_functions::LOG_check);
  fmanager->add_ext("log2", sdff_functions::LOG2, sdff_functions::LOG2_check);
  fmanager->add_ext("lb", sdff_functions::LOG2, sdff_functions::LOG2_check);
  fmanager->add_ext("log10", sdff_functions::LOG10, sdff_functions::LOG10_check);
  fmanager->add_ext("ld", sdff_functions::LOG10, sdff_functions::LOG10_check);
  fmanager->add_next("IF", sdff_functions::IF, sdff_functions::IF_check);
  fmanager->add_next("sum", sdff_functions::SUM, sdff_functions::SUM_check);
  fmanager->add_next("prod", sdff_functions::PROD, sdff_functions::PROD_check);
  fmanager->add_ext("asin", sdff_functions::ASIN, sdff_functions::ASIN_check);
  fmanager->add_ext("acos", sdff_functions::ACOS, sdff_functions::ACOS_check);
  fmanager->add_ext("atan", sdff_functions::ATAN, sdff_functions::ATAN_check);
  fmanager->add_ext("asinh", sdff_functions::ASINH, sdff_functions::ASINH_check);
  fmanager->add_ext("acosh", sdff_functions::ACOSH, sdff_functions::ACOSH_check);
  fmanager->add_ext("atanh", sdff_functions::ATANH, sdff_functions::ATANH_check);
  fmanager->add_ext("sinh", sdff_functions::SINH, sdff_functions::SINH_check);
  fmanager->add_ext("cosh", sdff_functions::COSH, sdff_functions::COSH_check);
  fmanager->add_ext("tanh", sdff_functions::TANH, sdff_functions::TANH_check);
  fmanager->add_ext("erf", sdff_functions::ERF, sdff_functions::ERF_check);
  fmanager->add_ext("erfc", sdff_functions::ERFC, sdff_functions::ERFC_check);
  fmanager->add_ext("gamma", sdff_functions::TGAMMA, sdff_functions::TGAMMA_check);
  fmanager->add_ext("j0", sdff_functions::J0, sdff_functions::J0_check);
  fmanager->add_ext("j1", sdff_functions::J1, sdff_functions::J1_check);
  fmanager->add_ext("jn", sdff_functions::JN, sdff_functions::JN_check);
  fmanager->add_ext("y0", sdff_functions::Y0, sdff_functions::Y0_check);
  fmanager->add_ext("y1", sdff_functions::Y1, sdff_functions::Y1_check);
  fmanager->add_ext("yn", sdff_functions::YN, sdff_functions::YN_check);
  fmanager->add_ext("abs", sdff_functions::ABS, sdff_functions::ABS_check);
  fmanager->add_ext("round", sdff_functions::ROUND, sdff_functions::ROUND_check);
  fmanager->add_ext("ceil", sdff_functions::CEIL, sdff_functions::CEIL_check);
  fmanager->add_ext("floor", sdff_functions::FLOOR, sdff_functions::FLOOR_check);
  fmanager->add_ext("trunc", sdff_functions::TRUNC, sdff_functions::TRUNC_check);
  fmanager->add_ext("tostring", sdff_functions::TOSTRING, sdff_functions::TOSTRING_check);
  fmanager->add_ext("tolower", sdff_functions::TOLOWER, sdff_functions::TOLOWER_check);
  fmanager->add_ext("toupper", sdff_functions::TOUPPER, sdff_functions::TOUPPER_check);
  fmanager->add_ext("strip", sdff_functions::STRIP, sdff_functions::STRIP_check);
  fmanager->add_ext("strget", sdff_functions::STRGET, sdff_functions::STRGET_check);
  fmanager->add_ext("str2num", sdff_functions::STR2NUM, sdff_functions::STR2NUM_check);
  fmanager->add_ext("str2bool", sdff_functions::STR2BOOL, sdff_functions::STR2BOOL_check);
  fmanager->add_ext("length", sdff_functions::LENGTH, sdff_functions::LENGTH_check);
  fmanager->add_ext("factorial", sdff_functions::FACTORIAL, sdff_functions::FACTORIAL_check);
  fmanager->add_ext("set", sdff_functions::SET, sdff_functions::SET_check);
  fmanager->add_ext("ramp_lin", sdff_functions::RAMPLIN, sdff_functions::RAMPLIN_check);
  fmanager->add_ext("ramp_cos", sdff_functions::RAMPCOS, sdff_functions::RAMPCOS_check);
  fmanager->add_ext("ramp_exp", sdff_functions::RAMPEXP, sdff_functions::RAMPEXP_check);
  fmanager->add_ext("ramp_sig", sdff_functions::RAMPSIG, sdff_functions::RAMPSIG_check);
  fmanager->add_ext("pulse", sdff_functions::PULSE, sdff_functions::PULSE_check);
  fmanager->add_ext("write", sdff_functions::WRITE, sdff_functions::WRITE_check);
  fmanager->add_ext("writeln", sdff_functions::WRITELN, sdff_functions::WRITELN_check);
  fmanager->add_ext("add_error_marker", sdff_functions::ADDERRORMARKER, sdff_functions::ADDERRORMARKER_check);
  fmanager->add_ext("add_warning_marker", sdff_functions::ADDWARNINGMARKER, sdff_functions::ADDWARNINGMARKER_check);
  fmanager->add_ext("add_info_marker", sdff_functions::ADDINFOMARKER, sdff_functions::ADDINFOMARKER_check);
  fmanager->add_ext("random", sdff_functions::RAND, sdff_functions::RAND_check);
  fmanager->add_ext("random_int", sdff_functions::RANDINT, sdff_functions::RANDINT_check);
  fmanager->add_ext("random_norm", sdff_functions::RANDGAUSS, sdff_functions::RANDGAUSS_check);
  fmanager->add_ext("random_seed", sdff_functions::RANDSEED, sdff_functions::RANDSEED_check);
  fmanager->add_ext("tohex", sdff_functions::TOHEX, sdff_functions::TOHEX_check);
  fmanager->add_ext("tobin", sdff_functions::TOBIN, sdff_functions::TOBIN_check);
  fmanager->add_ext("char", sdff_functions::CHAR, sdff_functions::CHAR_check);
  fmanager->add_ext("isstring", sdff_functions::ISOFTYPE, sdff_functions::ISOFTYPE_check);
  fmanager->add_ext("isinteger", sdff_functions::ISOFTYPE, sdff_functions::ISOFTYPE_check);
  fmanager->add_ext("isint", sdff_functions::ISOFTYPE, sdff_functions::ISOFTYPE_check);
  fmanager->add_ext("isdouble", sdff_functions::ISOFTYPE, sdff_functions::ISOFTYPE_check);
  fmanager->add_ext("isboolean", sdff_functions::ISOFTYPE, sdff_functions::ISOFTYPE_check);
  fmanager->add_ext("isbool", sdff_functions::ISOFTYPE, sdff_functions::ISOFTYPE_check);
  fmanager->add_ext("isvoid", sdff_functions::ISOFTYPE, sdff_functions::ISOFTYPE_check);
  fmanager->add_ext("fopen", sdff_functions::FOPEN, sdff_functions::FOPEN_check);
  fmanager->add_ext("fclose", sdff_functions::FCLOSE, sdff_functions::FCLOSE_check);
  fmanager->add_ext("fwrite", sdff_functions::FWRITE, sdff_functions::FWRITE_check);
  fmanager->add_ext("fwriteln", sdff_functions::FWRITELN, sdff_functions::FWRITELN_check);
  fmanager->add_ext("serial_open", sdff_functions::SEROPEN, sdff_functions::SEROPEN_check);
  fmanager->add_ext("serial_read", sdff_functions::SERREAD, sdff_functions::SERREAD_check);
  fmanager->add_ext("serial_write", sdff_functions::SERWRITE, sdff_functions::SERWRITE_check);
  fmanager->add_ext("serial_writeln", sdff_functions::SERWRITE, sdff_functions::SERWRITE_check);
  fmanager->add_ext("serial_close", sdff_functions::SERCLOSE, sdff_functions::SERCLOSE_check);
  fmanager->add_ext("sergpib_send", sdff_functions::SERGPIBSEND, sdff_functions::SERGPIBSEND_check);
  fmanager->add_ext("sergpib_send_DCL", sdff_functions::SERGPIBSENDCMD, sdff_functions::SERGPIBSENDCMD_check);
  fmanager->add_ext("sergpib_send_SDC", sdff_functions::SERGPIBSENDCMD, sdff_functions::SERGPIBSENDCMD_check);
  fmanager->add_ext("sergpib_send_GET", sdff_functions::SERGPIBSENDCMD, sdff_functions::SERGPIBSENDCMD_check);
  fmanager->add_ext("sergpib_send_GTL", sdff_functions::SERGPIBSENDCMD, sdff_functions::SERGPIBSENDCMD_check);
  fmanager->add_ext("sergpib_send_LLO", sdff_functions::SERGPIBSENDCMD, sdff_functions::SERGPIBSENDCMD_check);
  fmanager->add_ext("sergpib_send_PPC", sdff_functions::SERGPIBSENDCMD, sdff_functions::SERGPIBSENDCMD_check);
  fmanager->add_ext("sergpib_send_PPU", sdff_functions::SERGPIBSENDCMD, sdff_functions::SERGPIBSENDCMD_check);
  fmanager->add_ext("sergpib_send_UNT", sdff_functions::SERGPIBSENDCMD, sdff_functions::SERGPIBSENDCMD_check);
  fmanager->add_ext("sergpib_send_UNL", sdff_functions::SERGPIBSENDCMD, sdff_functions::SERGPIBSENDCMD_check);
  fmanager->add_ext("sergpib_send_SPD", sdff_functions::SERGPIBSENDCMD, sdff_functions::SERGPIBSENDCMD_check);
  fmanager->add_ext("sergpib_send_SPE", sdff_functions::SERGPIBSENDCMD, sdff_functions::SERGPIBSENDCMD_check);
  fmanager->add_ext("sergpib_send_TCT", sdff_functions::SERGPIBSENDCMD, sdff_functions::SERGPIBSENDCMD_check);
  fmanager->add_ext("sergpib_send_IFC", sdff_functions::SERGPIBSENDCMD, sdff_functions::SERGPIBSENDCMD_check);
  fmanager->add_ext("sergpib_query", sdff_functions::SERGPIBQUERY, sdff_functions::SERGPIBQUERY_check);
  fmanager->add_ext("sergpib_read", sdff_functions::SERGPIBREAD, sdff_functions::SERGPIBREAD_check);
  fmanager->add_ext("sergpib_config", sdff_functions::SERGPIBCONFIG, sdff_functions::SERGPIBCONFIG_check);
  fmanager->add_ext("sergpib_configint", sdff_functions::SERGPIBCONFIGINT, sdff_functions::SERGPIBCONFIGINT_check);
  fmanager->add_ext("sergpib_enable_interrupt", sdff_functions::SERGPIBENABLEINT, sdff_functions::SERGPIBENABLEINT_check);
  fmanager->add_ext("sergpib_disable_interrupt", sdff_functions::SERGPIBENABLEINT, sdff_functions::SERGPIBENABLEINT_check);
}


void SDFFparser::add_standard_constants(){
  vmanager->set_write_global(true);
  vmanager->add_variable_double("pi", 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899, true);
  vmanager->add_variable_double("e", 2.7182818284590452353602874713526624977572470936999595749669676277240766303535, true);
  vmanager->add_variable_double("sqrt2", sqrt(2), true);
  vmanager->add_variable_double("log2e", M_LOG2E, true);
  vmanager->add_variable_double("log10e", M_LOG10E, true);
  vmanager->add_variable_double("ln2", M_LN2, true);
  vmanager->add_variable_double("ln10", M_LN10, true);
  vmanager->set_write_global(false);
}


void SDFFparser::add_physics_constants(){
  vmanager->set_write_global(true);
  vmanager->add_variable_double("h", 6.6260689633E-34, true);
  vmanager->add_variable_double("hbar", 1.05457162853E-34, true);
  vmanager->add_variable_double("epsilon0", 8.854187817E-12, true);
  vmanager->add_variable_double("mu0", 12.566370614E-7, true);
  vmanager->add_variable_double("c", 299792458, true);
  vmanager->add_variable_double("ce", 1.60217648740E-19, true);
  vmanager->add_variable_double("muB", 927.40091523E-26, true);
  vmanager->add_variable_double("muB_eV", 5.788381755579E-5, true);
  vmanager->add_variable_double("muN", 5.0507832413E-27, true);
  vmanager->add_variable_double("muN_eV", 3.152451232645E-8, true);
  vmanager->add_variable_double("me", 9.1093821545E-31, true);
  vmanager->add_variable_double("mp", 1.67262163783E-27, true);
  vmanager->add_variable_double("mn", 1.67492721184E-27, true);
  vmanager->add_variable_double("NA", 6.0221417930E23, true);
  vmanager->add_variable_double("kB", 1.380650424E-23, true);
  vmanager->add_variable_double("kB_eV", 8.61734315E-5, true);

  vmanager->set_write_global(false);
}


int SDFFparser::import(std::string filename) {
  clear_events();
  generator->clear_used();
  current_time=-1;
  current_line=1;
  current_filename=filename;
  current_channel="";
  clear_markers();
  inside_sequence=false;
  inside_parameters=false;

  // delete all previous trees and generate empty ones
  delete parameters;
  parameters=new SDFFlistNode(this);
  delete sequence;
  sequence=new SDFFlistNode(this);
  delete safe_settings;
  safe_settings=new SDFFlistNode(this);
  if (last_event!=NULL) delete last_event;
  if (pre_event!=NULL) delete pre_event;
  last_event=NULL;
  pre_event=NULL;

  if (!file_exists(filename)) {
    add_error(SEQUENCER_CSVERROR_FILENOTEXISTS_NUM, format(get_errormessage(SEQUENCER_CSVERROR_FILENOTEXISTS_NUM), filename.c_str()), format(get_errorsmessage(SEQUENCER_CSVERROR_FILENOTEXISTS_NUM), filename.c_str()), filename,-1.0,0);
    return -1;
  };

  // allocate an input stream object for the file and load the file
	try {
    sdff=new std::ifstream(filename.c_str());
    sdff_file(true);

  // now make sure that sdff is destroyed before the exception is passed on to the caller
    delete sdff;
    sdff=NULL;
  } catch (exception E) {
    delete sdff;
    sdff=NULL;
    add_error_p(E.get_number(), E.get_emessage(), E.get_emessage(), current_filename, current_line);
  } catch (std::exception E) {
    delete sdff;
    sdff=NULL;
    add_error_p(-1, E.what(), E.what(), current_filename, current_line);
  }

  // return an error code and execute only if there were no errors
  if (errornum==0) {
    /*std::cout<<"-- parameters: -----------------------------------------------------\n";
    std::cout<<parameters->print()<<std::endl<<std::endl;
    std::cout<<"-- safe_settings: -----------------------------------------------------\n";
    std::cout<<safe_settings->print()<<std::endl<<std::endl;
    std::cout<<"-- sequence: -----------------------------------------------------\n";
    std::cout<<sequence->print()<<std::endl<<std::endl;*/


    // evaluate parameters section
    inside_sequence=false;
    try {
      vmanager->set_write_global(true);
      set_inside_parameters(true);
      parameters->evaluate();
      set_inside_parameters(false);
      vmanager->set_write_global(false);
    } catch (exception E) {
      add_error_p(E.get_number(), E.get_emessage(), E.get_emessage(), current_filename, current_line);
    } catch (std::exception E) {
      add_error_p(-1, E.what(), E.what(), current_filename, current_line);
    }

    //std::cout<<"import end"<<std::endl;
    if (warningnum==0) { return 0; } else { return +1; }
  } else return -1;
}


int SDFFparser::evaluate(std::string program, sdff_result* result) {
  clear_events();
  current_line=1;
  current_filename="";
  SDFFlistNode ln(this);


  // allocate an input stream object for the program string and load it
	try {
    sdff=new std::istringstream(program);
    // parse the expression_seq in the given program
    expression_seq_e(true, &ln);

  // now make sure that sdff is destroyed befor the exception is passed on to the caller
    delete sdff;
    sdff=NULL;
  } catch (exception E) {
    delete sdff;
    sdff=NULL;
    add_error_p(E.get_number(), E.get_emessage(), E.get_emessage(), current_filename, current_line);
  } catch (std::exception E) {
    delete sdff;
    sdff=NULL;
    add_error_p(-1, E.what(), E.what(), current_filename, current_line);
  }

  // return an error code and execute only if there were no errors
  if (errornum==0) {
    try {
      if (result!=NULL) {
        (*result)=ln.evaluate();
      } else {
        ln.evaluate();
      }
    } catch (exception E) {
      delete sdff;
      sdff=NULL;
      add_error_p(E.get_number(), E.get_emessage(), E.get_emessage(), current_filename, current_line);
    } catch (std::exception E) {
      delete sdff;
      sdff=NULL;
      add_error_p(-1, E.what(), E.what(), current_filename, current_line);
    }
    ln.dispose(); // remove data in node and free memory

    if (errornum>0) return -1;
    if (warningnum==0) { return 0; } else { return +1; }
  } else return -1;
}


int SDFFparser::evaluate_parameters(std::string program, sdff_result* result, bool add) {
  clear_events();
  current_line=1;
  current_filename="";
  SDFFlistNode ln(this);


  // allocate an input stream object for the program string and load it
	try {
    sdff=new std::istringstream(program);
    // parse the expression_seq in the given program
    set_inside_parameters(true);
    expression_seq_e(true, &ln);
    set_inside_parameters(false);

  // now make sure that sdff is destroyed befor the exception is passed on to the caller
    delete sdff;
    sdff=NULL;
  } catch (exception E) {
    delete sdff;
    sdff=NULL;
    add_error_p(E.get_number(), E.get_emessage(), E.get_emessage(), current_filename, current_line);
  } catch (std::exception E) {
    delete sdff;
    sdff=NULL;
    add_error_p(-1, E.what(), E.what(), current_filename, current_line);
  }

  // return an error code and execute only if there were no errors
  if (errornum==0) {
    try {
      vmanager->set_write_global(true);
      set_inside_parameters(true);
      if (result!=NULL) {
        (*result)=ln.evaluate();
      } else {
        ln.evaluate();
      }
      set_inside_parameters(false);
      vmanager->set_write_global(false);
      if (add) {
        parameters->add_children(&ln);
        ln.clear_children(false); // clear children without destroying them
      } else {
        ln.dispose(); // remove data in node and free memory
      }
    } catch (exception E) {
      delete sdff;
      sdff=NULL;
      add_error_p(E.get_number(), E.get_emessage(), E.get_emessage(), current_filename, current_line);
    } catch (std::exception E) {
      delete sdff;
      sdff=NULL;
      add_error_p(-1, E.what(), E.what(), current_filename, current_line);
    }

    if (errornum>0) return -1;
    if (warningnum==0) { return 0; } else { return +1; }
  } else return -1;
}



std::string SDFFparser::read_delim(char delimiter){
	std::string res="";
	char ch=0;

 	while(sdff->get(ch)) {
		if (ch=='\n') current_line++;
		if (ch==delimiter) break;
		if ((*sdff) && (ch!=delimiter)) res=res+ch;
	}

	return res;
}

sdff_token SDFFparser::read_number(){
	/**(*sdff) >> current_double;
	if (round(current_double)==current_double) {
    current_integer=(long long)current_double;
    return sdffTINTEGER;
  }
	return sdffTDOUBLE;*/
  current_double=0;
  current_integer=0;

  bool isInt=true;
  bool isMantissa=true;
  long long factor=1;
  double dfactor=1;
  int mantissaPos=0;
  bool isNumber=true;
  bool foundDot=false;


  std::string num="";
  int i=0;
  char c;

  if (sdff->get(c)) {
   // check sign
    if (c=='-') { factor=-1; dfactor=-1; isNumber=sdff->get(c); i++; }
    else if (c=='+') { isNumber=sdff->get(c); i++; }

    while (isNumber) {
      if (!isMantissa) {
        switch(c) {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            num+=c;
            break;
          case '-':
            if (i==mantissaPos) {
              num+=c;
            } else {
              isNumber=false;
              sdff->putback(c);
            }
            break;
          case '+':
            if (i!=mantissaPos) {
              isNumber=false;
              sdff->putback(c);
            }
            break;
          case 'a':
            dfactor*=1e-15;
            isInt=false;
            isNumber=false;
            break;
          case 'f':
            dfactor*=1e-12;
            isInt=false;
            isNumber=false;
            break;
          case 'n':
            dfactor*=1e-9;
            isInt=false;
            isNumber=false;
            break;
          case 'u':
            dfactor*=1e-6;
            isInt=false;
            isNumber=false;
            break;
          case 'm':
            dfactor*=1e-3;
            isInt=false;
            isNumber=false;
            break;
          case 'c':
            dfactor*=1e-2;
            isInt=false;
            isNumber=false;
            break;
          case 'd':
            dfactor*=1e-1;
            isInt=false;
            isNumber=false;
            break;
          case 'k':
            factor*=1000;
            dfactor*=1e3;
            isNumber=false;
            break;
          case 'M':
            factor*=1000000;
            dfactor*=1e6;
            isNumber=false;
            break;
          case 'G':
            factor*=1000000000;
            dfactor*=1e9;
            isNumber=false;
            break;
          case 'T':
            factor*=1000000000;
            factor*=1000;
            dfactor*=1e12;
            isNumber=false;
            break;
          case 'P':
            factor*=1000000000;
            factor*=1000;
            factor*=1000;
            dfactor*=1e15;
            isNumber=false;
            break;
          default:
            sdff->putback(c);
            isNumber=false;
        }
      }
      if (isMantissa) {
        switch(c) {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            num+=c;
            break;
          case '.':
            if (foundDot) {
              sdff->putback(c);
              isNumber=false;
            } else {
              num+=c;
              isInt=false;
              foundDot=true;
            }
            break;
          case '-':
            if (i==mantissaPos) {
              factor=-1;
            } else {
              isNumber=false;
              sdff->putback(c);
            }
            break;
          case '+':
            if (i==mantissaPos) {
              factor=1;
            } else {
              sdff->putback(c);
              isNumber=false;
            }
            break;
          case 'e':
          case 'E':
            isMantissa=false;
            isInt=false;
            num+='e';
            mantissaPos=i+1;
            break;
          case 'a':
            dfactor*=1e-15;
            isInt=false;
            isNumber=false;
            break;
          case 'f':
            dfactor*=1e-12;
            isInt=false;
            isNumber=false;
            break;
          case 'n':
            dfactor*=1e-9;
            isInt=false;
            isNumber=false;
            break;
          case 'u':
            dfactor*=1e-6;
            isInt=false;
            isNumber=false;
            break;
          case 'm':
            dfactor*=1e-3;
            isInt=false;
            isNumber=false;
            break;
          case 'c':
            dfactor*=1e-2;
            isInt=false;
            isNumber=false;
            break;
          case 'd':
            dfactor*=1e-1;
            isInt=false;
            isNumber=false;
            break;
          case 'k':
            factor*=1000;
            dfactor*=1e3;
            isNumber=false;
            break;
          case 'M':
            factor*=1000000;
            dfactor*=1e6;
            isNumber=false;
            break;
          case 'G':
            factor*=1000000000;
            dfactor*=1e9;
            isNumber=false;
            break;
          case 'T':
            factor*=1000000000;
            factor*=1000;
            dfactor*=1e12;
            isNumber=false;
            break;
          case 'P':
            factor*=1000000000;
            factor*=1000;
            factor*=1000;
            dfactor*=1e15;
            isNumber=false;
            break;
          default:
            sdff->putback(c);
            isNumber=false;
        }
      }
      i++;
      if (isNumber) isNumber=sdff->get(c);
    }
  }

  if (num.length()<=0) num="0";
  if (isInt) {
    current_integer=atoll(num.c_str());
    current_integer = factor*(current_integer);
    return sdffTINTEGER; // this is an integer
  } else {
    current_double = atof(num.c_str());
    current_double=(current_double)*dfactor;
    return sdffTDOUBLE;
  }
}

sdff_token SDFFparser::get_token(){
	char ch=0;
	unsigned char comment=0;  // 0: no comment
                            // 1: a comment that ends with a linebreak
                            // 2: a comment that ends with */

  void* get_ok=NULL;
  do {
    if(sdff->eof()) ch=0; else get_ok=sdff->get(ch);

		switch(ch) {
      case '\n':
        // increase line count on every linebreak
        current_line++;
        // a one-line comment stops here
        if (comment==1) comment=0;
        break;
      case '#':
        if (comment==0) {
          // if we are not inside a comment, start a new comment
          comment=1;
        }
        break;
      case '/':
        if (comment==0) {
          // start a multiline comment /* ... */  if the next character is '*', i.e. we read '/*'
          // to do so peek the next character
          if (sdff->peek()  == '*') {
            comment=2;
            get_ok=sdff->get(ch); // eat the '*'
            ch=0; // make sure to read one more character
          }
          if (sdff->peek()  == '/') {
            comment=1;
            get_ok=sdff->get(ch); // eat the '/'
            ch=0; // make sure to read one more character
          }
        }
        break;
      case '*':
        if (comment==2) {
          // peek at the next character if this is '/' then the comment ends
          if (sdff->peek()  == '/') {
            comment=0;
            get_ok=sdff->get(ch); // eat the '/'
            ch=0; // make sure to read one more character
          }
        }
        break;
    }
  } while (get_ok && ((comment>0) || (ch<=32))); // isspace(ch)

  //std::cout<<chartoprintablestr(ch)<<std::endl;
	if (!get_ok) ch=0;
  switch (ch) {
		case 0:
			return current_token=sdffTEND;
		case ';':
			return current_token=sdffTPRINT;
		case '*':
			return current_token=sdffTMUL;
		case '/':
			return current_token=sdffTDIV;
		case '%':
			return current_token=sdffTMODULO;
		case '+':
      return current_token=sdffTPLUS;
		case '-':
      return current_token=sdffTMINUS;
		case '[':
			return current_token=sdffTLBRACKET;
		case ']':
			return current_token=sdffTRBRACKET;
		case '(':
			return current_token=sdffTLPARANTHESIS;
		case ')':
			return current_token=sdffTRPARANTHESIS;
		case '{':
			return current_token=sdffTLBRACE;
		case '}':
			return current_token=sdffTRBRACE;
		case ',':
			return current_token=sdffTKOMMA;
		case '\'':
		case '"': {
		  current_string=read_delim(ch);
    	return current_token=sdffTSTRING;
    }
		case '^':
			return current_token=sdffTPOWER;
		case '~':
			return current_token=sdffTINVERT;
		case '!':{
			char ch1=0;
			if (*sdff) sdff->get(ch1);
			if (ch1=='=') return current_token=sdffTCOMP_UNEQUAL;
			// else
			sdff->putback(ch1);
			return current_token=sdffTLOGIC_NOT;
		}
		case '&':
			return current_token=sdffTLOGIC_AND;
		case '=':{
			char ch1=0;
			if (*sdff) sdff->get(ch1);
			if (ch1=='=') return current_token=sdffTCOMP_EQUAL;
			// else
			sdff->putback(ch1);
			return current_token=sdffTASSIGN;
		}
		case '>':{
			char ch1=0;
			if (*sdff) sdff->get(ch1);
			if (ch1=='=') return current_token=sdffTCOMP_GEQUAL;
			if (ch1=='>') return current_token=sdffTLOGIC_SHR;
			// else
			sdff->putback(ch1);
			return current_token=sdffTCOMP_GREATER;
		}
		case '<':{
			char ch1=0;
			if (*sdff) sdff->get(ch1);
			if (ch1=='=') return current_token=sdffTCOMP_SEQUAL;
			if (ch1=='<') return current_token=sdffTLOGIC_SHL;
			// else
			sdff->putback(ch1);
			return current_token=sdffTCOMP_SMALLER;
		}
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case '.': {
			sdff->putback(ch);
			return current_token=read_number();
		}
		default:
			if (isalpha(ch) || (ch=='_')) { // try to recognize NAME, LOGIC_TRUE, LOGIC_FALSE, DIFF_LBRACKET
				current_string=ch;
				while (sdff->get(ch) && (isalnum(ch) || (ch=='_') || (ch=='.')))
    			if (isalnum(ch) || (ch=='_') || (ch=='.')) current_string.push_back(ch);

				sdff->putback(ch); // now put the last thing read back int the stream, as it
									  // could belong to the next token

				if (current_string=="true") return current_token=sdffTLOGIC_TRUE;
				if (current_string=="false") return current_token=sdffTLOGIC_FALSE;
				if (current_string=="and") return current_token=sdffTLOGIC_AND;
				if (current_string=="or") return current_token=sdffTLOGIC_OR;
				if (current_string=="xor") return current_token=sdffTLOGIC_XOR;
				if (current_string=="not") return current_token=sdffTLOGIC_NOT;
				if (current_string=="shl") return current_token=sdffTLOGIC_SHL;
				if (current_string=="shr") return current_token=sdffTLOGIC_SHR;
				if (current_string=="nor") return current_token=sdffTLOGIC_NOR;
				if (current_string=="nand") return current_token=sdffTLOGIC_NAND;
				if (current_string=="mod") return current_token=sdffTMODULO;
				if (current_string=="if") return current_token=sdffTIF;
				if (current_string=="elseif") return current_token=sdffTELSEIF;
				if (current_string=="else") return current_token=sdffTELSE;
				if (current_string=="while") return current_token=sdffTWHILE;
				if (current_string=="do") return current_token=sdffTDO;
				if (current_string=="until") return current_token=sdffTUNTIL;
				if (current_string=="integer") return current_token=sdffTTINTEGER;
				if (current_string=="int") return current_token=sdffTTINTEGER;
				if (current_string=="double") return current_token=sdffTTDOUBLE;
				if (current_string=="float") return current_token=sdffTTDOUBLE;
				if (current_string=="real") return current_token=sdffTTDOUBLE;
				if (current_string=="boolean") return current_token=sdffTTBOOLEAN;
				if (current_string=="bool") return current_token=sdffTTBOOLEAN;
				if (current_string=="string") return current_token=sdffTTSTRING;
				if (current_string=="const") return current_token=sdffTCONST;
				if (current_string=="parameters") return current_token=sdffTPARAMETERS;
				if (current_string=="sequence") return current_token=sdffTSEQUENCE;
				if (current_string=="event") return current_token=sdffTEVENT;
				if (current_string=="pre_event") return current_token=sdffTPREEVENT;
				if (current_string=="last_event") return current_token=sdffTLASTEVENT;
				if (current_string=="relative_event") return current_token=sdffTRELATIVEEVENT;
				if (current_string=="safe_settings") return current_token=sdffTSAFESETTINGS;
				if (current_string=="channel") return current_token=sdffTCHANNEL;
				if (current_string=="channel_names") return current_token=sdffTCHANNELNAMES;

				return current_token=sdffTNAME;
			}
			// the parser has found an unknown token. an exception will be thrown
			SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNKNOWNTOKEN_NUM, "SDFFparser.get_token()", chartoprintablestr(ch).c_str());
	}
}


void SDFFparser::sdff_file(bool get) {
  char expect[]="parameters{...}, safe_settings{...} or sequence{...}";
  char parameters_help[]="parameters{...}";
  char safesettings_help[]="safe_settings{...}";
  char sequence_help[]="sequence{...}";
  char preevent_help[]="pre_event {instruction; ... }";
  char lastevent_help[]="last_event {instruction; ... }";
  if (get) get_token();
  inside_parameters=false;
  while(true) {
    switch(current_token) {
      case sdffTPARAMETERS:
        // check for opening '{' ... which will have to be read in.
        if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.sdff_file()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), parameters_help);
        set_inside_parameters(true);
        expression_seq(true, parameters);
        set_inside_parameters(false);
        if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.sdff_file()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), parameters_help);
        get_token(); // swallow '}'
        if (current_token==sdffTPRINT) get_token(); // swallow optional ';'
        break;
      case sdffTSAFESETTINGS:
        // check for opening '{' ... which will have to be read in.
        if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.sdff_file()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), safesettings_help);
        expression_seq(true, safe_settings);
        if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.sdff_file()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), safesettings_help);
        get_token(); // swallow '}'
        if (current_token==sdffTPRINT) get_token(); // swallow optional ';'
        break;
      case sdffTSEQUENCE:
        // check for opening '{' ... which will have to be read in.
        if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.sdff_file()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), sequence_help);
        event_seq(true, sequence);
        if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.sdff_file()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), sequence_help);
        get_token(); // swallow '}'
        if (current_token==sdffTPRINT) get_token(); // swallow optional ';'
        break;
      case sdffTPREEVENT:{ // found an pre_event: parse the contents
          if (pre_event==NULL) pre_event=new SDFFpreEventNode(this); // create pre_event node
          //list->add_child(ne);
          // check for opening '{' ... which will have to be read in.
          if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), preevent_help);
          // now we expect a sequence of instructions, ifs, whiles ...
          set_inside_preevent(true);
          expression_seq(true, pre_event);
          set_inside_preevent(false);
          // here we expect the closing '}' of the event block ... already read by expression_seq()
          if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), preevent_help);
          get_token(); // swallow '}'
          if (current_token==sdffTPRINT) get_token(); // swallow optional ';'
        }
        break;
      case sdffTLASTEVENT:{ // found an last_event: parse the contents
          if (last_event==NULL) last_event=new SDFFlastEventNode(this); // create last_event node
          //list->add_child(ne);
          // check for opening '{' ... which will have to be read in.
          if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), lastevent_help);
          // now we expect a sequence of instructions, ifs, whiles ...
          set_inside_lastevent(true);
          expression_seq(true, last_event);
          set_inside_lastevent(false);
          // here we expect the closing '}' of the event block ... already read by expression_seq()
          if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), lastevent_help);
          get_token(); // swallow '}'
          if (current_token==sdffTPRINT) get_token(); // swallow optional ';'
        }
        break;
      case sdffTEND: // we reached end of file
        return;
      default: // unexpected token
        SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKEN_NUM, "SDFFparser.sdff_file()", expect, asdff_tokentostr(current_token).c_str());
    }
  }
};



void SDFFparser::event_seq(bool get, SDFFnaryNode* list) {
  char expect[]="list of 'event(..){...}', 'relative_event(..){...}', 'pre_event{...}', 'if'- or loop-statements, and 'instruction;' or closing '}'";
  char if_help[]="if (<test>) {instruction; ... }  ,  if (<test>) {instruction; ... } else {instruction; ... }";
  char elseif_help[]="if (<test>) {instruction; ... } elseif (<test>) {instruction; ... } ... else {instruction; ... }";
  char do_help[]="do {instruction; ... } while (<test>);  ,  do {instruction; ... } until (<test>);";
  char while_help[]="while(<test>) {instruction; ... }";
  //char instruction_help[]="instruction ;";
  char event_help[]="event(<time>) {instruction; ... }";
  char revent_help[]="relative_event(<rel. time>) {instruction; ... }";
  char channel_help[]="channel(<channel_name>) {instruction; ... }";
  if (get) get_token();
  while(true) { // we expect a list of events, relative_events and instructions
    switch(current_token) {
      case sdffTCHANNEL:{ // found an event: parse the parameter and the contents
          if (get_token()!=sdffTLPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), channel_help);
          SDFFchannelNode* ne=new SDFFchannelNode(this, math_expression(true)); // create channel node and read the parameter
          list->add_child(ne);
          // check for closing ')', this was already read by math_expression()!
          if (current_token!=sdffTRPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), channel_help);
          // check for opening '{' ... which will have to be read in.
          if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), channel_help);
          // now we expect a sequence of instructions, ifs, whiles ...
          expression_seq(true, ne);
          // here we expect the closing '}' of the event block ... already read by expression_seq()
          if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), channel_help);
          get_token(); // swallow '}'
          if (current_token==sdffTPRINT) get_token(); // swallow optional ';'
        }
        break;
      case sdffTEVENT:{ // found an event: parse the parameter and the contents
          if (get_token()!=sdffTLPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), event_help);
          SDFFeventNode* ne=new SDFFeventNode(this, math_expression(true), false); // create event node and read the time parameter
          list->add_child(ne);
          // check for closing ')', this was already read by math_expression()!
          if (current_token!=sdffTRPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), event_help);
          // check for opening '{' ... which will have to be read in.
          if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), event_help);
          // now we expect a sequence of instructions, ifs, whiles ...
          set_inside_event(true);
          expression_seq(true, ne);
          set_inside_event(false);
          // here we expect the closing '}' of the event block ... already read by expression_seq()
          if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), event_help);
          get_token(); // swallow '}'
          if (current_token==sdffTPRINT) get_token(); // swallow optional ';'
        }
        break;
      case sdffTRELATIVEEVENT:{ // found an event: parse the parameter and the contents
          if (get_token()!=sdffTLPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), revent_help);
          SDFFeventNode* nr=new SDFFeventNode(this, math_expression(true), true); // create event node with and read time parameter
          list->add_child(nr);
          // check for closing ')', this was already read by math_expression()!
          if (current_token!=sdffTRPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), revent_help);
          // check for opening '{' ... which will have to be read in.
          if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), revent_help);
          // now we expect a sequence of instructions, ifs, whiles ...
          set_inside_event(true);
          expression_seq(true, nr);
          set_inside_event(false);
          // here we expect the closing '}' of the event block ... already read by expression_seq()
          if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), revent_help);
          get_token(); // swallow '}'
          if (current_token==sdffTPRINT) get_token(); // swallow optional ';'
        }
        break;

      case sdffTRBRACE: // closing '}' brace of the current block
        return;
      case sdffTIF:{ // found an if expression
        if (get_token()!=sdffTLPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), if_help);
        SDFFnode* t=logical_expression(true); // read in the <test> parameter
        SDFFifNode* n=new SDFFifNode(this, t); // create if node with the read test parameter
        // check for closing ')', this was already read by math_expression()!
        if (current_token!=sdffTRPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), if_help);
        // check for opening '{' ... which will have to be read in.
        if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), if_help);
        // now we expect a sequence of instructions, ifs, whiles ...
        SDFFlistNode ln(this); // this node stores all the node in the read block temporarily ...
                               // ... this is needed because SDFFifNode is not a descendent of SDFFnaryNode
                               // which is a required parameter of event_seq() !
        event_seq(true, &ln);
        n->add_true_children(&ln);
        ln.clear_children(false); // empty the temporary list node ln
        // here we expect the closing '}' of the event block ... already read by event_seq()
        if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), if_help);
        get_token(); // swallow '}'
        if (current_token==sdffTELSE) { // there is also an else block ...
          // check for opening '{' ... which will have to be read in.
          if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), if_help);
          event_seq(true, &ln);
          n->add_false_children(&ln);
          ln.clear_children(false); // empty the temporary list node ln
          if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), if_help);
          get_token(); // swallow '}' and read a new token for the next round ;-)
        } else if (current_token==sdffTELSEIF) {
            SDFFifNode* n2=n;
            // here we check for a sequence of 'elseif'
            while(current_token==sdffTELSEIF) {
                if (get_token()!=sdffTLPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), elseif_help);
                SDFFnode* t=logical_expression(true); // read in the <test> parameter
                SDFFifNode* n1=new SDFFifNode(this, t); // create if node with the read test parameter
                n2->add_false_child(n1);
                n2=n1;
                // check for closing ')', this was already read by math_expression()!
                if (current_token!=sdffTRPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), elseif_help);
                // check for opening '{' ... which will have to be read in.
                if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), elseif_help);
                event_seq(true, &ln);
                n1->add_true_children(&ln);
                ln.clear_children(false); // empty the temporary list node ln
                if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), elseif_help);
                get_token(); // swallow '}' and read a new token for the next round ;-)
            }
            // now we check for the final 'else'
            if (current_token==sdffTELSE) { // there is also an else block ...
                // check for opening '{' ... which will have to be read in.
                if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), elseif_help);
                event_seq(true, &ln);
                n2->add_false_children(&ln);
                ln.clear_children(false); // empty the temporary list node ln
                if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), elseif_help);
                get_token(); // swallow '}' and read a new token for the next round ;-)
            }
        }
        if (current_token==sdffTPRINT) get_token(); // swallow optional ';'
        list->add_child(n);
      }
      break;

      case sdffTWHILE: { // found a while loop
        if (get_token()!=sdffTLPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), while_help);
        SDFFwhileNode* n=new SDFFwhileNode(this, logical_expression(true)); // create if node and read test parameter
        // check for closing ')', this was already read by math_expression()!
        if (current_token!=sdffTRPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), while_help);
        // check for opening '{' ... which will have to be read in.
        if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), while_help);
        // now we expect a sequence of instructions, ifs, whiles ...
        event_seq(true, n);
        // here we expect the closing '}' of the event block ... already read by event_seq()
        if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), while_help);
        get_token(); // swallow '}' and read a new token for the next round ;-)
        if (current_token==sdffTPRINT) get_token(); // swallow optional ';'
        list->add_child(n);
      }
      break;

      case sdffTDO:
      case sdffTREPEAT: { // found a do .. while or do .. until loop
        // check for opening '{' ... which will have to be read in.
        if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), do_help);
        // now we expect a sequence of instructions, ifs, whiles ...
        SDFFlistNode ln(this); // this node stores all the node in the read block temporarily ...
                               // ... this is needed because we can not determine the type of the loop (do ... while, do ... until)
                               // so we first read the sub-nodes into a temporary list node and then copy them to the
                               // final dowhile or dountil node.
        event_seq(true, &ln);
        if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), do_help);
        get_token(); // swallow '}' read the next token ... this has to be while or until !!!
        SDFFtestloopNode* n=NULL;
        if (current_token==sdffTWHILE) {
          if (get_token()!=sdffTLPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), do_help);
          n=new SDFFdowhileNode(this, logical_expression(true));
        } else if (current_token==sdffTUNTIL) {
          if (get_token()!=sdffTLPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), do_help);
          n=new SDFFrepeatuntilNode(this, logical_expression(true));
        } else SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), do_help);
        n->add_children(&ln);
        ln.clear_children(false); // empty the temporary list node ln
        // check for closing ')', this was already read by math_expression()!
        if (current_token!=sdffTRPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), do_help);
        // check for ';' and swallow ')'
        if (get_token()!=sdffTPRINT) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTPRINT).c_str(), asdff_tokentostr(current_token).c_str(), do_help);
        get_token(); // swallow ';' and read a new token for the next round ;-)
        list->add_child(n);
      }
      break;

      case sdffTEND:
        SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKEN_NUM, "SDFFparser.event_seq()", expect, asdff_tokentostr(current_token).c_str());
      default:{ // we did not recognize event or relative_event ... so this has to be an instruction
          SDFFnode* n=logical_expression(false);
          if (current_token!=sdffTPRINT) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKEN_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTPRINT).c_str(), asdff_tokentostr(current_token).c_str());
          get_token(); // swallow ';' and read a new token for the next round ;-)
          list->add_child(n);
        }
        break;
    }
  }
};



void SDFFparser::expression_seq(bool get, SDFFnaryNode* list) {
  char expect[]="list of 'instruction;' or closing '}'";
  if (get) get_token();
  while(true) { // we expect a list of events, relative_events and instructions
    switch(current_token) {
      case sdffTRBRACE: // closing '}' brace of the current block
        return;
      case sdffTEND:
        SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKEN_NUM, "SDFFparser.expression_seq()", expect, asdff_tokentostr(current_token).c_str());
      default: {
          SDFFnode* n=instruction(false);
          list->add_child(n);
        }
        break;
    }
  }
};



void SDFFparser::expression_seq_e(bool get, SDFFnaryNode* list) {
  //char expect[]="list of 'instruction;'";
  if (get) get_token();
  while(true) { // we expect a list of events, relative_events and instructions
    switch(current_token) {
      case sdffTEND:
        return;
      default: {
          SDFFnode* n=instruction(false);
          list->add_child(n);
        }
        break;
    }
  }
};








SDFFnode* SDFFparser::instruction(bool get) {
  char expect[]="if-statement, loop or instruction;";
  char if_help[]="if (<test>) {instruction; ... }  ,  if (<test>) {instruction; ... } else {instruction; ... }";
  char elseif_help[]="if (<test>) {instruction; ... } elseif (<test>) {instruction; ... } ... else {instruction; ... }";
  char do_help[]="do {instruction; ... } while (<test>);  ,  do {instruction; ... } until (<test>);";
  char while_help[]="while(<test>) {instruction; ... }";
  char channelnames_help[]="channel_names { channel, alias[, 'info']; ... }";
  if (get) get_token();
  switch(current_token) {
    case sdffTIF:{ // found an if expression
      if (get_token()!=sdffTLPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTLPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), if_help);
      SDFFnode* t=logical_expression(true); // read in the <test> parameter
      SDFFifNode* n=new SDFFifNode(this, t); // create if node with the read test parameter
      // check for closing ')', this was already read by math_expression()!
      if (current_token!=sdffTRPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTRPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), if_help);
      // check for opening '{' ... which will have to be read in.
      if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), if_help);
      // now we expect a sequence of instructions, ifs, whiles ...
      SDFFlistNode ln(this); // this node stores all the node in the read block temporarily ...
                             // ... this is needed because SDFFifNode is not a descendent of SDFFnaryNode
                             // which is a required parameter of expression_seq() !
      expression_seq(true, &ln);
      n->add_true_children(&ln);
      ln.clear_children(false); // empty the temporary list node ln
      // here we expect the closing '}' of the event block ... already read by expression_seq()
      if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), if_help);
      get_token(); // swallow '}'
      if (current_token==sdffTELSE) { // there is also an else block ...
        // check for opening '{' ... which will have to be read in.
        if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), if_help);
        expression_seq(true, &ln);
        n->add_false_children(&ln);
        ln.clear_children(false); // empty the temporary list node ln
        if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), if_help);
        get_token(); // swallow '}' and read a new token for the next round ;-)
      } else if (current_token==sdffTELSEIF) {
            SDFFifNode* n2=n;
            // here we check for a sequence of 'elseif'
            while(current_token==sdffTELSEIF) {
                if (get_token()!=sdffTLPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), elseif_help);
                SDFFnode* t=logical_expression(true); // read in the <test> parameter
                SDFFifNode* n1=new SDFFifNode(this, t); // create if node with the read test parameter
                n2->add_false_child(n1);
                n2=n1;
                // check for closing ')', this was already read by math_expression()!
                if (current_token!=sdffTRPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), elseif_help);
                // check for opening '{' ... which will have to be read in.
                if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), elseif_help);
                expression_seq(true, &ln);
                n1->add_true_children(&ln);
                ln.clear_children(false); // empty the temporary list node ln
                if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), elseif_help);
                get_token(); // swallow '}' and read a new token for the next round ;-)
            }
            // now we check for the final 'else'
            if (current_token==sdffTELSE) { // there is also an else block ...
                // check for opening '{' ... which will have to be read in.
                if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), elseif_help);
                expression_seq(true, &ln);
                n2->add_false_children(&ln);
                ln.clear_children(false); // empty the temporary list node ln
                if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.event_seq()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), elseif_help);
                get_token(); // swallow '}' and read a new token for the next round ;-)
            }
      }
      if (current_token==sdffTPRINT) get_token(); // swallow optional ';'
      return n;
    }

    case sdffTWHILE: { // found a while loop
      if (get_token()!=sdffTLPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTLPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), while_help);
      SDFFwhileNode* n=new SDFFwhileNode(this, logical_expression(true)); // create if node and read test parameter
      // check for closing ')', this was already read by math_expression()!
      if (current_token!=sdffTRPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTRPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), while_help);
      // check for opening '{' ... which will have to be read in.
      if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), while_help);
      // now we expect a sequence of instructions, ifs, whiles ...
      expression_seq(true, n);
      // here we expect the closing '}' of the event block ... already read by expression_seq()
      if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), while_help);
      get_token(); // swallow '}' and read a new token for the next round ;-)
      if (current_token==sdffTPRINT) get_token(); // swallow optional ';'
      return n;
    }

    case sdffTDO:
    case sdffTREPEAT: { // found a do .. while or do .. until loop
      // check for opening '{' ... which will have to be read in.
      if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), do_help);
      // now we expect a sequence of instructions, ifs, whiles ...
      SDFFlistNode ln(this); // this node stores all the node in the read block temporarily ...
                             // ... this is needed because we can not determine the type of the loop (do ... while, do ... until)
                             // so we first read the sub-nodes into a temporary list node and then copy them to the
                             // final dowhile or dountil node.
      expression_seq(true, &ln);
      if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), do_help);
      get_token(); // swallow '}' read the next token ... this has to be while or until !!!
      SDFFtestloopNode* n=NULL;
      if (current_token==sdffTWHILE) {
        if (get_token()!=sdffTLPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTLPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), do_help);
        n=new SDFFdowhileNode(this, logical_expression(true));
      } else if (current_token==sdffTUNTIL) {
        if (get_token()!=sdffTLPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTLPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), do_help);
        n=new SDFFrepeatuntilNode(this, logical_expression(true));
      } else SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTRPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), do_help);
      n->add_children(&ln);
      ln.clear_children(false); // empty the temporary list node ln
      // check for closing ')', this was already read by math_expression()!
      if (current_token!=sdffTRPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTRPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), do_help);
      // check for ';' and swallow ')'
      if (get_token()!=sdffTPRINT) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTPRINT).c_str(), asdff_tokentostr(current_token).c_str(), do_help);
      get_token(); // swallow ';' and read a new token for the next round ;-)
      return n;
    }
    case sdffTCHANNELNAMES: { // found a channel_names loop
      if (!get_inside_parameters()) SEQUENCER_ERRORN(SEQUENCER_SDFFERROR_CHANNELNAMESONLYINPARAM_NUM, "SDFFparser.instruction()");
      // check for opening '{' ... which will have to be read in.
      if (get_token()!=sdffTLBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTLBRACE).c_str(), asdff_tokentostr(current_token).c_str(), channelnames_help);
      // now we expect a sequence of instructions, ifs, whiles ...
      get_token();
      SDFFchannelnamesNode* n=new SDFFchannelnamesNode(this);
      while (current_token!=sdffTRBRACE) {
          if (current_token!=sdffTNAME) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", (asdff_tokentostr(sdffTNAME)+" (channel)").c_str(), asdff_tokentostr(current_token).c_str(), channelnames_help);
          std::string channel=current_string;
          if (get_token()!=sdffTKOMMA) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTKOMMA).c_str(), asdff_tokentostr(current_token).c_str(), channelnames_help);
          if (get_token()!=sdffTNAME) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", (asdff_tokentostr(sdffTNAME)+" (alias)").c_str(), asdff_tokentostr(current_token).c_str(), channelnames_help);
          std::string alias=current_string;
          std::string info="";
          if (get_token()==sdffTKOMMA) {
              if (get_token()!=sdffTSTRING) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", (asdff_tokentostr(sdffTSTRING)+" (info)").c_str(), asdff_tokentostr(current_token).c_str(), channelnames_help);
              info=current_string;
              get_token();
          }
          if (current_token!=sdffTPRINT) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTPRINT).c_str(), asdff_tokentostr(current_token).c_str(), do_help);
          n->add_item(channel, alias, info);
          //std::cout<<"added:  "<<channel<<"  "<<alias<<"  "<<info<<std::endl;
          get_token();
      }
      // here we expect the closing '}' of the event block ... already read by expression_seq()
      if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), channelnames_help);
      get_token(); // swallow '}' and read a new token for the next round ;-)
      if (current_token==sdffTPRINT) get_token(); // swallow optional ';'
      return n;
    }

    case sdffTEND:
      SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKEN_NUM, "SDFFparser.instruction()", expect, asdff_tokentostr(current_token).c_str());
    default:
      SDFFnode* n=logical_expression(false);
      if (current_token!=sdffTPRINT) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKEN_NUM, "SDFFparser.instruction()", asdff_tokentostr(sdffTPRINT).c_str(), asdff_tokentostr(current_token).c_str());
      get_token(); // swallow ';' and read a new token for the next round ;-)
      return n;
  }
};













SDFFnode* SDFFparser::logical_expression(bool get) {
  SDFFnode* left=logical_term(get);
	while(true) // forever, do until you find anything else than an expressions ... this allows for
	            // chains like a+b+c+d+e+...
		switch(current_token) {
			case sdffTLOGIC_OR:
				left= (SDFFnode*)new SDFFlogicNode(this, left, logical_term(true), sdffLOOR );
				break;
			case sdffTLOGIC_NOR:
				left= (SDFFnode*)new SDFFlogicNode(this, left, logical_term(true), sdffLONOR );
				break;
			case sdffTLOGIC_XOR:
				left= (SDFFnode*)new SDFFlogicNode(this, left, logical_term(true), sdffLOXOR );
				break;
				break;
			default:
				return left;
		}
};





SDFFnode* SDFFparser::logical_term(bool get) {
  SDFFnode* left=comp_expression(get);
	while(true) // forever, do until you find anything else than an expressions ... this allows for
	            // chains like a+b+c+d+e+...
		switch(current_token) {
			case sdffTLOGIC_AND:
				left= (SDFFnode*)new SDFFlogicNode(this, left, comp_expression(true), sdffLOAND );
				break;
			case sdffTLOGIC_NAND:
				left= (SDFFnode*)new SDFFlogicNode(this, left, comp_expression(true), sdffLONAND );
				break;
/*			case sdffTLOGIC_SHL:
				left= (SDFFnode*)new SDFFlogicNode(this, left, math_expression(true), sdffLOSHL );
				break;
			case sdffTLOGIC_SHR:
				left= (SDFFnode*)new SDFFlogicNode(this, left, math_expression(true), sdffLOSHR );
				break;*/
			default:
				return left;
		}
};





SDFFnode* SDFFparser::comp_expression(bool get) {
  SDFFnode* left=math_expression(get);
	while(true) // forever, do until you find anything else than an expressions ... this allows for
	            // chains like a+b+c+d+e+...
		switch(current_token) {
			case sdffTCOMP_EQUAL:
				left= (SDFFnode*)new SDFFcompareNode(this, left, math_expression(true), sdffCOEqual );
				break;
			case sdffTCOMP_UNEQUAL:
				left= (SDFFnode*)new SDFFcompareNode(this, left, math_expression(true), sdffCOUnequal );
				break;
			case sdffTCOMP_GREATER:
				left= (SDFFnode*)new SDFFcompareNode(this, left, math_expression(true), sdffCOBigger );
				break;
			case sdffTCOMP_SMALLER:
				left= (SDFFnode*)new SDFFcompareNode(this, left, math_expression(true), sdffCOLess );
				break;
			case sdffTCOMP_GEQUAL:
				left= (SDFFnode*)new SDFFcompareNode(this, left, math_expression(true), sdffCOBiggerEqual );
				break;
			case sdffTCOMP_SEQUAL:
				left= (SDFFnode*)new SDFFcompareNode(this, left, math_expression(true), sdffCOLessEqual );
				break;
			default:
				return left;
		}
};




SDFFnode* SDFFparser::math_expression(bool get) {
  SDFFnode* left=term(get);
	while(true) // forever, do until you find anything else than an expressions ... this allows for
	            // chains like a+b+c+d+e+...
		switch(current_token) {
			case sdffTPLUS:
				left= (SDFFnode*)new SDFFarithmeticNode(this, left, term(true), sdffAOAdd );
				break;
			case sdffTMINUS:
				left= (SDFFnode*)new SDFFarithmeticNode(this, left, term(true), sdffAOSubtract );
				break;
			default:
				return left;
		}
};





SDFFnode* SDFFparser::term(bool get) {
  SDFFnode* left=primary(get);
	while(true) // forever, do until you find anything else than an expressions ... this allows for
	            // chains like a+b+c+d+e+...
		switch(current_token) {
			case sdffTDIV:
				left= (SDFFnode*)new SDFFarithmeticNode(this, left, primary(true), sdffAODivide );
				break;
			case sdffTMUL:
				left= (SDFFnode*)new SDFFarithmeticNode(this, left, primary(true), sdffAOMultiply );
				break;
			case sdffTMODULO:
				left= (SDFFnode*)new SDFFarithmeticNode(this, left, primary(true), sdffAOModulo );
				break;
			default:
				return left;
		}
};





SDFFnode* SDFFparser::primary(bool get) {
  char expect[]="primary";
  SDFFnode* res=NULL;
  if (get) get_token();

  // we first check for the simple primaries ... and at last for the poer operator '^'
  //                                             -> see behind the switch-statement
  //
  // the get_token() instruction is included in most case-statements to read in a new token.
  // this token is then subsequently testet for a power operator '^'. But there are some
  // case-statements that for the one or the other reason already have to check the next token.
  // in them we can easily omit the get_token() command.
  switch(current_token) {

    // constant values of different types:
    case sdffTDOUBLE: {
        SDFFconstantNode* nn=new SDFFconstantNode(this);
        nn->set_double(current_double);
        res= nn;
        get_token(); // swallow constante
        break;
      }
    case sdffTINTEGER: {
        SDFFconstantNode* nn=new SDFFconstantNode(this);
        nn->set_integer(current_integer);
        res= nn;
        get_token(); // swallow constante
        break;
      }
    case sdffTSTRING: {
        SDFFconstantNode* nn=new SDFFconstantNode(this);
        nn->set_string(current_string);
        res= nn;
        get_token(); // swallow constante
        break;
      }
    case sdffTLOGIC_TRUE: {
        SDFFconstantNode* nn=new SDFFconstantNode(this);
        nn->set_boolean(true);
        res= nn;
        get_token(); // swallow 'true'
        break;
      }
    case sdffTLOGIC_FALSE: {
        SDFFconstantNode* nn=new SDFFconstantNode(this);
        nn->set_boolean(false);
        res= nn;
        get_token(); // swallow 'false'
        break;
      }


    // unary operators (negation ...): (not|-|+|!|~) <primary>
    case sdffTPLUS: {
        res= primary(true);
        break;
      }
    case sdffTMINUS: {
        SDFFnode* p=primary(true);
        //SDFFnegateNode* n;
        if (SDFFconstantNode* c=dynamic_cast<SDFFconstantNode*>(p)) { // this is just -[number] ... so we combine it to a negative constant
          res=c;
          c->negate();
        } else {
          res=new SDFFnegateNode(this, p, sdffNTMinus);
        }
        break;
      }
    case sdffTLOGIC_NOT: {
        SDFFnegateNode* n=new SDFFnegateNode(this, primary(true), sdffNTLogic);
        res= n;
        //get_token();
        break;
      }
    case sdffTINVERT: {
        SDFFnegateNode* n=new SDFFnegateNode(this, primary(true), sdffNTBitwise);
        res= n;
        //get_token();
        break;
      }

    // a left paranthesis '(' ... we now expect a primary in parantheses: (<primary>)
    case sdffTLPARANTHESIS: {
        // read the expression
        SDFFnode* n=logical_expression(true);
        // check whether the expression is closed by ')'
        if (current_token!=sdffTRPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.primary()", asdff_tokentostr(sdffTRPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), "( <primary> )");
        res= n;
        get_token(); // swallow ')'
        break;
      }


    // a name token has been recognized
    case sdffTNAME: {
        std::string name=current_string;
        get_token(); // here we have to check the next token ... if it's '(' we have a function or function assignment
                     //                                      ... if it's '[' this is a channel assignment ...
                     //                                      ... if it's '=' we have an assignment
        if (current_token==sdffTASSIGN) { // variable assignment
          SDFFvariableassignNode* n=new SDFFvariableassignNode(this, name, logical_expression(true));
          res=n;
        } else if (current_token==sdffTLBRACKET) { // we found a variable assignment with a parameter in [  ]

          // check whether this is a channel name:
          //if (!cmanager->exists(resolve_alias(name))) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_VARASSIGNNOBRACKETS_NUM, "SDFFparserNode.primary()", name.c_str());

          // parse parameter
          SDFFnode* p=logical_expression(true);
          if (current_token!=sdffTRBRACKET) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.primary()", asdff_tokentostr(sdffTRBRACKET).c_str(), asdff_tokentostr(current_token).c_str(), "variable[...]=");
          get_token(); // swallow ']'

          // check for assignment '=' character and generate a variable assignment node
          if (current_token==sdffTASSIGN) {// we have a function assign node
            SDFFvariableassignNode* n=new SDFFvariableassignNode(this, name, logical_expression(true));
            n->set_parameter(p);
            res=n;
          } else {
            SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.primary()", asdff_tokentostr(sdffTASSIGN).c_str(), asdff_tokentostr(current_token).c_str(), "variable[...]=");
          }
        } else if (current_token==sdffTLPARANTHESIS) { // we found a function call
          /* This is a function ... but we can not yet decide whether this is a function definition or
           * a function call. If its a definition all the arguments have to be names, if it is a call all
           * the arguments may be names, but they may also be expressions. So we first parse the list as
           * if it was a list of expressions. If we then find a '=' we have a function assign node and will
           * subsequently have to check whether all parameters are SDFFvariableNode s. If they are we have a
           * valid function definition and can form a list of parameter names from the list
           * of SDFFvariableNodes. To check the type we have to use the C++ RTTI system ...
           */
           std::vector<SDFFnode*> params; // vector that holds all the parameters
           get_token(); // get the next token
           while ((current_token!=sdffTRPARANTHESIS) && (current_token!=sdffTEND)) {
             params.push_back(logical_expression(false));
             // now we expect ',' or ')'
             if ((current_token!=sdffTKOMMA) && (current_token!=sdffTRPARANTHESIS))
               SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.primary()", (asdff_tokentostr(sdffTKOMMA)+" or "+asdff_tokentostr(sdffTRPARANTHESIS)).c_str(), asdff_tokentostr(current_token).c_str(), "function_name( a, b, c )");
             if (current_token==sdffTKOMMA)  get_token(); // swallow ',' only !!! not ')' ...
           }
           if (current_token!=sdffTRPARANTHESIS) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.primary()", asdff_tokentostr(sdffTRPARANTHESIS).c_str(), asdff_tokentostr(current_token).c_str(), "function_name( a, b, c )");
           get_token(); // swallow ')'
           if (current_token==sdffTASSIGN) {// we have a function assign node
             SDFFfunctionassignNode* n;
             if (get_token()==sdffTLBRACE) { // this is function(..) = { ... };
               SDFFlistNode* list=new SDFFlistNode(this);
               expression_seq(true, list);
               if (current_token!=sdffTRBRACE) SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKENP_NUM, "SDFFparser.primary()", asdff_tokentostr(sdffTRBRACE).c_str(), asdff_tokentostr(current_token).c_str(), "function_name( a, b, c ) = {...}");
               get_token(); // swallow '}'
               n=new SDFFfunctionassignNode(this, name, list);
             } else { // this is function(..) = ...;
               n=new SDFFfunctionassignNode(this, name, logical_expression(false));
             }
             // now we do a tye check for SDFFvariableNode for each item in params!
             if (params.size()>0) for (size_t i=0; i<params.size(); i++) {
               if (SDFFvariableNode* vn=dynamic_cast<SDFFvariableNode*>(params[i])) {
                 n->add_parameter(vn->get_varname());
               } else {
                 SEQUENCER_ERRORN(SEQUENCER_SDFFERROR_FDEFNOVARNAME_NUM, "SDFFparser.primary()");
               }
             }
             params.clear();
             res=n;

           } else { // we have a function call
             SDFFfunctionNode* n=new SDFFfunctionNode(this, name);
             if (params.size()>0) for (size_t i=0; i<params.size(); i++) {
               n->add_child(params[i]);
             }
             params.clear();
             res=n;
           }
        } else { // this is just a variable access
          SDFFvariableNode* n=new SDFFvariableNode(this, name);
          res=n;
          // we already got the next token, so we do not have to call get_token() !!!!!!!!
        }

        break;
      }

    default:
      // we did not recognize the current token ... so it is not a primary -> error
      SEQUENCER_ERRORV(SEQUENCER_SDFFERROR_UNEXPECTEDTOKEN_NUM, "SDFFparser.primary()", expect, asdff_tokentostr(current_token).c_str());
  }
  // check the next token for a power operator!
  if (current_token==sdffTPOWER) {
    SDFFarithmeticNode* n=new SDFFarithmeticNode(this, res, primary(true), sdffAOPower);
    res=n;
    //get_token();
  }
  if (current_token==sdffTLOGIC_SHL) {
		SDFFlogicNode* n= new SDFFlogicNode(this, res, primary(true), sdffLOSHL );
		res=n;
  }
  if (current_token==sdffTLOGIC_SHR) {
		SDFFlogicNode* n= new SDFFlogicNode(this, res, primary(true), sdffLOSHR );
		res=n;
  }
  return res;

};









std::string SDFFparser::report_contents(){
  std::string res="";
  res+="-- parameters: --------------------------------------------------------\n";
  res+=parameters->print()+"\n\n";
  res+="-- safe_settings: -----------------------------------------------------\n";
  res+=safe_settings->print()+"\n\n";
  res+="-- sequence: ----------------------------------------------------------\n";
  res+=sequence->print()+"\n\n";

  res+="\n\n";
  res+="-- currently defined functions ----------------------------------------\n";
  res+=fmanager->report()+"\n\n";
  res+="-- currently defined variables ----------------------------------------\n";
  res+=vmanager->report()+"\n\n";

  return res;
};

std::string SDFFparser::get_current_sdff_code(bool params_with_current_setting){
  std::string res="";
  res+="// ================================================================================\n";
  res+="// = automatically generated dump of a loaded SDFF file\n";
  res+="// = time stamp:   "+datetimetostr(time(NULL))+"\n";
  res+="// ================================================================================\n\n";
  res+="parameters {\n";
  res+=parameters->print()+"\n\n";
  if (params_with_current_setting) {
    res+="// --------------------------------------------------------------------------------\n";
    res+="// - this is a dump of all global (non-constant) variables:\n";
    res+="// --------------------------------------------------------------------------------\n";
    unsigned int cnt=vmanager->get_globalcount();
    if (cnt>0) {
      for(unsigned int i=0; i<cnt; i++) {
        sdff_variable v=vmanager->get_globalvardef(i);
        if (! v.constant) {
          switch(v.type) {
            case sdffVTString:  res+="  "+v.name+" = '"+(*(v.stringValue))+"';\n"; break;
            case sdffVTBoolean: res+="  "+v.name+" = "+booltostr(*(v.boolValue))+";\n"; break;
            case sdffVTInteger: res+="  "+v.name+" = "+inttostr(*(v.intValue))+";\n"; break;
            case sdffVTDouble:  res+="  "+v.name+" = "+floattostr(*(v.doubleValue))+";\n"; break;
            case sdffVTVoid:    res+="  "+v.name+" = <void>;\n"; break;
          }
        }
      }
    }
    res+="// --------------------------------------------------------------------------------\n\n\n\n";
  }

  res+="}\n\n\n\n";
  res+="sequence{\n";
  res+=sequence->print()+"\n\n";
  res+="}\n";

  return res;
};


int SDFFparser::generate(double sample_timestep){
    if (debug && log!=NULL) {
      log->log_text("SDFFparser::generate(sample_timestep="+floattostr(sample_timestep)+")\n");
      log->inc_indent();
      log->log_linebreak();
    }
  // clear old sequence elements
  emanager->clear();
  emanager->set_generator(generator);
  emanager->set_parser(this);

  // clear old SDFF commands
  cmd_manager->clear();
  cmd_manager->set_parser(this);

  // set sample_timestep in all objects
  this->sample_timestep=sample_timestep;
  emanager->set_sample_timestep(sample_timestep);
  cmd_manager->set_sample_timestep(sample_timestep);

  // evaluate the sequence ... this will generate elements in emanager
  inside_sequence=true;
  inside_channel=false;
  inside_event=false;
  inside_preevent=false;
  inside_lastevent=false;
  if (pre_event!=NULL) pre_event->evaluate();
  sequence->evaluate();
  set_current_time(emanager->extract_end_time());
  if (debug && log!=NULL) {
    log->log_text("Sextracted_endtime="+floattostr(emanager->extract_end_time())+"\n");
  }
  if (last_event!=NULL) last_event->evaluate();
  //std::cout<<"evaluation complete ... now checking "<<std::endl;

  // check the sequence
  int ret=emanager->check();
  //std::cout<<"checking complete ... now evaluating "<<std::endl;
  int result=-1;
  if (ret>=0) {
    // generate the sequence from the elements in emanager if there were no errors
    emanager->generate();
    if (errornum>0) result=-1;
    else if (warningnum==0) { result=0; }
    else { result=+1; }
  } else result=-1;

  if (debug && log!=NULL) {
    log->log_text("SDFFparser::generate(sample_timestep="+floattostr(sample_timestep)+"): "+inttostr(result));
    log->dec_indent();
    log->log_linebreak();
  }
  return result;
}


int SDFFparser::reset(){
  clear_events();
  generator->clear_used();
  current_time=-1;
  current_line=1;
  current_channel="";
  clear_markers();

  vmanager->clear();
  fmanager->clear();
  add_standard_functions();
  add_standard_constants();


  // evaluate parameters section
  inside_sequence=false;
  try {
    vmanager->set_write_global(true);
    parameters->evaluate();
    vmanager->set_write_global(false);
  } catch (exception E) {
    add_error_p(E.get_number(), E.get_emessage(), E.get_emessage(), current_filename, current_line);
  } catch (std::exception E) {
    add_error_p(-1, E.what(), E.what(), current_filename, current_line);
  }

  //std::cout<<"import end"<<std::endl;
  if (warningnum==0) { return 0; } else { return +1; }
}



};
