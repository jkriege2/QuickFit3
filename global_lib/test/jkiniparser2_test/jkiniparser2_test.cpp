#include <iostream>
#include "..\..\jkiniparser2.h"
#include "..\..\tools.h"
using namespace std;

int main()
{


    jkINIParser2 ini;
    try {
       ini.readFile("test.ini"); // read in an INI file
       cout<<endl<<endl<<"printing all contents in the file ..."<<endl;
       ini.print();

       // read some data
       cout<<"gruppe1.intdata = "<<ini.getAsInt("gruppe1.intdata", -10)<<endl;
       cout<<"gruppe1.doubledata1 = "<<ini.getAsDouble("gruppe1.doubledata1", -10.00125)<<endl;
       cout<<"gruppe1.stringdata = "<<ini.getAsString("gruppe1.stringdata")<<endl;

       // change the current root and set there some data
       ini.enterGroup("gruppe1");
       ini.set("intdata", -100.0);
       ini.leaveGroup();

       cout<<"printing all group names on the top level ..."<<endl;
       if (ini.getGroupCount()>0) {
         for (unsigned long i=0; i<ini.getGroupCount(); i++) {
           cout<<ini.getGroupName(i)<<endl;;
         }
       }
       ini.set("gruppe2.data", 15.678);

       cout<<endl<<endl<<"printing all contents in the file ..."<<endl;
       ini.print();


       ini.writeFile("test_out.ini"); // write the changed INI file to disk*/

       // trying to re-read the currently
       ini.readFile("test_out.ini");
       cout<<endl<<endl<<"printing all contents in the file ..."<<endl;
       ini.print();

       // read another, more complex file
       ini.readFile("diffusion3.ini");
       cout<<endl<<endl<<"printing all contents in the file ..."<<endl;
       ini.print();

    } catch (std::exception& e) { // error handling
       cout<<"error:   "<<e.what()<<endl;
    }
    return 0;
}
