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
//  Here we handle serialization of primitive types, which is an
//  architecture dependent mess. For correctly serialized data,
//  the classes FileWriterBinary and FileReaderBinary should be used.
//  A lot of work still needs to be done on this one. For now it
//  barely works and is by far not optimized for speed.
//
///////////////////////////////////////////////////////////////////////////
#ifndef __YAID_SERIALIZATION_HPP
#define __YAID_SERIALIZATION_HPP

#include <string>
#include <fstream>

#include "types.hpp"
#include "endianness.hpp"
#include "exception.hpp"


namespace yaid{

  //class declarations
  template<uint16> class Converter16;
  template<uint32> class Converter32;
  template<uint64> class Converter64;
  
  class FileWriterBinary;
  class FileReaderBinary;


  ///////////////////////////////////////////////////////////////////////////
  //
  //  defining some inline byte-swap functions that are currently unused
  //
  ///////////////////////////////////////////////////////////////////////////

  inline void endian_swap(int16& _i){
    _i=((_i>>8) |
	(_i<<8));   }

  inline void endian_swap(uint16& _i){
    _i=((_i>>8) |
	(_i<<8));   }

  inline void endian_swap(int32& _i){
    _i=((_i>>24) |
	((_i>>8) & 0x0000FF00) |
	((_i<<8) & 0x00FF0000) |
	(_i<<24));  }

  inline void endian_swap(uint32& _i){
    _i=((_i>>24)|
	((_i>>8) & 0x0000FF00) |
	((_i<<8) & 0x00FF0000) |
	(_i<<24));  }

  inline void endian_swap(int64& _i){
    _i=((_i>>56)|
	((_i>>40) & 0x000000000000FF00ULL) |
	((_i>>24) & 0x0000000000FF0000ULL) |
	((_i>>8)  & 0x00000000FF000000ULL) |
	((_i<<8)  & 0x000000FF00000000ULL) |
	((_i<<24) & 0x0000FF0000000000ULL) |
	((_i<<40) & 0x00FF000000000000ULL) |
	(_i<<56));  }

  inline void endian_swap(uint64& _i){
    _i=((_i>>56)|
	((_i>>40) & 0x000000000000FF00ULL) |
	((_i>>24) & 0x0000000000FF0000ULL) |
	((_i>>8)  & 0x00000000FF000000ULL) |
	((_i<<8)  & 0x000000FF00000000ULL) |
	((_i<<24) & 0x0000FF0000000000ULL) |
	((_i<<40) & 0x00FF000000000000ULL) |
	(_i<<56));  }


  ///////////////////////////////////////////////////////////////////////////
  //
  //  Defining the template classes that will decide on endianness conversion
  //  during compile time
  //
  ///////////////////////////////////////////////////////////////////////////

  template<> 
  class Converter16<0x3412>{
  public:
    ~Converter16(){}

  private:
    inline Converter16(){}
    Converter16(const Converter16&);

    //write
    inline std::ofstream& write(std::ofstream& _out, const uint16& _i)const{_out << _i; return _out;}
    inline std::ofstream& write(std::ofstream& _out, const uint16* _p, const uint& _n)const{
      _out.write((char*)_p,_n*sizeof(uint16));
      return _out;
    }

    //read
    inline std::ifstream& read(std::ifstream& _in, uint16& _i)const{_in.read((char*)(&_i),sizeof(uint16)); return _in;}
    inline std::ifstream& read(std::ifstream& _in, const uint16* _p, const uint& _n)const{
      _in.read((char*)_p,_n*sizeof(uint16));
      return _in;
    }

  public:
    friend class FileWriterBinary;
    friend class FileReaderBinary;
  };


  template<> 
  class Converter16<0x1234>{
  public:
    ~Converter16(){}

  private:
    inline Converter16(){}
    Converter16(const Converter16&);

