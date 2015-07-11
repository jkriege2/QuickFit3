///////////////////////////////////////////////////////////////////////////
//
//  This file is part of the YAID image format reader package
//  Copyright (C) 2011 by Jan Buchholz, Jan Krieger, and Thomas Kuehn
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//
//  Version 0.1
//  This file defines all primitive types used in the YAID format as
//  pixel types. Furthermore here we define an enum structure that
//  determines between those types and is used by the Header class
//  in yaid.hpp. For easier handling of short char* arrays this file
//  contains a small template class, Cstr. Finally, operators to correctly
//  work with output streams can be found in the end
//
///////////////////////////////////////////////////////////////////////////
#ifndef __YAID_TYPES_HPP
#define __YAID_TYPES_HPP

#include <stdint.h>     //standard uint definitions
#include <iostream>

#include "exception.hpp"


//Macro for calling pointer-to-member functions
#define CALL_MEMBER_FUNC(object,ptrToMember)  ((object).*(ptrToMember))

namespace yaid{

  ///////////////////////////////////////////////////////////////////////////
  //
  //  defining the primitive types used as pixel types in the YAID format
  //
  ///////////////////////////////////////////////////////////////////////////

  typedef unsigned char uchar;
  typedef int8_t   int8;
  typedef uint8_t  uint8;
  typedef int16_t  int16;
  typedef uint16_t uint16;
  typedef int32_t  int32;
  typedef uint32_t uint32;
  typedef int64_t  int64;
  typedef uint64_t uint64;

  ///////////////////////////////////////////////////////////////////////////
  //
  //  defining other primitive types
  //
  ///////////////////////////////////////////////////////////////////////////

  typedef unsigned int uint;
  typedef unsigned long int ulint;
  typedef unsigned short int usint;

  ///////////////////////////////////////////////////////////////////////////
  //
  //  defining enum PixelType used to determine between types
  //
  ///////////////////////////////////////////////////////////////////////////

  enum PixelType{
    Tundefined =  0,
    Tbool      =  1,
    Tchar      =  2,
    Tuchar     =  3,
    Tint8      =  4,
    Tuint8     =  5,
    Tint16     =  6,
    Tuint16    =  7,
    Tint32     =  8,
    Tuint32    =  9,
    Tint64     = 10,
    Tuint64    = 11,
    Tfloat     = 12,
    Tdouble    = 13
  };


  ///////////////////////////////////////////////////////////////////////////
  //
  //  here we declare the classes FileWriterBinary, FileReaderBinary, and Cstr
  //
  ///////////////////////////////////////////////////////////////////////////

  template<uint Length> class Cstr;
  class FileWriterBinary;
  class FileReaderBinary;

  ///////////////////////////////////////////////////////////////////////////
  //
  //  ... and the functions that need these declarations.
  //  These two functions are defined in serialization.hpp
  //
  ///////////////////////////////////////////////////////////////////////////

  template<uint Length>
  FileWriterBinary& operator<< (FileWriterBinary&, const Cstr<Length>&);
  template<uint Length>
  FileReaderBinary& operator>> (FileReaderBinary&, Cstr<Length>&);



  ///////////////////////////////////////////////////////////////////////////
  //
  //  defining template class Cstr to deal with small char* objects more 
  //  easily. notice that contrary to normla C-type strings these objects
  //  are missing the usual '\0' character at the end, but thins are being
  //  taken care of when using the defined stream operators.
  //
  ///////////////////////////////////////////////////////////////////////////

  template<uint Length>
  class Cstr{
  private:

    //here we store the data
    char Content[Length];

  public:

    //destructor
    inline ~Cstr(){
    }

    //default constructor, setting all elements to zero
    inline Cstr():Content(){
      for (ulint i(0); i<Length; ++i) Content[i]='0';
    }

    //copying constructor
    inline Cstr(const Cstr& _o):Content(){
      for (ulint i(0); i<Length; ++i) Content[i]=_o.Content[i];
    }

    //copying constructor from char*; does not check lenght of input string!
    inline Cstr(const char* _o):Content(){
      for (ulint i(0); i<Length; ++i) Content[i]=_o[i];
    }

    //copying operator
    inline const Cstr& operator=(const Cstr& _o){
      for (ulint i(0); i<Length; ++i) Content[i]=_o.Content[i]; 
      return *this;
    }

    //copying operator from char*; does not check lenght of input string!
    inline const Cstr& operator=(const char* _o){
      for (ulint i(0); i<Length; ++i) Content[i]=_o[i];
      return *this;
    }

    //index operator with out-of bounds check
    inline char& operator()(const uint& _i){
      if (_i>=Length) throw OutOfBounds("Cstr::operator(): trying to access elements beyond range");
      return Content[_i];
    }

