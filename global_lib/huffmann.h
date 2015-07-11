#ifnfed __HUFFMANN_H__
#define __HUFFMANN_H__

class huffman{
 protected:
 private:
  huffman(){
  }
  
  static bool generateTree(unsigned int pos, unsigned int pool, unsigned int remaining,char *s){
    if(pool==0){
      s[pos]=0;
      printf("%s\n",s);
      return true;
    }

    //normal
    generateTree(pos+1,2,remaining-1);
    //skip
    generateTree(pos+1,4,remaining);

    return true;
  }
}

#endif
