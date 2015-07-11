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
//
///////////////////////////////////////////////////////////////////////////

#include "serialization.hpp"


namespace yaid{

  ///////////////////////////////////////////////////////////////////////////
  //
  //  class FileWriterBinary
  //  Wraps an std::ofstream opened in binary mode, taking care that all
  //  primitive types are serialized correctly. Usage should be corresponding
  //  to std::ostream usage.
  //
  ///////////////////////////////////////////////////////////////////////////


  FileWriterBinary::~FileWriterBinary()
  {
    close();
  }//destructor


  FileWriterBinary::FileWriterBinary():Stream()
  {
  }//default constructor


  FileWriterBinary::FileWriterBinary(const std::string& _filename):Stream()
  {
    open(_filename);
  }//constructor(string)


  void FileWriterBinary::open(const std::string& _filename)
  {
    if (Stream.is_open()) throw Exception("FileWriterBinary::open(): already open");
    Stream.open(_filename.c_str(), std::ios::out | std::ios::binary);
    if (!Stream.is_open()) throw FileBusy("FileWriterBinary::open(): could not open file \"" + _filename + "\"");
  }//open()


  void FileWriterBinary::close()
  {
    if (Stream.is_open()) Stream.close();
  }//close()


//   FileWriterBinary& operator<<(FileWriterBinary& _o, const char& _out)
//   {
//     _o.Stream.write(&_out,1);
//     return _o;
//   }


//   FileWriterBinary& operator<<(FileWriterBinary& _o, const uchar& _out)
//   {
//     _o.Stream.write((char*)(&_out),1);
//     return _o;
//   }


//   FileWriterBinary& operator<<(FileWriterBinary& _o, const int8& _out)
//   {
//     _o.Stream.write((char*)(&_out),1);
//     return _o;
//   }


//   FileWriterBinary& operator<<(FileWriterBinary& _o, const int16& _out)
//   {
// //     std::cout << "writing int16 to FileWriterBinary" << std::endl;
//     static const union {uchar bytes[2]; int16 value;} int16_host_tester={{0,1}};
//     static const Cstr<2> int16_host_order(int16_host_tester.bytes[0]==0 ? "\0\1" : "\1\0");
//     for (uint8 i(0); i<2; ++i) _o.Stream.write(((char*)(&_out))+(int16_host_order[i]),1);
//     return _o;
//   }


//   FileWriterBinary& operator<<(FileWriterBinary& _o, const uint16& _out)
//   {
// //     std::cout << "writing uint16 to FileWriterBinary" << std::endl;
//     static const union {uchar bytes[2]; uint16 value;} uint16_host_tester={{0,1}};
//     static const Cstr<2> uint16_host_order(uint16_host_tester.bytes[0]==0 ? "\0\1" : "\1\0");
//     for (uint8 i(0); i<2; ++i) _o.Stream.write(((char*)(&_out))+(uint16_host_order[i]),1);
//     return _o;
//   }


//   FileWriterBinary& operator<<(FileWriterBinary& _o, const int32& _out)
//   {
// //     std::cout << "writing int32 to FileWriterBinary" << std::endl;
//     //not quite right yet!!!
//     static const union {uchar bytes[4]; int32 value;} int32_host_tester={{0,1,2,3}};
//     static const Cstr<4> int32_host_order(int32_host_tester.bytes[0]==0 ? "\0\1\2\3" : "\3\2\1\0");
//     for (uint8 i(0); i<4; ++i) _o.Stream.write(((char*)(&_out))+(int32_host_order[i]),1);
//     return _o;
//   }


//   FileWriterBinary& operator<<(FileWriterBinary& _o, const uint32& _out)
//   {
// //     std::cout << "writing uint32 to FileWriterBinary" << std::endl;
//     //not quite right yet!!!
//     static const union {uchar bytes[4]; uint32 value;} uint32_host_tester={{0,1,2,3}};
//     static const Cstr<4> uint32_host_order(uint32_host_tester.bytes[0]==0 ? "\0\1\2\3" : "\3\2\1\0");
//     for (uint8 i(0); i<4; ++i) _o.Stream.write(((char*)(&_out))+(uint32_host_order[i]),1);
//     return _o;
//   }


//   FileWriterBinary& operator<<(FileWriterBinary& _o, const int64& _out)
//   {
// //     std::cout << "writing int64 to FileWriterBinary" << std::endl;
//     //not quite right yet!!!
//     static const union {uchar bytes[8]; int64 value;} int64_host_tester={{0,1,2,3}};
//     static const Cstr<8> int64_host_order(int64_host_tester.bytes[0]==0 ? "\0\1\2\3\4\5\6\7" : "\7\6\5\4\3\2\1\0");
//     for (uint8 i(0); i<8; ++i) _o.Stream.write(((char*)(&_out))+(int64_host_order[i]),1);
//     return _o;
//   }


//   FileWriterBinary& operator<<(FileWriterBinary& _o, const uint64& _out)
//   {
// //     std::cout << "writing uint64 to FileWriterBinary" << std::endl;
//     //not quite right yet!!!
//     static const union {uchar bytes[8]; uint64 value;} uint64_host_tester={{0,1,2,3}};
//     static const Cstr<8> uint64_host_order(uint64_host_tester.bytes[0]==0 ? "\0\1\2\3\4\5\6\7" : "\7\6\5\4\3\2\1\0");
//     for (uint8 i(0); i<8; ++i) _o.Stream.write(((char*)(&_out))+(uint64_host_order[i]),1);
//     return _o;
//   }













