#include <iostream>
#include "..\..\jkiniparser.h"
using namespace std;

int main()
{
    jkiniparser ini;
    try {
       ini.readFile("test.ini"); // read in an INI file

       // rwad some data
       int data1=ini.getAsInt("gruppe1", "int data", -10);
       double data2=ini.getAsDouble("gruppe1", "doubledata", -10.00125);
       string data3=ini.getAsString("gruppe1", "stringdata");

       cout<<"printing all group names in the file ..."<<endl;
       if (ini.getGroupCount()>0) {
         for (unsigned long i=0; i<ini.getGroupCount(); i++) {
           cout<<ini.getGroupName(i)<<endl;;
         }
       }

       cout<<endl<<endl<<"printing all contents in the file ..."<<endl;
       ini.print();


       if (ini.groupExists("parsed")) { // check whether group exists
         // parse some property in the group
         double data4=ini.getAsParsedDouble("parsed", "data", -10.00125); // now [parsed] ... data=pi^2+5 will return the evaluated data
       }

       ini.set("gruppe2", "data", 15.678);
       ini.writeFile("test1.ini"); // write the changed INI file to disk
    } catch (jkipError e) { // error handling
       cout<<"error:   "<<e.getMessage()<<endl;
    }
    return 0;
}
