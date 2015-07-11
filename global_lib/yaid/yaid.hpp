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
#ifndef __YAID_HPP
#define __YAID_HPP

//Version 0.1
//Basic Header in-output
//raw data reading and writing

//Version 0.2
//Proper conversion between native and big endianness
//pixel manipulation in new Slice class

//the usial includes:
#include <iostream>
#include <string>

//own includes:
#include "types.hpp"                 //defining all used types
#include "exception.hpp"             //for better flow control
#include "serialization.hpp"         //serialization of primitive types

namespace yaid{

  //loan functions:
  using std::ostream;

  //for holding the data:
  class Header;
  class Tag;
  class Slice;

  //for handling the files:
  class YaidReader;
  class YaidWriter;
  class YaidAppender;


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //class Header
  //defines the Format of the Header Part
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class Header{
  private:
    Cstr<8>   FileID;              //file id (4 bytes) and version number (4 bytes)
    uint16    Width;               //width of the slice
    uint16    Height;              //height of the slice
    uint16    Depth;               //depth of the slice
    uint16    Channels;            //amount of channels
    uint16    Type;                //the data type used for the pixel value
    Cstr<10>  Reserved;            //in case we forgot something
    uint32    Offset;              //offset from header to first slice in bytes

  public:
    inline ~Header(){}

  private:
    //default Header with default values for all 
    inline Header():
      FileID("YAIDV0.2"),Width(0),Height(0),Depth(1),Channels(1),
      Type((uint16)Tuchar),Reserved("!Reserved!"),Offset(1024){
    }
    inline Header(const Header& _o):
      FileID(_o.FileID),Width(_o.Width),Height(_o.Height),Depth(_o.Depth),Channels(_o.Channels),
      Type(_o.Type),Reserved(_o.Reserved),Offset(_o.Offset){
    }
    friend inline ostream& operator<<(ostream& _o,const Header& _h){
      if ((&_o)==(&std::cout)){
	_o <<   "FileID   = " << _h.FileID
	   << "\nWidth    = " << _h.Width
	   << "\nHeight   = " << _h.Height
	   << "\nDepth    = " << _h.Depth
	   << "\nChannels = " << _h.Channels
	   << "\nType     = " << (PixelType)_h.Type
	   << "\nOffset   = " << _h.Offset << std::flush;
      }
      else { 
	_o << _h.FileID          << '\n'
	   << _h.Width           << '\n'
	   << _h.Height          << '\n'
	   << _h.Depth           << '\n'
	   << _h.Channels        << '\n'
	   << (PixelType)_h.Type << '\n'
	   << _h.Offset;
      }
      return _o;
    }//operator<<(ostream,Header)


    friend inline FileWriterBinary& operator<<(FileWriterBinary& _o, const Header& _h){
      _o << _h.FileID;
      _o << _h.Width;
      _o << _h.Height;
      _o << _h.Depth;
      _o << _h.Channels;
      _o << _h.Type;
      _o << _h.Reserved;
      _o << _h.Offset;
      return _o;
    }//operator<<(FileWriterBinary)


    friend inline FileReaderBinary& operator>>(FileReaderBinary& _i, Header& _h){
      _i >> _h.FileID;
      _i >> _h.Width;
      _i >> _h.Height;
      _i >> _h.Depth;
      _i >> _h.Channels;
      _i >> _h.Type;
      _i >> _h.Reserved;
      _i >> _h.Offset;
      return _i;
    }//operator>>(FileReaderBinary)

    //Friends:
    friend class YaidWriter;
    friend class YaidReader;
    friend class YaidAppender;
    friend class Slice;
  };//class Header


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //class Tag
  //defines the Format of the Tags
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class Tag{
  };//class Tag

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //class Slice
  //defines the Format of the Slices
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  class Slice{
  private:
    typedef void (Slice::*writer_func)(FileWriterBinary& _writer)const;
    typedef void (Slice::*reader_func)(FileReaderBinary& _reader)const;
    typedef char* (Slice::*reference) (const uint16&, const uint16&, const uint16&, const uint16&)const;

  private:
    uint16 Width;
    uint16 Height;
    uint16 Depth;
    uint16 Channels;
    uint16 Type;

    uint64 DataSize;
    uint64 MemSize;

    char*  Data;
    writer_func WriteFunc;
    reader_func ReadFunc;
    reference   PointerFunc;

  public:
    ~Slice();
    Slice();
    Slice(const Slice&);
    Slice(const Header&);

  private:
    //memory handling:
    void allocate();
    void free();
    void set_pointers();

  public:
    //information
    inline const uint16&    width()const{return Width;}
    inline const uint16&   height()const{return Height;}
    inline const uint16&    depth()const{return Depth;}
    inline const uint16& channels()const{return Channels;}
    inline const PixelType   type()const{return (PixelType)Type;}

    //setting of parameters
    void set_parameters (const Header&);

    //for direct data field manipulation -- handle with care!
    inline char* const& data()const{return Data;}


    //writing pixels:
    template<typename InputType>
    void write_pixel(const uint16& _x, const uint16& _y, const uint16& _z, const uint16& _c, const InputType& _p){
//       uint16 pos( (_x+Width*(_y+Height*(_z+Depth*_c))) );
//       if (pos*sizeof(InputType)>DataSize) throw OutOfBounds("Slice::write_pixel");
//       ((InputType*)(Data))[pos]=_p;
      char* ptr(pointer(_x,_y,_z,_c));
      if (((uint64)(ptr-Data))>DataSize) throw OutOfBounds("Slice::write_pixel");
      (*((InputType*)(ptr)))=_p;
    }


    //reading pixels:
    template<typename InputType>
    void read_pixel(const uint16& _x, const uint16& _y, const uint16& _z, const uint16& _c, InputType& _p){
//       uint16 pos( (_x+Width*(_y+Height*(_z+Depth*_c))) );
//       if (pos*sizeof(InputType)>DataSize) throw OutOfBounds("Slice::read_pixel");
//       _p=((InputType*)(Data))[pos];
      char* ptr(pointer(_x,_y,_z,_c));
      if (((uint64)(ptr-Data))>DataSize) throw OutOfBounds("Slice::write_pixel");
      _p=(*((InputType*)(ptr)));
    }


  private:
    inline void write   (FileWriterBinary& _writer)const{CALL_MEMBER_FUNC(*this,WriteFunc)(_writer);}
    inline void write_8 (FileWriterBinary& _writer)const{_writer.write((char*)Data,DataSize/sizeof(uint8));}
    inline void write_16(FileWriterBinary& _writer)const{_writer.write((uint16*)Data,DataSize/sizeof(uint16));}
    inline void write_32(FileWriterBinary& _writer)const{_writer.write((uint32*)Data,DataSize/sizeof(uint32));}
    inline void write_64(FileWriterBinary& _writer)const{_writer.write((uint64*)Data,DataSize/sizeof(uint64));}

    inline void read   (FileReaderBinary& _reader)const{CALL_MEMBER_FUNC(*this,ReadFunc)(_reader);}
    inline void read_8 (FileReaderBinary& _reader)const{_reader.read((char*)Data,DataSize/sizeof(uint8));}
    inline void read_16(FileReaderBinary& _reader)const{_reader.read((uint16*)Data,DataSize/sizeof(uint16));}
    inline void read_32(FileReaderBinary& _reader)const{_reader.read((uint32*)Data,DataSize/sizeof(uint32));}
    inline void read_64(FileReaderBinary& _reader)const{_reader.read((uint64*)Data,DataSize/sizeof(uint64));}

    inline char* pointer   (const uint16& _x, const uint16& _y, const uint16& _z, const uint16& _c)const{return CALL_MEMBER_FUNC(*this,PointerFunc)(_x,_y,_z,_c);}
    inline char* pointer_8 (const uint16& _x, const uint16& _y, const uint16& _z, const uint16& _c)const{return Data+(_x+Width*(_y+Height*(_z+Depth*_c)));}
    inline char* pointer_16(const uint16& _x, const uint16& _y, const uint16& _z, const uint16& _c)const{return Data+(_x+Width*(_y+Height*(_z+Depth*_c)))*sizeof(uint16);}
    inline char* pointer_32(const uint16& _x, const uint16& _y, const uint16& _z, const uint16& _c)const{return Data+(_x+Width*(_y+Height*(_z+Depth*_c)))*sizeof(uint32);}
    inline char* pointer_64(const uint16& _x, const uint16& _y, const uint16& _z, const uint16& _c)const{return Data+(_x+Width*(_y+Height*(_z+Depth*_c)))*sizeof(uint64);}

  public:
    friend class YaidReader;
    friend class YaidWriter;

  };//class Slice


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //class YaidReader
  //for readin Yaid files from disk
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class YaidReader{
  private:
    FileReaderBinary Reader;
    Header MyHeader;
    uint64 SliceTot;
    uint64 SliceCurr;

  private:
    YaidReader(const YaidReader&); //copy constructor not implemented
    const YaidReader& operator=(const YaidReader&);  //copy operator not implemented

  public:
    ~YaidReader();
    YaidReader();
    YaidReader(const std::string&);

    //file handling
    void open(const std::string&);
    void close();

    //data reading
    inline ulint slice_size()const{
      ulint temp(MyHeader.Width*MyHeader.Height*MyHeader.Depth*MyHeader.Channels*pixel_type_to_size(type()));
      return temp+(sizeof(SliceCurr)+(8-(temp+sizeof(SliceCurr))%8)%8);
    }
    YaidReader& read_next_slice(Slice&);
    YaidReader& read_next_slice(void*);
    YaidReader& set_slice(const ulint&);

    //other
    void check(const Slice&)const;

    //operators:
    inline operator bool(){return (bool)Reader;}

    //information:
    inline const Header&   header()       const{return MyHeader;}
    inline const Cstr<8>&  file_id()      const{return MyHeader.FileID;}
    inline const uint16&   width()        const{return MyHeader.Width;}
    inline const uint16&   height()       const{return MyHeader.Height;}
    inline const uint16&   depth()        const{return MyHeader.Depth;}
    inline const uint16&   channels()     const{return MyHeader.Channels;}
    inline const PixelType type()         const{return (PixelType)MyHeader.Type;}
    inline const uint64&   current_slice()const{return SliceCurr;}

  };//class YaidReader


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //class YaidWriter
  //for writing Yaid files to disk
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class YaidWriter{
  private:
    FileWriterBinary Writer;
    Header MyHeader;
    uint64 SliceTot;
    uint64 SliceCurr;

  private:
    YaidWriter(const YaidWriter&); //copy constructor not implemented
    const YaidWriter& operator=(const YaidWriter&);  //copy operator not implemented

  public:
    ~YaidWriter();
    YaidWriter();
//     YaidWriter(const std::string&);

    //file handling
    void open(const std::string&);
    void close();

    //slice writing
    inline ulint slice_size()const{
      ulint temp(MyHeader.Width*MyHeader.Height*MyHeader.Depth*MyHeader.Channels*pixel_type_to_size(type()));
      return temp+(sizeof(SliceCurr)+(8-(temp+sizeof(SliceCurr))%8)%8);
    }
    YaidWriter& write_next_slice(const Slice&);
    YaidWriter& write_next_slice(void*);
    YaidWriter& set_slice(const ulint&);

    //other
    void check(const Slice&)const;

    //operators:
    inline operator bool(){return (bool)Writer;}

    //information
    inline const Header&   header()       const{return MyHeader;}
    inline const Cstr<8>& file_id()const{return MyHeader.FileID;}
    inline const uint16&    width()const{return MyHeader.Width;}
    inline const uint16&   height()const{return MyHeader.Height;}
    inline const uint16&    depth()const{return MyHeader.Depth;}
    inline const uint16& channels()const{return MyHeader.Channels;}
    inline const PixelType   type()const{return (PixelType)MyHeader.Type;}
    inline const uint64& current_slice()const{return SliceCurr;}

    inline void set_width   (const uint16& _w)   {if (Writer.is_open()) throw FileBusy(); MyHeader.Width=_w;}
    inline void set_height  (const uint16& _h)   {if (Writer.is_open()) throw FileBusy(); MyHeader.Height=_h;}
    inline void set_depth   (const uint16& _d)   {if (Writer.is_open()) throw FileBusy(); MyHeader.Depth=_d;}
    inline void set_channels(const uint16& _c)   {if (Writer.is_open()) throw FileBusy(); MyHeader.Channels=_c;}
    inline void set_type    (const PixelType& _t){if (Writer.is_open()) throw FileBusy(); MyHeader.Type=(uint16)_t;}
  };//class YaidWriter


}//namespace yaid
#endif
