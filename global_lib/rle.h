#ifndef __RLE_H__
#define __RLE_H__

class rle{
 protected:
  unsigned int *table;
  unsigned int tableEntryCount;
 public:
  rle(unsigned int entryCount){
    table = new unsigned int [entryCount];
    for(unsigned int i=0; i<entryCount;i++){
      table[i]=0;
    }
    tableEntryCount=entryCount;
  };
  ~rle(){
    delete[] table;
  };
  virtual void process(unsigned int value) = 0;
  unsigned int *getTable(){return table;}

  rle& operator+=(rle const& rhs){
    if(tableEntryCount==rhs.tableEntryCount){
      for(unsigned int i=0;i<tableEntryCount;i++){
	table[i]+=rhs.table[i];
      }
    }
    return *this;
  }

  friend std::ostream& operator<<(std::ostream& os, const rle &RLE){
    for(unsigned int i=0;i<RLE.tableEntryCount;i++){
      os << RLE.table[i] << std::endl;
    }
    return os;
  }

  unsigned int getEntry(unsigned int i){return table[i];}
  unsigned int getEntryCount(){return tableEntryCount;}
};

/*
 * 0  ="1"
 * 1  ="01"
 * 2  ="001"
 * n-1="0...0"
 */

class rle_binary : public rle{
 private:
  unsigned int currentCount;
 public:

  rle_binary(unsigned int entryCount=256U) : rle(entryCount),currentCount(0){}
  ~rle_binary(){};

  void process(unsigned int value){
    if(value==0){
      currentCount++;
    }else{
      table[currentCount]++;
      currentCount=0;
    }
    if(currentCount==(tableEntryCount-1)){
      table[currentCount]++;
      currentCount=0;
    }
  }
};


class rle2 : public rle{
 protected:
  unsigned int tableEntryCount0,tableEntryCount1;
  unsigned int currentCount0,currentCount1,currentValue;

 public:

  rle2(unsigned int entryCount0=512,unsigned int entryCount1=8) : 
    rle(entryCount0*entryCount1),
    tableEntryCount0(entryCount0),
    tableEntryCount1(entryCount1),
    currentCount0(0),
    currentCount1(0),
    currentValue(1){}
  ~rle2(){};

  void process(unsigned int value){
    if(value!=currentValue){
      if(value==0){
	table[currentCount0*tableEntryCount1+currentCount1]++;
	currentCount0=1;
	currentCount1=0;
	currentValue=0;
      }else{
	currentValue=1;
	currentCount1++;
      }
    }else{
      if(value==0){
	currentCount0++;
      }else{
	currentCount1++;
      }
    }
    if((currentCount0==(tableEntryCount0-1)) || (currentCount1==(tableEntryCount1-1))){
      table[currentCount0*tableEntryCount1+currentCount1]++;
      currentCount0=0;
      currentCount1=0;
      currentValue=0;
    }
  }
};

#endif