    //write
    inline std::ofstream& write(std::ofstream& _out, const uint16& _i)const{
      register uint16 tmp(_i); 
      endian_swap(tmp); 
      _out << tmp; 
      return _out;
    }
    std::ofstream& write(std::ofstream& _out, const uint16* _p, const uint& _n)const{
      for (uint i(0); i<_n; ++i){
	uint16 tmp(_p[i]); 
	endian_swap(tmp);
	_out.write((char*)(&tmp),sizeof(uint16));
      }//for(i)
      return _out;
    }//write(_out,_p,_n)

    //read
    inline std::ifstream& read(std::ifstream& _in, uint16 _i)const{_in >> _i; endian_swap(_i); return _in;}
    std::ifstream& read(std::ifstream& _in, uint16* _p, const uint& _n)const{
      _in.read((char*)_p,_n*sizeof(uint16));
      for (uint i(0); i<_n; ++i){
	endian_swap(*(_p+i));
      }//for(i)
      return _in;
    }//read(_in,_p,_n)

  public:
    friend class FileWriterBinary;
    friend class FileReaderBinary;
  };

  
  template<> 
  class Converter32<0x78563412>{
  public: 
    ~Converter32(){}

  private:
    inline Converter32(){}
    Converter32(const Converter32&);

    //write
    inline std::ofstream& write(std::ofstream& _out, const uint32& _i)const{_out << _i; return _out;}
    inline std::ofstream& write(std::ofstream& _out, const uint32* _p, const uint& _n)const{
      _out.write((char*)_p,_n*sizeof(uint32));
      return _out;
    }

    //read
    inline std::ifstream& read(std::ifstream& _in, uint32& _i)const{_in.read((char*)(&_i),sizeof(uint32)); return _in;}
    inline std::ifstream& read(std::ifstream& _in, const uint32* _p, const uint& _n)const{
      _in.read((char*)_p,_n*sizeof(uint32));
      return _in;
    }

  public:
    friend class FileWriterBinary;
    friend class FileReaderBinary;
  };


  template<> class Converter32<0x12345678>{
    inline Converter32(){}
    Converter32(const Converter32&);

    //write
    inline std::ofstream& write(std::ofstream& _out, const uint32& _i)const{
      register uint32 tmp(_i); 
      endian_swap(tmp); 
      _out << tmp;
      return _out;
    }
    std::ofstream& write(std::ofstream& _out, const uint32* _p, const uint& _n)const{
      for (uint i(0); i<_n; ++i){
	uint32 tmp(_p[i]); 
	endian_swap(tmp);
	_out.write((char*)(&tmp),sizeof(uint32));
      }//for(i)
      return _out;
    }//write(_out,_p,_n)

    //read
    inline std::ifstream& read(std::ifstream& _in, uint32 _i)const{_in >> _i; endian_swap(_i); return _in;}
    std::ifstream& read(std::ifstream& _in, uint32* _p, const uint& _n)const{
      _in.read((char*)_p,_n*sizeof(uint32));
      for (uint i(0); i<_n; ++i){
	endian_swap(*(_p+i));
      }//for(i)
      return _in;
    }//read(_in,_p,_n)

  public:
    friend class FileWriterBinary;
    friend class FileReaderBinary;
  };
  

  template<> 
  class Converter64<0xefcdab8967452301LLU>{
  public:
    ~Converter64(){}

  private:
    inline Converter64(){}
    Converter64(const Converter64&);

    //write
    inline std::ofstream& write(std::ofstream& _out, const uint64& _i)const{_out << _i; return _out;}
    inline std::ofstream& write(std::ofstream& _out, const uint64* _p, const uint& _n)const{
      _out.write((char*)_p,_n*sizeof(uint64));
      return _out;
    }

    //read
    inline std::ifstream& read(std::ifstream& _in, uint64& _i)const{_in.read((char*)(&_i),sizeof(uint64)); return _in;}
    inline std::ifstream& read(std::ifstream& _in, const uint64* _p, const uint& _n)const{
      _in.read((char*)_p,_n*sizeof(uint64));
      return _in;
    }