  ///////////////////////////////////////////////////////////////////////////
  //
  //  class FileReaderBinary
  //  Wraps an std::ifstream opened in binary mode, taking care that all
  //  primitive types are serialized correctly. Usage should be corresponding
  //  to std::istream usage.
  //
  ///////////////////////////////////////////////////////////////////////////

  FileReaderBinary::~FileReaderBinary()
  {
    close();
  }//destructor


  FileReaderBinary::FileReaderBinary():Stream()
  {
  }//default constructor


  FileReaderBinary::FileReaderBinary(const std::string& _filename):Stream()
  {
    open(_filename);
  }//constructor(string)


  void FileReaderBinary::open(const std::string& _filename)
  {
    if (Stream.is_open()) throw Exception("FileReaderBinary::open(): already open");
    Stream.open(_filename.c_str(), std::ios::out | std::ios::binary);
    if (!Stream.is_open()) throw FileBusy("FileReaderBinary::open(): could not open file \"" + _filename + "\"");
  }//open()


  void FileReaderBinary::close()
  {
    if (Stream.is_open()) Stream.close();
  }//close()


//   FileReaderBinary& operator>>(FileReaderBinary& _i, char& _in)
//   {
//     _i.Stream.read(&_in,1);
//     return _i;
//   }


//   FileReaderBinary& operator>>(FileReaderBinary& _i, uchar& _in)
//   {
//     _i.Stream.read((char*)(&_in),1);
//     return _i;
//   }


//   FileReaderBinary& operator>>(FileReaderBinary& _i, int8& _in)
//   {
//     _i.Stream.read((char*)(&_in),1);
//     return _i;
//   }


//   FileReaderBinary& operator>>(FileReaderBinary& _i, int16& _in)
//   {
//     static union {uchar bytes[2]; int16 value;} int16_host_tester={{0,1}};
//     static const Cstr<2> int16_host_irder(int16_host_tester.bytes[0]==0 ? "\0\1" : "\1\0");
//     for (uint8 i(0); i<2; ++i) _i.Stream.read(((char*)(&_in))+(int16_host_irder[i]),1);
//     return _i;
//   }


//   FileReaderBinary& operator>>(FileReaderBinary& _i, uint16& _in)
//   {
//     static union {uchar bytes[2]; uint16 value;} uint16_host_tester={{0,1}};
//     static const Cstr<2> uint16_host_irder(uint16_host_tester.bytes[0]==0 ? "\0\1" : "\1\0");
//     for (uint8 i(0); i<2; ++i) _i.Stream.read(((char*)(&_in))+(uint16_host_irder[i]),1);
//     return _i;
//   }


//   FileReaderBinary& operator>>(FileReaderBinary& _i, int32& _in)
//   {
//     //not quite right yet!!!
//     static union {uchar bytes[4]; int32 value;} int32_host_tester={{0,1,2,3}};
//     static const Cstr<4> int32_host_irder(int32_host_tester.bytes[0]==0 ? "\0\1\2\3" : "\3\2\1\0");
//     for (uint8 i(0); i<4; ++i) _i.Stream.read(((char*)(&_in))+(int32_host_irder[i]),1);
//     return _i;
//   }


//   FileReaderBinary& operator>>(FileReaderBinary& _i, uint32& _in)
//   {
//     //not quite right yet!!!
//     static union {uchar bytes[4]; uint32 value;} uint32_host_tester={{0,1,2,3}};
//     static const Cstr<4> uint32_host_irder(uint32_host_tester.bytes[0]==0 ? "\0\1\2\3" : "\3\2\1\0");
//     for (uint8 i(0); i<4; ++i) _i.Stream.read(((char*)(&_in))+(uint32_host_irder[i]),1);
//     return _i;
//   }


//   FileReaderBinary& operator>>(FileReaderBinary& _i, int64& _in)
//   {
//     //not quite right yet!!!
//     static union {uchar bytes[8]; int64 value;} int64_host_tester={{0,1,2,3}};
//     static const Cstr<8> int64_host_irder(int64_host_tester.bytes[0]==0 ? "\0\1\2\3\4\5\6\7" : "\7\6\5\4\3\2\1\0");
//     for (uint8 i(0); i<8; ++i) _i.Stream.read(((char*)(&_in))+(int64_host_irder[i]),1);
//     return _i;
//   }


//   FileReaderBinary& operator>>(FileReaderBinary& _i, uint64& _in)
//   {
//     //not quite right yet!!!
//     static union {uchar bytes[8]; uint64 value;} uint64_host_tester={{0,1,2,3}};
//     static const Cstr<8> uint64_host_irder(uint64_host_tester.bytes[0]==0 ? "\0\1\2\3\4\5\6\7" : "\7\6\5\4\3\2\1\0");
//     for (uint8 i(0); i<8; ++i) _i.Stream.read(((char*)(&_in))+(uint64_host_irder[i]),1);
//     return _i;
//   }


}//namespace yaid
