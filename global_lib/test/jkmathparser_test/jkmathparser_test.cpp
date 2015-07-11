#include <iostream>
#include <exception>
#include "../../jkmathparser.h"

using namespace std;


void error(std::string message) {
    cout<<"error: "<<message<<endl;
    system("PAUSE");
    cout<<endl;
}

int main()
{
    try {
        jkMathParser mp; // instanciate
        jkMathParser::jkmpNode* n;
        jkMathParser::jkmpResult r;
        // parse some numeric expression
        n=mp.parse("pi^2+4*sin(65*pi/exp(3.45))");
        r=n->evaluate();
        cout<<"pi^2+4*sin(65*pi/exp(3.45)) = "<<r.num<<endl;

        n=mp.parse("5*2+3"); r=n->evaluate();
        cout<<"5*2+3="<<r.num<<endl;
        delete n;
        n=mp.parse("5*(2+3)"); r=n->evaluate();
        cout<<"5*(2+3)="<<r.num<<endl;
        delete n;

        cout<<endl;

        // parse some boolean expression
        n=mp.parse("true==false"); cout<<"true==false = ";
        r=n->evaluate();
        if (r.type==jkMathParser::jkmpBool) {
            if (r.boolean) cout<<"true";
            else cout<<"false";
        }
        cout<<endl;  delete n;

        n=mp.parse("false&&true||true&&false"); cout<<"false&&true||true&&false = ";
        r=n->evaluate();
        if (r.type==jkMathParser::jkmpBool) {
            if (r.boolean) cout<<"true";
            else cout<<"false";
        }
        cout<<endl;  delete n;

        n=mp.parse("false||true&&true||false"); cout<<"false||true&&true||false = ";
        r=n->evaluate();
        if (r.type==jkMathParser::jkmpBool) {
            if (r.boolean) cout<<"true";
            else cout<<"false";
        }
        cout<<endl;  delete n;

        n=mp.parse("1>5 || 5<6"); cout<<"1>5 || 5<6 = ";
        r=n->evaluate();
        if (r.type==jkMathParser::jkmpBool) {
            if (r.boolean) cout<<"true";
            else cout<<"false";
        }
        cout<<endl;  delete n;

        n=mp.parse("1>5 && 5<6"); cout<<"1>5 && 5<6 = ";
        r=n->evaluate();
        if (r.type==jkMathParser::jkmpBool) {
            if (r.boolean) cout<<"true";
            else cout<<"false";
        }
        cout<<endl;  delete n;

        n=mp.parse("1+8>5 && 5<6"); cout<<"1+8>5 && 5<6 = ";
        r=n->evaluate();
        if (r.type==jkMathParser::jkmpBool) {
            if (r.boolean) cout<<"true";
            else cout<<"false";
        }
        cout<<endl;  delete n;

        cout<<endl<<endl;

        // parse some string expression
        n=mp.parse("var1=\"false\"; var1+\"true\"");
        r=n->evaluate();
        if (r.type==jkMathParser::jkmpString) cout<<r.str<<endl;
        delete n;

        cout<<endl<<endl;


        // generate an error, handled by the error() function
        mp.set_exception_function(error);
        n=mp.parse("\"false\"+5");
        r=n->evaluate();
        if (r.type==jkMathParser::jkmpString) cout<<r.str<<endl;
        delete n;
        mp.reset_exception_function();

        // generate an error, handled by an exception
        n=mp.parse("\"false\"+5");
        r=n->evaluate();
        if (r.type==jkMathParser::jkmpString) cout<<r.str<<endl;
        delete n;

    } catch(std::exception& E) {
        cout<<"ERROR!!!\n  "<<E.what()<<endl<<endl;
    }
    return 0;
}
