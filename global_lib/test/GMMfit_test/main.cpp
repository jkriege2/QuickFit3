#include <iostream>
#include "GMMfit.h"
#include "../ticktock.h"


using namespace std;

int main()
{


    cout << "test Gaussian distribution" << endl;
    Gaussian *gauss = new Gaussian(2);
    Eigen::VectorXd x;
    x.resize(2);
    x<<2,3;

    cout << "x=" << endl << x << endl;
    cout <<gauss->p(x) << endl;
    cout <<gauss->logP(x) << endl;
    //cout <<gauss->p(x.data()) << endl;
    //cout <<gauss->logP(x.data()) << endl;


//    std::cout<<"Gaussian in 1D"<<std::endl;
//    std::cout<<"x\tp(x)\tlog(p(x))\tlogP(x)"<<std::endl;
//    double *xx = new double;
//    for(int i=0;i<100;++i) {
//        *xx = 5-10*(100-(double)i)/100;
//        std::cout<<*xx<<"\t"<<gauss->p(xx)<<"\t"<<log(gauss->p(xx))<<"\t"<<gauss->logP(xx)<<std::endl;
//    }

    gauss->init();
    gauss->print();
    //cout <<gauss->p(x) << endl;

    x.resize(3);
    x<<4,1,2;
    gauss->setMu(x);

    // empty model
    GMMfit *gmm1 = new GMMfit();
    gmm1->fit();
    gmm1->print();


    std::cout<<"fitting GMM with 1 component"<<std::endl;
    std::vector<double> dataVector;
    dataVector.resize(5,0);
    dataVector.push_back(1.0);
    dataVector.push_back(0.5);
    dataVector.push_back(0.7);
    dataVector.push_back(-0.1);
    dataVector.push_back(-0.2);
    dataVector.push_back(0.3);
    dataVector.push_back(0.5);
    dataVector.push_back(0.4);
    dataVector.push_back(0);
    dataVector.push_back(0);
    dataVector.push_back(0.3);
    dataVector.push_back(.1);
    dataVector.resize(20,0);

    GMMfit *gmm = new GMMfit(2,1);
    x.resize(2);
    x<<1,1;
    gmm->getGauss(0).setMu(x);
    gmm->setUseSingleArray(true);
    gmm->setData(dataVector.data(),dataVector.size());
    gmm->fit();
    gmm->print();

//return 0;
    std::cout<<"fitting GMM with 2 components"<<std::endl;
    dataVector.push_back(11.5);
    dataVector.push_back(11.7);
    dataVector.push_back(12.5);
    dataVector.push_back(12.1);
    dataVector.push_back(11.5);
    dataVector.push_back(12.2);
    dataVector.push_back(12.5);
    dataVector.push_back(12);
    dataVector.push_back(12.1);
    dataVector.push_back(12.5);
    dataVector.push_back(12);
    dataVector.push_back(12.1);
    dataVector.push_back(11.9);
    dataVector.push_back(12.1);
    dataVector.resize(36,12);

    GMMfit *gmm2 = new GMMfit(2,2);
    // parameter estimation
    x<<0,0;
    gmm2->getGauss(0).setMu(x);
    x<<12,10;
    gmm2->getGauss(1).setMu(x);
    gmm2->setUseSingleArray(true);
    gmm2->setData(dataVector.data(),dataVector.size());
    gmm2->fit();
    gmm2->print();





    std::cout<<"test seperate data vectors for each dimension"<<std::endl;
    dataVector.clear();
    std::vector<double> dataVector2;
    dataVector.push_back(2.1); dataVector2.push_back(0);
    dataVector.push_back(2.2); dataVector2.push_back(0.1);
    dataVector.push_back(2.51);dataVector2.push_back(0.2);
    dataVector.push_back(2.4);dataVector2.push_back(0);
    dataVector.push_back(2.43);dataVector2.push_back(.15);
    dataVector.push_back(2.44);dataVector2.push_back(.25);
    dataVector.push_back(2.4);dataVector2.push_back(.22);
    dataVector.push_back(2.3);dataVector2.push_back(.21);
    dataVector.push_back(2.34);dataVector2.push_back(.25);
    dataVector.push_back(2.44);dataVector2.push_back(.12);
    dataVector.push_back(2.35);dataVector2.push_back(.1);
    std::vector<double*> data;
    data.push_back(dataVector.data());
    data.push_back(dataVector2.data());
    gmm->setData(data,dataVector.size());
    GMMfitOptions options;
    options.threshold=1e-3;
    x.resize(2);
    x<<3,0;
    gmm->getGauss(0).setMu(x);
    gmm->setOptions(options);
    gmm->fit();
    gmm->print();


    PublicTickTock tg;
    tg.tick();

    std::cout<<"example with large data set"<<std::endl;
    dataVector.clear(); dataVector2.clear();

    PublicTickTock t;

    t.tick();
    for(int i=0;i<500;++i) {
        dataVector.push_back(2.1); dataVector2.push_back(0);
        dataVector.push_back(2.2); dataVector2.push_back(0.1);
        dataVector.push_back(2.51);dataVector2.push_back(0.2);
        dataVector.push_back(2.4);dataVector2.push_back(0);
        dataVector.push_back(2.43);dataVector2.push_back(.15);
        dataVector.push_back(2.44);dataVector2.push_back(.25);
        dataVector.push_back(2.4);dataVector2.push_back(.22);
        dataVector.push_back(2.3);dataVector2.push_back(.21);
        dataVector.push_back(2.34);dataVector2.push_back(.25);
        dataVector.push_back(2.44);dataVector2.push_back(.12);

        dataVector.push_back(3.1); dataVector2.push_back(0);
//        dataVector.push_back(5.2); dataVector2.push_back(0.1);
//        dataVector.push_back(5.51);dataVector2.push_back(0.2);
//        dataVector.push_back(5.4);dataVector2.push_back(0);
//        dataVector.push_back(5.43);dataVector2.push_back(.15);
//        dataVector.push_back(5.44);dataVector2.push_back(.25);
//        dataVector.push_back(5.4);dataVector2.push_back(.22);
        dataVector.push_back(5.3);dataVector2.push_back(.21);
        dataVector.push_back(5.34);dataVector2.push_back(.25);
        dataVector.push_back(5.44);dataVector2.push_back(.12);
    }
    t.tockPrint("vector");
    x<<1,1;
    gmm2->getGauss(0).setMu(x);
    x<<4,2;
    gmm2->getGauss(1).setMu(x);
    data.clear();
    data.push_back(dataVector.data());
    data.push_back(dataVector2.data());
    t.tick();
    gmm2->setData(data,dataVector.size());
    t.tockPrint("setData");
    t.tick();
    gmm2->fit();
    t.tockPrint("main::fit");
    t.tick();
    gmm2->print();
    t.tockPrint("print");

    tg.tockPrint("ex large data set");

    return 0;
}

