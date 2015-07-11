#include <iostream>
#include <cstdio>
#include "../shiftregister.h"

using namespace std;

int main()
{
    ShiftRegister<double> sr(5);
    double d[5]={};
    for (int i=0; i<16; i++) {
        cout<<sr.to_string();
        cout<<"\tpeek="<<sr.peek();
        if (i<8) {
            cout<<"\tpush_left("<<i+1<<")";
            sr.push_left(i+1);
        } else {
            cout<<"\tpush_right("<<i+1<<")";
            sr.push_right(i+1);
        }
        sr.multiplyAddToExternal(d, 2.0);
        cout<<"\t"<<sr.to_string()<<std::endl;
    }
    cout<<endl<<endl;
    for (int i=0; i<5; i++) {
        cout<<d[i]<<", ";
    }
    return 0;
}

