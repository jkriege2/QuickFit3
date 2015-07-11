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
//  All exceptions used in the reader tools should be defined here and
//  should be derived from the original Exception class.
//  The syntax should be straight forward.
//  
///////////////////////////////////////////////////////////////////////////
#ifndef __YAID_EXCEPTION_HPP
#define __YAID_EXCEPTION_HPP

#include <iostream>

namespace yaid{
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //EXCEPTION
  //exception base class
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class Exception{
  protected:
    std::string Message;
  public:
    inline Exception():Message("unknown error"){}
    inline Exception(const std::string& _message):Message(_message){}
    inline Exception(const Exception& _orig):Message(_orig.Message){}
    inline ~Exception(){}

    inline friend std::ostream& operator<<(std::ostream& _o, const Exception& _e){return _o << _e.Message;}
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //OUTOFBOUNDS
  //should be thrown by all array types when acces to out-of-bounds elements is attempted
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class OutOfBounds: public Exception{
  public:
    inline OutOfBounds():Exception("out of bounds"){}
    inline OutOfBounds(const std::string& _message):Exception(_message){}
    inline OutOfBounds(const OutOfBounds& _orig):Exception(_orig){}
    inline ~OutOfBounds(){}

    inline friend std::ostream& operator<<(std::ostream& _o, const OutOfBounds& _e){return _o << (Exception)_e;}
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //FILEBUSY
  //used by file handling routines when file operations fail
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class FileBusy: public Exception{
  public:
    inline FileBusy():Exception("file busy"){}
    inline FileBusy(const std::string& _message):Exception(_message){}
    inline FileBusy(const FileBusy& _orig):Exception(_orig){}
    inline ~FileBusy(){}

    inline friend std::ostream& operator<<(std::ostream& _o, const FileBusy& _e){return _o << (Exception)_e;}
  };


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //DIMENSIONMISMATCH
  //used by dimension handling routines when dimension operations fail
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class DimensionMismatch: public Exception{
  public:
    inline DimensionMismatch():Exception("dimension mismatch"){}
    inline DimensionMismatch(const std::string& _message):Exception(_message){}
    inline DimensionMismatch(const DimensionMismatch& _orig):Exception(_orig){}
    inline ~DimensionMismatch(){}

    inline friend std::ostream& operator<<(std::ostream& _o, const DimensionMismatch& _e){return _o << (Exception)_e;}
  };

}//namespace yaid
#endif
