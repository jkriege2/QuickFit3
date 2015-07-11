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
#include "yaid_test.hpp"


int main(){
  using namespace yaid;

  //opening yaid file
  YaidWriter output;

  //changing the default values for the first one:
  uint16 width(16);
  uint16 height(8);
  uint16 depth(2);
  uint16 channels(1);
  PixelType type(Tuint32);

  output.set_width(width);
  output.set_height(height);
  output.set_depth(depth);
  output.set_channels(channels);
  output.set_type(type);
  output.open("header_output.yaid");

  //writing some slices:
  //allocating memory:
//   ulint size(width*height*depth*channels);
//   uchar* data(new uchar[size]);

//   //writing the slices:
//   ulint divider(32);
//   for (ulint i(0); i<4; ++i){

//     //filling data with meaning:
//     for (ulint i(0); i<size; ++i) data[i]=i&(divider-1);  //%divider

//     output.write_next_slice((void*)data);

//     divider*=2;
//   }

//   //freeing memory:
//   delete[] data;
  Slice TestSlice(output.header());

  uint8 val(0);

  for (ulint i(0); i<4; ++i){
    //filling data field

    for (uint16 c(0); c<channels; ++c){
      for (uint16 z(0); z<depth; ++z){
	for (uint16 y(0); y<height; ++y){
	  for (uint16 x(0); x<width; ++x){
	    TestSlice.write_pixel(x,y,z,c,++val);
	  }//for(x)
	}//for(y)
      }//for(z)
    }//for(c)

    output.write_next_slice(TestSlice);
  }//for(i)

  output.close();

  //reading the data back in:
  YaidReader input("header_output.yaid");
  YaidWriter copy;                           //here we store the copy of the original:
  copy.set_width(input.width());
  copy.set_height(input.height());
  copy.set_depth(input.depth());
  copy.set_channels(input.channels());
  copy.set_type(input.type());
  copy.open("header_copy.yaid");

  TestSlice.set_parameters(copy.header());

  //allocating and cleaning memory:
//   ulint input_size(input.width()*input.height()*input.depth()*input.channels());
//   uchar* data(new uchar[input_size]);
//   for (ulint i(0); i<input_size; ++i) data[i]=0;

  //reading the data:
  while (input.read_next_slice(TestSlice)){
    //some testing
    copy.write_next_slice(TestSlice);
  }

//   //reading the data:
//   while (input.read_next_slice((void*)data)){
//     //some testing
//     copy.write_next_slice((void*)data);
//   }

//   delete[] data;
  input.close();
  copy.close();

//   Writer16<0x1234>().write(0xabcd);
//   Writer16<0x3412>().write(0xabcd);

//   Writer32<0x12345678>().write(0x9abcdef0);
//   Writer32<0x78563412>().write(0x9abcdef0);

//   Writer64<0x123456789abcdef0>().write(0x123456789abcdf0);
//   Writer64<0xf0debc9a78563412>().write(0x123456789abcdf0);

//   write(uint16(0xabcd));
//   write(uint16(0x0001));
//   write(uint32(0xabcd));
//   write(uint32(0x0001));
//   write(uint64(0xabcd));
//   write(uint64(0x0001));

  return 0;
}//main()



