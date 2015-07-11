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

#include "yaid.hpp"

namespace yaid{



  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //class Slice
  //for readin Yaid files from disk
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Slice::~Slice()
  {
  }


  Slice::Slice(const Slice& _orig):
    Width(_orig.Width), Height(_orig.Height), Depth(_orig.Depth), Channels(_orig.Channels),
    Type(_orig.Type), DataSize(_orig.DataSize), MemSize(0), Data(NULL),
    WriteFunc(&Slice::write_8), ReadFunc(&Slice::read_8), PointerFunc(&Slice::pointer_8)
  {
    allocate();

    //copying the data
    for (ulint i(0); i<DataSize; ++i) Data[i]=_orig.Data[i];
  }


  Slice::Slice(const Header& _h):
    Width(_h.Width), Height(_h.Height), Depth(_h.Depth), Channels(_h.Channels), Type(_h.Type), 
    DataSize(Width*Height*Depth*Channels*pixel_type_to_size((PixelType)Type)), MemSize(0), Data(NULL),
    WriteFunc(&Slice::write_8), ReadFunc(&Slice::read_8), PointerFunc(&Slice::pointer_8)
  {
    allocate();
    set_pointers();
  }


  void Slice::allocate()
  {
    uint64 Size((DataSize/1024-(1024-DataSize%1024)/1024+1)*1024);
//     std::cout << "Size = " << Size << std::endl;
    if (Size>MemSize){
      //allocating:
      char* ptr_new(new char[Size]);
      if (MemSize>0){

	//copying
	for (ulint i(0); i<DataSize; ++i) ptr_new[i]=Data[i];

	//freeing
	delete[] Data;
      } //if(MemSize)

      Data=ptr_new;
      MemSize=Size;
    }//if(Size>MemSize)
  }//allocate()


  void Slice::free()
  {
    if (MemSize>0) delete[] Data;
    MemSize=0;
  }


  void Slice::set_pointers()
  {
    //deciding which function to call for writing
    switch((PixelType)Type){
    case(Tint16):
    case(Tuint16):
      WriteFunc=&Slice::write_16;
      ReadFunc=&Slice::read_16;
      PointerFunc=&Slice::pointer_16;
//       _writer.write((uint16*)Data,DataSize/sizeof(uint16));
      break;
    case(Tint32):
    case(Tuint32):
      WriteFunc=&Slice::write_32;
      ReadFunc=&Slice::read_32;
      PointerFunc=&Slice::pointer_32;
//       _writer.write((uint32*)Data,DataSize/sizeof(uint32));
      break;
    case(Tint64):
    case(Tuint64):
      WriteFunc=&Slice::write_64;
      ReadFunc=&Slice::read_64;
      PointerFunc=&Slice::pointer_64;
//       _writer.write((uint64*)Data,DataSize/sizeof(uint64));
      break;
    case(Tfloat):
      throw Exception("Slice::write(): Tfloat not defined");
      break;
    case(Tdouble):
      throw Exception("Slice::write(): Tdouble not defined");
      break;
    case(Tundefined):
    case(Tbool):
    case(Tchar):
    case(Tuchar):
    case(Tint8):
    case(Tuint8):
    default:
      WriteFunc=&Slice::write_8;
      ReadFunc=&Slice::read_8;
      PointerFunc=&Slice::pointer_8;
//       _writer.write((char*)Data,DataSize/sizeof(uint8));
      break;
    }//switch(Type)
  }

  void Slice::set_parameters(const Header& _h)
  {
    Width    = _h.Width;
    Height   = _h.Height;
    Depth    = _h.Depth;
    Channels = _h.Channels;
    Type     = _h.Type;
    DataSize = Width*Height*Depth*Channels*pixel_type_to_size((PixelType)Type);
    allocate();
    set_pointers();
  }


//   void Slice::read(FileReaderBinary& _reader)const
//   {
//     //deciding which function to call
//     switch((PixelType)Type){
//     case(Tint16):
//     case(Tuint16):
//       _reader.read((uint16*)Data,DataSize/sizeof(uint16));
//       break;
//     case(Tint32):
//     case(Tuint32):
//       _reader.read((uint32*)Data,DataSize/sizeof(uint32));
//       break;
//     case(Tint64):
//     case(Tuint64):
//       _reader.read((uint64*)Data,DataSize/sizeof(uint64));
//       break;
//     case(Tfloat):
//       throw Exception("Slice::read(): Tfloat not defined");
//       break;
//     case(Tdouble):
//       throw Exception("Slice::read(): Tdouble not defined");
//       break;
//     case(Tundefined):
//     case(Tbool):
//     case(Tchar):
//     case(Tuchar):
//     case(Tint8):
//     case(Tuint8):
//     default:
//       _reader.read((char*)Data,DataSize/sizeof(uint8));
//       break;
//     }//switch(Type)
//   }//read()


//   void Slice::write(FileWriterBinary& _writer)const
//   {
//     //deciding which function to call
//     switch((PixelType)Type){
//     case(Tint16):
//     case(Tuint16):
//       _writer.write((uint16*)Data,DataSize/sizeof(uint16));
//       break;
//     case(Tint32):
//     case(Tuint32):
//       _writer.write((uint32*)Data,DataSize/sizeof(uint32));
//       break;
//     case(Tint64):
//     case(Tuint64):
//       _writer.write((uint64*)Data,DataSize/sizeof(uint64));
//       break;
//     case(Tfloat):
//       throw Exception("Slice::write(): Tfloat not defined");
//       break;
//     case(Tdouble):
//       throw Exception("Slice::write(): Tdouble not defined");
//       break;
//     case(Tundefined):
//     case(Tbool):
//     case(Tchar):
//     case(Tuchar):
//     case(Tint8):
//     case(Tuint8):
//     default:
//       _writer.write((char*)Data,DataSize/sizeof(uint8));
//       break;
//     }//switch(Type)
//   }//write()


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //class YaidReader
  //for readin Yaid files from disk
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  YaidReader::~YaidReader()
  {
  }


