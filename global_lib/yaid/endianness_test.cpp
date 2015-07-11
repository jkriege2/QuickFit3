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
#include <iostream>
#include <iomanip>

#include "types.hpp"

namespace yaid{
  uint64& endian_swap(uint64& _i){
    _i=((_i>>56)|
	((_i>>40) & 0x000000000000FF00ull) |
	((_i>>24) & 0x0000000000FF0000ull) |
	((_i>>8)  & 0x00000000FF000000ull) |
	((_i<<8)  & 0x000000FF00000000ull) |
	((_i<<24) & 0x0000FF0000000000ull) |
	((_i<<40) & 0x00FF000000000000ull) |
	(_i<<56)); 
    return _i;
  }
}


int main(){

  using namespace yaid;


  std::cout << "#ifndef __YAID_ENDIANNESS_HPP\n#define __YAID_ENDIANNESS_HPP\n";


  union{
    uint16 Type;
    uint8 Order[sizeof(Type)];
  }Type16;
  
  Type16.Type=0x1234u;
  
  std::cout << "#define YAID_ENDIANNESS_16 0x" 
	    << std::hex << (uint)Type16.Order[0] << std::hex << (uint)Type16.Order[1] << "u" << std::endl;


  union{
    uint32 Type;
    uint8 Order[sizeof(Type)];
  }Type32;
  
  Type32.Type=0x12345678u;
  
  std::cout << "#define YAID_ENDIANNESS_32 0x" 
	    << std::hex << (uint)Type32.Order[0] << std::hex << (uint)Type32.Order[1]
	    << std::hex << (uint)Type32.Order[2] << std::hex << (uint)Type32.Order[3] << "u" << std::endl;


  union{
    uint64 Type;
    uint8 Order[sizeof(Type)];
  }Type64;
  
  Type64.Type=0x123456789abcdefllu;
  
  std::cout << "#define YAID_ENDIANNESS_64 0x" 
	    << std::hex << (uint)Type64.Order[0] << std::hex << std::setw(2) << std::setfill('0') << (uint)Type64.Order[1]
	    << std::hex << (uint)Type64.Order[2] << std::hex << std::setw(2) << std::setfill('0') << (uint)Type64.Order[3]
	    << std::hex << (uint)Type64.Order[4] << std::hex << std::setw(2) << std::setfill('0') << (uint)Type64.Order[5]
	    << std::hex << (uint)Type64.Order[6] << std::hex << std::setw(2) << std::setfill('0') << (uint)Type64.Order[7] 
	    << "llu" << std::endl;


  std::cout << "#define YAID_ENDIANNESS_DECIMAL1 " << std::dec << Type64.Type << std::endl;
  std::cout << "#define YAID_ENDIANNESS_DECIMAL2 " << std::dec << endian_swap(Type64.Type) << std::endl;

  std::cout << "#endif\n";

}//main()