    //index operator with out-of bounds check; const version
    inline const char& operator()(const uint& _i)const{
      if (_i>=Length) throw OutOfBounds("Cstr::operator(): trying to access elements beyond range");
      return Content[_i];
    }

    //index operator without out-of bounds check
    inline char& operator[](const uint& _i){
      return Content[_i];
    }

    //index operator without out-of bounds check; const version
    inline const char& operator[](const uint& _i)const{
      return Content[_i];
    }

    //friend function defining operator to write on std::ostream
    friend std::ostream& operator<<(std::ostream& _o, const Cstr& _out){
      return _o.write(_out.Content,Length);
//       for (uint i(0); i<Length; ++i) _o<<_out.Content[i]; 
//       return _o;
    }

    //these two functions are defined in serialization.hpp
    friend FileWriterBinary& operator<< <>(FileWriterBinary&, const Cstr<Length>&);
    friend FileReaderBinary& operator>> <>(FileReaderBinary&, Cstr<Length>&);
  };//class Cstr


  ///////////////////////////////////////////////////////////////////////////
  //
  //  stream output for enum PixelType
  //
  ///////////////////////////////////////////////////////////////////////////
  inline std::ostream& operator<<(std::ostream& _o, const PixelType& _out){
    if ((&_o)==(&std::cout)){
      switch ((uint16)_out){
      case  0: _o << "undefined"; break;
      case  1: _o << "bool";      break;
      case  2: _o << "char";      break;
      case  3: _o << "uchar";     break;
      case  4: _o << "int8";      break;
      case  5: _o << "uint8";     break;
      case  6: _o << "int16";     break;
      case  7: _o << "uint16";    break;
      case  8: _o << "int32";     break;
      case  9: _o << "uint32";    break;
      case 10: _o << "int64";     break;
      case 11: _o << "uint64";    break;
      case 12: _o << "float";     break;
      case 13: _o << "double";    break;
      default: _o << "use a defined type, bummer!"; break;
      }//switch
    }
    else _o << (uint16)_out;
    return _o;
  }//operator<<()


  inline std::string pixel_type_to_string(const PixelType& _p){
    std::string answer;
    switch ((uint16)_p){
    case  0: answer = "undefined"; break;
    case  1: answer = "bool";      break;
    case  2: answer = "char";      break;
    case  3: answer = "uchar";     break;
    case  4: answer = "int8";      break;
    case  5: answer = "uint8";     break;
    case  6: answer = "int16";     break;
    case  7: answer = "uint16";    break;
    case  8: answer = "int32";     break;
    case  9: answer = "uint32";    break;
    case 10: answer = "int64";     break;
    case 11: answer = "uint64";    break;
    case 12: answer = "float";     break;
    case 13: answer = "double";    break;
    default: answer = "use a defined type, bummer!"; break;
    }//switch
    return answer;
  }//pixel_type_to_string()


  inline PixelType string_to_pixel_type(const std::string& _s){
    if (_s==std::string("undefined")) return Tundefined;
    if (_s==std::string("bool"))       return Tbool;
    if (_s==std::string("char"))       return Tchar;
    if (_s==std::string("uchar"))      return Tuchar;
    if (_s==std::string("int8"))       return Tint8;
    if (_s==std::string("uint8"))      return Tuint8;
    if (_s==std::string("int16"))      return Tint16;
    if (_s==std::string("uint16"))     return Tuint16;
    if (_s==std::string("int32"))      return Tint32;
    if (_s==std::string("uint32"))     return Tuint32;
    if (_s==std::string("int64"))      return Tint64;
    if (_s==std::string("uint64"))     return Tuint64;
    if (_s==std::string("float"))      return Tfloat;
    if (_s==std::string("double"))     return Tdouble;
    throw Exception("string_to_pixel_type(): unknown type");
  }//string_to_pixel_type()


  inline uint16 pixel_type_to_size(const PixelType& _p){
    uint16 answer;
    switch ((uint16)_p){
    case  1: return sizeof(bool);      break;
    case  2: return sizeof(char);      break;
    case  3: return sizeof(uchar);     break;
    case  4: return sizeof(int8);      break;
    case  5: return sizeof(uint8);     break;
    case  6: return sizeof(int16);     break;
    case  7: return sizeof(uint16);    break;
    case  8: return sizeof(int32);     break;
    case  9: return sizeof(uint32);    break;
    case 10: return sizeof(int64);     break;
    case 11: return sizeof(uint64);    break;
    case 12: return sizeof(float);     break;
    case 13: return sizeof(double);    break;
    default: throw Exception("pixel_type_to_length(): unknown type"); break;
    }//switch
    return answer;
  }//pixel_type_to_string()


}//namespace yaid

#endif