  YaidReader::YaidReader():Reader(),MyHeader(),SliceTot(0),SliceCurr(0)
  {
  }


  YaidReader::YaidReader(const std::string& _filename):Reader(),MyHeader(),SliceTot(0),SliceCurr(0)
  {
    open(_filename);
  }


  void YaidReader::open(const std::string& _filename)
  {
    std::cout << "the Header before reading from file:\n";
    std::cout << MyHeader << std::endl;

    Reader.open(_filename);

    Reader >> MyHeader;
    std::cout << "\nand after:\n";
    std::cout << MyHeader << std::endl;

    //for now we just fast-forward to the first slice:
    Reader.seekg(MyHeader.Offset, std::ios_base::cur);

  }


  void YaidReader::close()
  {
    Reader.close();
  }


  YaidReader& YaidReader::read_next_slice(Slice& _s)
  {
    //checking Slice Dimensions agains Header
    check(_s);

    //reading slice number:
    Reader >> SliceCurr;

    //making the Slice read the data:
    _s.read(Reader);

    //jumping over the fill-bits at the end of the slice:
    Reader.seekg((8-(_s.DataSize+sizeof(SliceCurr))%8)%8, std::ios_base::cur);


    if ((SliceCurr++)==SliceTot) ++SliceTot; //to get the total number of slices correct
    return *this;
  }//read_next_slice(Slice)


  YaidReader& YaidReader::read_next_slice(void* _pointer)
  {
    ulint size(MyHeader.Width*MyHeader.Height*MyHeader.Depth*MyHeader.Channels);

    //reading slice number:
    Reader >> SliceCurr;
    Reader.read((char*)_pointer, size);

    //jumping over the fill-bits at the end of the slice:
    Reader.seekg((8-(size+sizeof(SliceCurr))%8)%8, std::ios_base::cur);


    if ((SliceCurr++)==SliceTot) ++SliceTot; //to get the total number of slices correct
    return *this;
  }


  void YaidReader::check(const Slice& _s)const{
    if (MyHeader.Width!=_s.Width) throw DimensionMismatch("width");
    if (MyHeader.Height!=_s.Height) throw DimensionMismatch("height");
    if (MyHeader.Depth!=_s.Depth) throw DimensionMismatch("depth");
    if (MyHeader.Channels!=_s.Channels) throw DimensionMismatch("channels");
    if (MyHeader.Type!=_s.Type) throw DimensionMismatch("type");
  }


  YaidReader& YaidReader::set_slice(const ulint& _num){
    int dist(_num-SliceCurr);
    Reader.seekg(dist*slice_size(),std::ios_base::cur);

    if(!Reader) throw OutOfBounds("given slice number larger than slice number of file");

    return *this;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //class YaidWriter
  //for reading Yaid files from disk
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  YaidWriter::~YaidWriter()
  {
  }


  YaidWriter::YaidWriter():Writer(),MyHeader(),SliceTot(0),SliceCurr(0)
  {
  }


//   YaidWriter::YaidWriter(const std::string& _filename):Writer(),MyHeader(),SliceTot(0),SliceCurr(0)
//   {
//     open(_filename);
//   }


  void YaidWriter::open(const std::string& _filename)
  {
    std::cout << "What we are going to save:\n";
    std::cout << MyHeader << std::endl;

    //writing the header
    Writer.open(_filename);
    Writer << MyHeader;

    //reserving an MyHeader.Offset amount of bytes for tags:
    for (ulint i(0); i<MyHeader.Offset; ++i) Writer << uchar(0x00);

  }


  void YaidWriter::close()
  {
    Writer.close();
  }


  YaidWriter& YaidWriter::write_next_slice(const Slice& _s)
  {
    static const char fillbytes[8]={'\0','\0','\0','\0','\0','\0','\0','\0'};

    //checking Slice Dimensions agains Header
    check(_s);

    //writing slice number:
    Writer << SliceCurr;

    //making the Slice write the data:
    _s.write(Writer);

    //filling up with chars to 64 bit (8 byte) length:
    Writer.write(fillbytes,(8-(_s.DataSize+sizeof(SliceCurr))%8)%8);

    ++SliceCurr;
    ++SliceTot;
    return *this;
  }//write_next_slice(Slice)


  YaidWriter& YaidWriter::write_next_slice(void* _pointer)
  {
    static const char fillbytes[8]={'\0','\0','\0','\0','\0','\0','\0','\0'};

    ulint size(MyHeader.Width*MyHeader.Height*MyHeader.Depth*MyHeader.Channels);

    Writer << SliceCurr;

    //writing the actual data:
    Writer.write((const char*)_pointer, size);

    //filling up with chars to 64 bit (8 byte) length:
    Writer.write(fillbytes,(8-(size+sizeof(SliceCurr))%8)%8);

    ++SliceCurr;
    ++SliceTot;
    return *this;
  }//write_next_slice(void*)


  void YaidWriter::check(const Slice& _s)const{
    if (MyHeader.Width!=_s.Width) throw DimensionMismatch("width");
    if (MyHeader.Height!=_s.Height) throw DimensionMismatch("height");
    if (MyHeader.Depth!=_s.Depth) throw DimensionMismatch("depth");
    if (MyHeader.Channels!=_s.Channels) throw DimensionMismatch("channels");
    if (MyHeader.Type!=_s.Type) throw DimensionMismatch("type");
  }


}//namespace yaid



