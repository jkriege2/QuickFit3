#include <iostream>
#include <stdio.h>
#include <windows.h>
#include "../../../../../../LIB/trunk/jkserialconnection.h"
#include "../../../../../../LIB/trunk/tools.h"
using namespace std;

int main() {
    JKSerialConnection conn("COM8", 115200);
    char filename[]="test_D30_I1_IM100.dat";
    if (conn.open()) {
        conn.clearBuffer();
        conn.write("T400\n");
        conn.write("O0\n");
        conn.clearBuffer();
        FILE* f=fopen(filename, "w");
        fprintf(f,"");
        fclose(f);
        for (int i=0; i<60*60; i++) {
            if (i==10) conn.write("O1\n");
            conn.write("S\n");
            bool ok=true;
            string d=conn.readUntil('\n', &ok);
            if (ok) {
                f=fopen(filename, "a");
                if (f) {
                    fprintf(f, "%f, %s", (float)i, d.c_str());
                    printf("%f, %s", (float)i, d.c_str());
                    fclose(f);
                }
            } else {
                cerr<<"read error at i="<<i<<endl;
            }
            Sleep(500);
        }
        conn.close();
    } else {
        cerr<<"error opening connection\n";
    }



    return 0;
}