  public:
    friend class FileWriterBinary;
    friend class FileReaderBinary;
  };


  template<> 
  class Converter64<0x123456789abcdefLLU>{
  public:
    ~Converter64(){}

  private:
    inline Converter64(){}
    Converter64(const Converter64&);

    //write
    inline std::ofstream& write(std::ofstream& _out, const uint64& _i)const{
      register uint64 tmp(_i); 
      endian_swap(tmp); 
      _out << tmp;
      return _out;
    }
    std::ofstream& write(std::ofstream& _out, const uint64* _p, const uint& _n)const{
      for (uint i(0); i<_n; ++i){
	uint64 tmp(_p[i]);
	endian_swap(tmp);
	_out.write((char*)(&tmp),sizeof(uint64));
      }//for(i)
      return _out;
    }//write(_out,_p,_n)

    //read
    inline std::ifstream& read(std::ifstream& _in, uint64 _i)const{_in >> _i; endian_swap(_i); return _in;}
    std::ifstream& read(std::ifstream& _in, uint64* _p, const uint& _n)const{
      _in.read((char*)_p,_n*sizeof(uint64));
      for (uint i(0); i<_n; ++i){
	endian_swap(*(_p+i));
      }//for(i)
      return _in;
    }//read(_in,_p,_n)

  public:
    friend class FileWriterBinary;
    friend class FileReaderBinary;
  };

  ///////////////////////////////////////////////////////////////////////////
  //
  //  class FileWriterBinary
  //  Wraps an std::ofstream opened in binary mode, taking care that all
  //  primitive types are serialized correctly. Usage should be corresponding
  //  to std::ofstream usage.
  //
  ///////////////////////////////////////////////////////////////////////////

  class FileWriterBinary{
  private:
    //the stream
    std::ofstream Stream;


//     //write-function pointers:
//     write_int16 WriteInt16;

  public:
    //destructor
    ~FileWriterBinary();

    //default constructor, leaving Stream un-opened
    FileWriterBinary();

    //constructor attempting to open Stream to file named in passed string
    FileWriterBinary(const std::string&);

  private:
    //copy constructor not implemented
    FileWriterBinary(const FileWriterBinary&);

  public:
    //attempts to open stream to file named in passed string
    void open(const std::string&);

    //closes currently open stream
    void close();

    //returns true if Stream is open
    bool is_open()const{return Stream.is_open();}

    //writing _n characters from addres _p into Stream
    inline FileWriterBinary& write(const char* _p, ulint _n){
      Stream.write(_p, _n);
      return *this;
    }

    //writing _n unsigned characters from addres _p into Stream
    inline FileWriterBinary& write(const uchar* _p, ulint _n){
      Stream.write((char*)_p, _n);
      return *this;
    }

    //writing _n int16 from address _p into Stream
    inline FileWriterBinary& write(const int16* _p, ulint _n){
      static const Converter16<YAID_ENDIANNESS_16> Converter;
      Converter.write(Stream,(uint16*)_p,_n);
      return *this;
    }

    //writing _n uint16 from address _p into Stream
    inline FileWriterBinary& write(const uint16* _p, ulint _n){
      static const Converter16<YAID_ENDIANNESS_16> Converter;
      Converter.write(Stream,_p,_n);
      return *this;
    }

    //writing _n int32 from address _p into Stream
    inline FileWriterBinary& write(const int32* _p, ulint _n){
      static const Converter32<YAID_ENDIANNESS_32> Converter;
      Converter.write(Stream,(uint32*)_p,_n);
      return *this;
    }

    //writing _n uint32 from address _p into Stream
    inline FileWriterBinary& write(const uint32* _p, ulint _n){
      static const Converter32<YAID_ENDIANNESS_32> Converter;
      Converter.write(Stream,_p,_n);
      return *this;
    }

    //writing _n int64 from address _p into Stream
    inline FileWriterBinary& write(const int64* _p, ulint _n){
      static const Converter64<YAID_ENDIANNESS_64> Converter;
      Converter.write(Stream,(uint64*)_p,_n);
      return *this;
    }

