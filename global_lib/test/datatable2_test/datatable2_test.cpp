#include <iostream>
#include "..\..\datatable2.h"

using namespace std;

int main()
{
    try {
        datatable2 tab;                   // instanciate
        tab.load_csv("output1.txt", ' ');        // load some csv file
        tab.set(0,0,10);                 // set some data
        cout<<tab.as_string()<<endl;     // output table on screen
        tab.save_sylk("test.slk");       // save changed table in SYLK format (for Excel)
    } catch(datatable2_exception e) {   // error handling with exceptions
        cout<<e.get_message()<<endl;
    }
    try {
        datatable2 tab;                   // instanciate
        tab.load_csv("output2.txt",',', '#', "[Data]");        // load some csv file
        cout<<tab.as_string()<<endl;     // output table on screen
    } catch(datatable2_exception e) {   // error handling with exceptions
        cout<<e.get_message()<<endl;
    }
   try {
        datatable2 tab;                   // instanciate
        tab.load_csv("output2.txt",',', '#', "[Data]", "[Times]");        // load some csv file
        cout<<tab.as_string()<<endl;     // output table on screen
        tab.save_sylk("test2.slk");       // save changed table in SYLK format (for Excel)
    } catch(datatable2_exception e) {   // error handling with exceptions
        cout<<e.get_message()<<endl;
    }
    try {
        datatable2 tab;                   // instanciate
        tab.load_csv("output3.txt",',', '#');        // load some csv file
        cout<<tab.as_string()<<endl;     // output table on screen
        tab.save_sylk("test3.slk");       // save changed table in SYLK format (for Excel)
    } catch(datatable2_exception e) {   // error handling with exceptions
        cout<<e.get_message()<<endl;
    }
}
