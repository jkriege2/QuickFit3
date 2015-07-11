/*
Copyright (c) 2008-2014 Jan Buchholz, German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CRC_H
#define CRC_H

#include <stdint.h>
#include <stdio.h>
#include <cstdlib>

//from http://de.wikipedia.org/wiki/Zyklische_Redundanzpr%C3%BCfung

class crc16 {

public:

  void reset() {
    crc=0xFFFF;
  }

  crc16(uint16_t poly=0xA001) {
    reset();
    this->poly=poly;
  };

  ~crc16() {};

	template <typename T>
	inline static T reverse(T data) {
		T result=0;
		for(unsigned int i=0;i<sizeof(T)*8;i++) {
			result <<= 1;
			result=result | (data & 0x1);
			data >>= 1;
		}
		return result;
	}

void processByte(uint8_t inByte) {
		uint16_t crc_old=crc;
		uint8_t byte=reverse<uint8_t>(inByte);
		crc=(unsigned char) (crc>>8) | (crc<<8);
		crc ^= byte;
    for(unsigned int n=0;n<8;n++) {
      if((crc&0x8000U)!=0) {
        crc=(crc << 1) ^ poly;
      } else {
        crc=crc << 1;
      }
    }
		crc=(unsigned char) (crc>>8) | (crc<<8);
    printf("[%4.4x] (%2.2x) %4.4x->%4.4x\n",poly,byte,crc_old,crc);
	}

  void processBytes(const unsigned char * data,unsigned int len) {
    const unsigned char *ptr=data;
    for(unsigned int i=0;i<len;i++) {
      processByte(*ptr++);
    }
  }

  inline uint16_t getValue() {
    return crc;
  }

  unsigned int search(const unsigned char *data, unsigned int len) {
    const unsigned char *ptr=data;
    unsigned int pos=0;
    unsigned result=0;
    do {
      processBytes(ptr,1);
      ptr++;
      uint16_t crc0=*((uint16_t*)ptr);
      uint16_t crc1=getValue();
      if(crc0==crc1)result=pos;
			printf("%4.4i %4.4x = %4.4x\n",pos,crc0,crc1);
      pos++;
    }while((pos<len)||(result!=0));
    return result;
  }

private:
  uint16_t crc,poly;

};

#endif // CRC_H