    //writing _n uint64 from address _p into Stream
    inline FileWriterBinary& write(const uint64* _p, ulint _n){
      static const Converter64<YAID_ENDIANNESS_64> Converter;
      Converter.write(Stream,_p,_n);
      return *this;
    }

    //setting Stream position to absolute (from beginning of stream) position _p
    inline FileWriterBinary& seekp(std::streampos _p){
      Stream.seekp(_p);
      return *this;
    }

    //setting Stream position to relative position (relative to beginning: std::ios_base::beg,
    //end:std::ios_base::end or current: std::ios_base::cur position) _o.
    //not quite clear yet if _o can take negative values
    inline FileWriterBinary& seekp(std::streamoff _o, std::ios_base::seekdir _d){
      Stream.seekp(_o,_d);
      return *this;
    }

    //checking the status of the stream. If the return value is true, all is fine
    inline operator bool(){
      return (bool)Stream;
    }

    //Here we define all the necessary operators to write the primitive types
    //correctly into FileWriterBinary
    friend inline FileWriterBinary& operator<<(FileWriterBinary& _out, const char&   _i){return _out.write(&_i,1);}
    friend inline FileWriterBinary& operator<<(FileWriterBinary& _out, const uchar&  _i){return _out.write(&_i,1);}
    friend inline FileWriterBinary& operator<<(FileWriterBinary& _out, const int8&   _i){return _out.write((const char*)&_i,1);}
//     friend FileWriterBinary& operator<<(FileWriterBinary&, const uint8&);
    friend inline FileWriterBinary& operator<<(FileWriterBinary& _out, const int16&  _i){return _out.write(&_i,1);}
    friend inline FileWriterBinary& operator<<(FileWriterBinary& _out, const uint16& _i){return _out.write(&_i,1);}
    friend inline FileWriterBinary& operator<<(FileWriterBinary& _out, const int32&  _i){return _out.write(&_i,1);}
    friend inline FileWriterBinary& operator<<(FileWriterBinary& _out, const uint32& _i){return _out.write(&_i,1);}
    friend inline FileWriterBinary& operator<<(FileWriterBinary& _out, const int64&  _i){return _out.write(&_i,1);}
    friend inline FileWriterBinary& operator<<(FileWriterBinary& _out, const uint64& _i){return _out.write(&_i,1);}

    //Here the friend function that writes out Cstr objects
    template<uint Length>
    inline friend FileWriterBinary& operator<<(FileWriterBinary& _o, const Cstr<Length>& _out){
      _o.Stream.write(_out.Content,Length);
      return _o;
    }

  };//class FileWriterBinary




  ///////////////////////////////////////////////////////////////////////////
  //
  //  class FileReaderBinary
  //  Wraps an std::ifstream opened in binary mode, taking care that all
  //  primitive types are serialized correctly. Usage should be corresponding
  //  to std::ifstream usage.
  //
  ///////////////////////////////////////////////////////////////////////////

  class FileReaderBinary{
  private:
    //the stream
    std::ifstream Stream;

  public:
    //destructor
    ~FileReaderBinary();

    //default constructor, leaving Stream un-opened
    FileReaderBinary();

    //constructor attempting to open Stream to file named in passed string
    FileReaderBinary(const std::string&);

  private:
    //copy constructor not implemented
    FileReaderBinary(const FileReaderBinary&);

  public:
    //attempts to open stream to file named in passed string
    void open(const std::string&);

    //closes currently open stream
    void close();

    //returns true if Stream is open
    bool is_open()const{return Stream.is_open();}

    //reading _n characters from Stream into addres _p
    inline FileReaderBinary& read(char* _p, ulint _s){
      Stream.read(_p, _s);
      return *this;
    }

    //reading _n unsigned characters from addres _p into Stream
    inline FileReaderBinary& read(const uchar* _p, ulint _n){
      Stream.read((char*)_p, _n);
      return *this;
    }

    //reading _n int16 from Stream into address _p
    inline FileReaderBinary& read(const int16* _p, ulint _n){
      static const Converter16<YAID_ENDIANNESS_16> Converter;
      Converter.read(Stream,(uint16*)_p,_n);
      return *this;
    }

    //reading _n uint16 from Stream into address _p
    inline FileReaderBinary& read(const uint16* _p, ulint _n){
      static const Converter16<YAID_ENDIANNESS_16> Converter;
      Converter.read(Stream,_p,_n);
      return *this;
    }

    //reading _n int32 from Stream into address _p
    inline FileReaderBinary& read(const int32* _p, ulint _n){
      static const Converter32<YAID_ENDIANNESS_32> Converter;
      Converter.read(Stream,(uint32*)_p,_n);
      return *this;
    }

    //reading _n uint32 from Stream into address _p
    inline FileReaderBinary& read(const uint32* _p, ulint _n){
      static const Converter32<YAID_ENDIANNESS_32> Converter;
      Converter.read(Stream,_p,_n);
      return *this;
    }

    //reading _n int64 from Stream into address _p
    inline FileReaderBinary& read(const int64* _p, ulint _n){
      static const Converter64<YAID_ENDIANNESS_64> Converter;
      Converter.read(Stream,(uint64*)_p,_n);
      return *this;
    }

    //reading _n uint64 from Stream into address _p
    inline FileReaderBinary& read(const uint64* _p, ulint _n){
      static const Converter64<YAID_ENDIANNESS_64> Converter;
      Converter.read(Stream,_p,_n);
      return *this;
    }

    //setting Stream position to absolute (from beginning of stream) position _p
    inline FileReaderBinary& seekg(std::streampos _p){
      Stream.seekg(_p);
      return *this;
    }

    //setting Stream position to relative position (relative to beginning: std::ios_base::beg,
    //end:std::ios_base::end or current: std::ios_base::cur position) _o.
    //not quite clear yet if _o can take negative values
    inline FileReaderBinary& seekg(std::streamoff _o, std::ios_base::seekdir _d){
      Stream.seekg(_o,_d);
      return *this;
    }

    //checking the status of the stream. If the return value is true, all is fine
    inline operator bool(){
      return (bool)Stream;
    }

    //Here we define all the necessary operators to read the primitive types
    //correctly out of FileReaderBinary
    friend inline FileReaderBinary& operator>>(FileReaderBinary& _in, char&   _i){return _in.read(&_i,1);}
    friend inline FileReaderBinary& operator>>(FileReaderBinary& _in, uchar&  _i){return _in.read(&_i,1);}
    friend inline FileReaderBinary& operator>>(FileReaderBinary& _in, int8&   _i){return _in.read((char*)&_i,1);}
//     friend FileReaderBinary& operator>>(FileReaderBinary&, const uint8&);
    friend inline FileReaderBinary& operator>>(FileReaderBinary& _in, int16&  _i){return _in.read(&_i,1);}
    friend inline FileReaderBinary& operator>>(FileReaderBinary& _in, uint16& _i){return _in.read(&_i,1);}
    friend inline FileReaderBinary& operator>>(FileReaderBinary& _in, int32&  _i){return _in.read(&_i,1);}
    friend inline FileReaderBinary& operator>>(FileReaderBinary& _in, uint32& _i){return _in.read(&_i,1);}
    friend inline FileReaderBinary& operator>>(FileReaderBinary& _in, int64&  _i){return _in.read(&_i,1);}
    friend inline FileReaderBinary& operator>>(FileReaderBinary& _in, uint64& _i){return _in.read(&_i,1);}

    //Here the friend function that reads in Cstr objects
    template<uint Length>
    inline friend FileReaderBinary& operator>>(FileReaderBinary& _i, Cstr<Length>& _out){
      _i.Stream.read(_out.Content,Length);
      return _i;
    }
  };//class FileReaderBinary


}//namespace yaid
#endif
