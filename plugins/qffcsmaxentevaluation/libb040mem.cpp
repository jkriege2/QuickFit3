/** \file libb040mem.cpp
*/

#include "libb040mem.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <math.h>
#include <fstream>
#include <sstream>

#include <QDebug>

#include "Eigen/Dense"
#include "Eigen/SVD"


#define QFFloatIsOK(v) (std::isfinite(v) && (!std::isinf(v)) && (!std::isnan(v)))
#define sqr(x) ((x)*(x))


// Default Constructor
MaxEntB040::MaxEntB040() {
    
    m_Nd=0; //initialisation
    m_kappa=6.0; //default if not overwritten by user
    m_tripTau=3.0*1e-6; //default if not overwritten by user (in microseconds)
    m_tripTheta=0.2; // default if not overwritten by user
    // m_cutoff has been raplced, since the data range is given to the  class as externally
	//m_cutoff=3; // number of data points that are omitted
	m_N=100; // number of distribution points (default value)
	m_s=0; //internal deafult value for the singular space dimension
	m_oldDist=false; //default
    NumIter=200;
    m_model=0; //default

}

//Default Destructor
MaxEntB040::~MaxEntB040(){}




void MaxEntB040::setData(const double* taus, const double* correlation,\
	const double* weights,unsigned long long Nd,int rangeMinDatarange,\
    int rangeMaxDatarange,uint32_t Ndist,double * dist, double * distTaus, int model,\
    int parameterCount,double * param_list)
	{
    m_Nd=rangeMaxDatarange-rangeMinDatarange;
	m_xdata.resize(rangeMaxDatarange-rangeMinDatarange);
    m_ydata.resize(rangeMaxDatarange-rangeMinDatarange);
    m_stdev.resize(rangeMaxDatarange-rangeMinDatarange);
	for (int i=0; i<(rangeMaxDatarange-rangeMinDatarange); i++)
        {
        m_xdata(i)=taus[i+rangeMinDatarange];
        m_ydata(i)=correlation[i+rangeMinDatarange];
		m_stdev(i)=fabs(weights[i+rangeMinDatarange]);
		if (fabs(weights[i+rangeMinDatarange])<1e-3) m_stdev(i)=1e-3;
		if (fabs(weights[i+rangeMinDatarange])>1e3) m_stdev(i)=1e3;
        }
	
    ///// model specific transformation of the ydata //////////
    switch (model)
    {
        case 1:
        double offset;
        offset=param_list[5];
        for (int i=0;i<m_Nd;i++)
            {
            m_ydata(i)=m_ydata(i)-offset;
            }
        break;

    }
    ///////////////////////////////////////////////////////////





	double value;
	double value2=m_Nd;
	value=m_stdev.sum();
	m_stdev=m_stdev.array()*(value2/value); //renormalization to 1
	//////////
	m_N=Ndist;
	//////////

    if (dist==NULL || distTaus==NULL ){m_oldDist=false;}
    else 
    	{
    		m_oldDist=true;
    		m_taudiffs.resize(m_N);
    		m_f.resize(m_N);
            for (uint32_t i=0; i<Ndist; i++)
    			{
    				m_taudiffs(i)=distTaus[i];	
    				m_f(i)=dist[i];
    			}
    	}
    }


bool MaxEntB040::run(double alpha,int NumIter,double * param_list,int model,int parameter_count)
    {



    m_alpha=alpha;
    m_model=model;
    m_paramStore.resize(parameter_count);
    for (int i=0; i<parameter_count; i++)
        {
            m_paramStore(i)=param_list[i];
        }

    /*
    switch(m_model)
        {
            case 0:
                    m_tripTau=param_list[0];
                    m_tripTheta=param_list[1];
                    m_kappa=param_list[2];
                    break;
         }
    */

/*
    m_kappa=kappa;
    m_tripTau=tripTau;
    m_tripTheta=tripTheta;
*/

    this->NumIter=NumIter;

    qDebug() << "DEBUG #3 from memB040 alpha = " << alpha << "Ndist = "<< m_N << "NumIter= " << NumIter << "model= " << m_model;

    if (m_oldDist==false){setTauGrid();}
    setTmatrix();
    performSVD();
	setMmatrix();
	performIter();





    return true;
    }



void MaxEntB040::setTauGrid()
    {
    m_taudiffs.resize(m_N);
    /*
    double tminexp=floor(log10(m_xdata(0)));
	double tmaxexp=ceil(log10(m_xdata(m_Nd-1)));
	double taumin=pow(10.0,tminexp);
	double taumax=pow(10.0,tmaxexp);
	*/
	double taumin=m_xdata(0)+0.1*m_xdata(0);
	double taumax=m_xdata(m_Nd-1)-0.1*m_xdata(m_Nd-1);
	for (int i=0; i<m_N; i++)
        {
        m_taudiffs(i) = taumin*exp((i)*log(10)*log10(taumax/(taumin))/(m_N-1));
        }   
    }


void MaxEntB040::setTmatrix(){
	
	m_T.resize(m_Nd,m_N);

    double value;
    double tripFactor;
    double darkTau;
    double darkTheta;
    double offset;
    double tau1;
    double tau2;
    double fraction;
    double particle_number;
    double factor;

    qDebug() << "calling from setTmatrix with model= " << m_model;

    //m_model=0;
    switch (m_model)
    {
    // model #1: FCS 3D diffusion with triplet
    case 0:

        m_tripTau=m_paramStore(0);
        m_tripTheta=m_paramStore(1);
        m_kappa=m_paramStore(2);



        for (int i=0; i<m_Nd; i++)
            {
                for (int j=0; j<m_N; j++)
                    {
                        tripFactor=(1.0-m_tripTheta+m_tripTheta*\
                        exp(-m_xdata(i)/m_tripTau))/(1.0-m_tripTheta);
                        value=(1.0/((1.0+m_xdata(i)/(m_taudiffs(j)))))*\
                        (1.0/(sqrt(1.0+m_xdata(i)/(m_taudiffs(j)*m_kappa*m_kappa))));
                        m_T(i,j)=tripFactor*value;
                    }
            }
        break;
    // model #2: FCS 3D diffusion with two blinking components
    case 1:
        m_tripTau=m_paramStore(0);
        m_tripTheta=m_paramStore(1);
        m_kappa=m_paramStore(2);
        darkTau=m_paramStore(3);
        darkTheta=m_paramStore(4);
        offset=m_paramStore(5);


        for (int i=0; i<m_Nd; i++)
            {
            tripFactor=(1.0-m_tripTheta+m_tripTheta*exp(-m_xdata(i)/m_tripTau)-darkTheta+darkTheta*exp(-m_xdata(i)/darkTau))/(1.0-m_tripTheta-darkTheta);
                for (int j=0; j<m_N; j++)
                    {
                        value=(1.0/((1.0+m_xdata(i)/(m_taudiffs(j)))))*\
                        (1.0/(sqrt(1.0+m_xdata(i)/(m_taudiffs(j)*m_kappa*m_kappa))));
                        m_T(i,j)=tripFactor*value+offset;
                    }
            }

        break;

    case 2:
        m_tripTau=m_paramStore(0);
        m_tripTheta=m_paramStore(1);
        darkTau=m_paramStore(2);
        darkTheta=m_paramStore(3);
        offset=m_paramStore(4);

        for (int i=0; i<m_Nd; i++)
            {
            tripFactor=(1.0-m_tripTheta+m_tripTheta*exp(-m_xdata(i)/m_tripTau)-darkTheta+darkTheta*exp(-m_xdata(i)/darkTau))/(1.0-m_tripTheta-darkTheta);
                for (int j=0; j<m_N; j++)
                    {
                        value=(1.0/((1.0+m_xdata(i)/(m_taudiffs(j)))));
                        ///////////////////////////////////////////////////////////////////////////////////////////
                        m_T(i,j)=tripFactor*value+offset; // this offset that is added here should still be changed
                        ///////////////////////////////////////////////////////////////////////////////////////////
                    }
            }

        break;

    case 3:// Dynamic Light Scattering
        //double A=m_paramStore(0);
        for (int i=0; i<m_Nd; i++)
            {
            for (int j=0; j<m_N;j++)
                {
                    m_T(i,j)=exp(-m_xdata(i)/m_taudiffs(j));
                }
            }


    break;

    case 4:
        tau1=m_paramStore(0);
        tau2=m_paramStore(1);
        m_kappa=m_paramStore(2);
        fraction=m_paramStore(3);
        particle_number=m_paramStore(4);
        offset=m_paramStore(5);

        for (int i=0; i<m_Nd; i++)
            {
            factor=(1/(particle_number))*(fraction*(1/(1+m_xdata(i)/tau1))*(1/(sqrt(1+m_xdata(i)/(sqr(m_kappa)*tau1))))\
                   +(1-fraction)*((1/(1+m_xdata(i)/tau2))*(1/(sqrt(1+m_xdata(i)/(sqr(m_kappa)*tau2))))));

                for (int j=0; j<m_N; j++)
                    {
                    value=exp(-m_xdata(i)/m_taudiffs(j));
                    m_T(i,j)=factor*value;
                    }
            }


    }
}


void MaxEntB040::performSVD()
    {
    	m_svals.resize(m_N);
    	//Jacobi SVD Decomposition	
    	Eigen::JacobiSVD<Eigen::MatrixXd> svd(m_T, Eigen::ComputeFullU | Eigen::ComputeFullV);
    	m_svals=svd.singularValues();
    	m_V=svd.matrixU();
		m_U=svd.matrixV();
		m_S=Eigen::MatrixXd::Zero(m_N,m_N);
    	for (int i=0; i<m_N ; i++){m_S(i,i)=m_svals(i);}
    	m_s=0;
    	for (int i=0; i<m_N; i++)
 				{if (m_svals(i)>=(m_svals(0)/100000.0))
 						{m_s++;}}
 		//std::cout << "singular space dim. s = "<< m_s << std::endl;	
 		m_Sred.resize(m_s,m_s);
 		m_Vred.resize(m_Nd,m_s);
 		m_Ured.resize(m_N,m_s);
 		m_svalsred.resize(m_s);
 		m_Sred=m_S.block(0,0,m_s,m_s);
		m_Vred=m_V.block(0,0,m_Nd,m_s);
		m_Ured=m_U.block(0,0,m_N,m_s);
		m_svalsred=m_svals.head(m_s);
		m_M.resize(m_s,m_s);
	 }

void MaxEntB040::setMmatrix()
    {
    
	m_sigmamatrix=Eigen::MatrixXd::Zero(m_Nd,m_Nd);
	for (int i=0; i< m_Nd ; i++)
		{
		m_sigmamatrix(i,i)=1.0/(pow(m_stdev(i),2));
		}
	m_M=m_Sred.transpose()*m_Vred.transpose()*m_sigmamatrix*m_Vred*m_Sred;
    }
    
    
void MaxEntB040::performIter()
	{
	if (m_oldDist==false){m_f.resize(m_N);}
	m_m.resize(m_N);
	m_u.resize(m_s);
	m_K.resize(m_s,m_s);
	m_F.resize(m_Nd);
	m_g.resize(m_s);
	
	double value=m_N;
	
	if (m_oldDist==false)
		{
		for (int i=0; i<m_N; i++){m_f(i)=1/value;}
		}
	for (int i=0; i<m_N; i++){m_m(i)=1/value;}
	
	if (m_oldDist==false)
		{
			m_u=Eigen::VectorXd::Zero(m_s);	
		}
	else {	
			Eigen::VectorXd temp_b(m_N);
			double zahl;
			for (int i=0; i<m_N; i++)
				{
				zahl=log(m_f(i)/m_m(i));
				temp_b(i)=zahl;
				}
			m_u=m_Ured.colPivHouseholderQr().solve(temp_b);
		 }	 
		 
	Eigen::VectorXd work(m_N);
	Eigen::VectorXd work2(m_N);
	Eigen::VectorXd work3(m_N);
	Eigen::MatrixXd workmatrix(m_N,m_N);
	workmatrix=Eigen::MatrixXd::Zero(m_N,m_N);
	work=m_Ured*m_u;
	for (int i=0; i<m_N ; i++){work2(i)=exp(work(i));}
	work3=m_m.array()*work2.array();
	for (int i=0; i<m_N ; i++){workmatrix(i,i)=work3(i);}
	m_K=m_Ured.transpose()*workmatrix*m_Ured;
	
	Eigen::VectorXd work4(m_Nd);
	Eigen::VectorXd work5(m_Nd);
	Eigen::VectorXd stdevsquared(m_Nd);
	stdevsquared=m_stdev.array()*m_stdev.array();
	Eigen::MatrixXd A(m_s,m_s);
	Eigen::VectorXd b(m_s);
	Eigen::VectorXd du(m_s);
	
	//////////////////
	int counter=0;
	//////////////////

	for (int it=0; it<NumIter; it++)
		{
		//current f
		work=m_Ured*m_u;
		for (int i=0; i<m_N ; i++){work2(i)=exp(work(i));}
		m_f=m_m.array()*work2.array();
		
		//K-Matrix
		for (int i=0; i<m_N ; i++){workmatrix(i,i)=m_f(i);}
		m_K=m_Ured.transpose()*workmatrix*m_Ured;
		
		//Modeldata F=V*S*U'*f
		m_F=m_Vred*m_Sred*m_Ured.transpose()*m_f;
		
		work4=(m_F-m_ydata);
		work5=work4.array()/stdevsquared.array();
		//work5=work4/(1.0);
		m_g=m_Sred.transpose()*m_Vred.transpose()*work5;
	
		A=m_alpha*Eigen::MatrixXd::Identity(m_s,m_s)+m_M*m_K;
		b=-m_alpha*m_u-m_g;
		du=A.colPivHouseholderQr().solve(b);
		m_u=m_u+du;	
		
		//if (!QFFloatIsOK(m_u(0))){std::cout << "Error"<< it << std::endl;}
		
		counter=counter+1;
		}	
		
		
		//current_f
		work=m_Ured*m_u;
		for (int i=0; i<m_N ; i++){work2(i)=exp(work(i));}
		m_f=m_m.array()*work2.array();
		
		//Modeldata F=V*S*U'*f
		m_F=m_Vred*m_Sred*m_Ured.transpose()*m_f;	
		
		////////// Validity check for m_f /////////////
		for (int i=0; i<m_N; i++)
			{
			if ( !QFFloatIsOK(m_f(i)) )
				{
					m_f(i)=0.0;
				}
			}
		////////////////////////////////////////////////

	

    }
//////////////END OF performIter //////////////////





void MaxEntB040::evaluateModel(double * taus,double *modelEval,uint32_t N,\
                              double* distTaus,double* dist,uint32_t Ndist, double* param_list, int model)
{

    double gamma;
    double trip_tau;
    double trip_theta;
    double N_particle;
    double dark_tau;
    double dark_theta;
    double offset;
    double trip_factor;
    register double sum;
    double A;
    double tau_1;
    double tau_2;
    double fraction;
    double factor,sum1,sum2;


    switch(model)
    {
        case 0: // 3D diffusion with triplet
                gamma=param_list[2];
                trip_tau=param_list[0];
                trip_theta=param_list[1];
                N_particle=1;

                for (uint32_t i=0; i<N; i++) {
                    trip_factor=(1.0-trip_theta+trip_theta*exp(-taus[i]/trip_tau))/(1.0-trip_theta);
                    //output[i]=trip_factor*1.0/N/(1+taus[i]/100e-6)/sqrt(1+taus[i]/100e-6/sqr(gamma));
                    sum=0;
                    if (distTaus && dist && Ndist>0) {
                        for (register uint32_t j=0; j<Ndist; j++) {
                            sum=sum+dist[j]/(1.0+taus[i]/distTaus[j])/sqrt(1.0+taus[i]/distTaus[j]/sqr(gamma));

                        }
                    } else {
                        sum=1.0/double(N);
                    }
                    modelEval[i]=sum/N_particle*trip_factor;
                }
                break;

          case 1://FCS: 3D diffusion with 2 blinking components
               trip_tau=param_list[0];
               trip_theta=param_list[1];
               gamma=param_list[2];
               dark_tau=param_list[3];
               dark_theta=param_list[4];
               offset=param_list[5];
               N_particle=1;



               for (uint32_t i=0; i<N; i++) {
                   trip_factor=(1.0-trip_theta+trip_theta*exp(-taus[i]/trip_tau)-dark_theta+dark_theta*exp(-taus[i]/dark_tau))/(1.0-trip_theta-dark_theta);
                   sum=0;
                   if (distTaus && dist && Ndist>0) {
                       for (register uint32_t j=0; j<Ndist; j++) {
                           sum=sum+dist[j]/(1.0+taus[i]/distTaus[j])/sqrt(1.0+taus[i]/distTaus[j]/sqr(gamma));
                       }
                   } else {
                       sum=1.0/double(N);
                   }
                   modelEval[i]=offset+sum/N_particle*trip_factor;
               }

        break;


        case 2: //FCS: 2D diffusion with 2 blinking components
            trip_tau=param_list[0];
            trip_theta=param_list[1];
            dark_tau=param_list[2];
            dark_theta=param_list[3];
            offset=param_list[4];
            N_particle=1;
            for (uint32_t i=0; i<N; i++) {
                trip_factor=(1.0-trip_theta+trip_theta*exp(-taus[i]/trip_tau)-dark_theta+dark_theta*exp(-taus[i]/dark_tau))/(1.0-trip_theta-dark_theta);
                sum=0;
                if (distTaus && dist && Ndist>0) {
                    for (register uint32_t j=0; j<Ndist; j++) {
                        sum=sum+dist[j]/(1.0+taus[i]/distTaus[j]);
                    }
                } else {
                sum=1.0/double(N);
                }
                modelEval[i]=offset+sum/N_particle*trip_factor;
                }
        break;


        case 3://Dynamic Light Scattering
        A=param_list[0];
        for (uint32_t i=0; i<N; i++) {
            sum=0;
            if (distTaus && dist && Ndist>0) {
                for (register uint32_t j=0; j<Ndist; j++) {
                    sum=sum+dist[j]*exp(-taus[i]/distTaus[j]);
                }
            } else {
            sum=1.0/double(N);
            }
            modelEval[i]=A+sqr(sum);
            }
        break;

        case 4://FCS Blinking with 3D diffusion

        tau_1=param_list[0];
        tau_2=param_list[1];
        gamma=param_list[2];
        fraction=param_list[3];
        N_particle=param_list[4];
        offset=param_list[5];
        for (uint32_t i=0; i<N; i++)
        {
            sum1=0;
            sum2=0;
            factor=(1/N_particle)*(fraction*(1/(1+(taus[i]/tau_1)))*(1/(sqrt(1+(taus[i]/(sqr(gamma)*tau_1)))))\
                                   +(1-fraction)*(1/(1+(taus[i]/tau_2)))*(1/(sqrt(1+(taus[i]/(sqr(gamma)*tau_1))))));
            for (uint32_t j=0; j<Ndist;j++)
                    {
                sum1=sum1+dist[j]*exp(-taus[i]/dist[j]);
                sum2=sum2+dist[j];
                    }
            modelEval[i]=offset+(1+sum1/(1-sum2))*factor;
        }
        break;


    }
}







void MaxEntB040::writeDistTaus(double * distTaus)		
	{
	for (int i=0; i<m_N; i++)
		{
		distTaus[i]=m_taudiffs(i);
		}
	}

void MaxEntB040::writeDistribution(double * dist)
	{
	for (int i=0; i<m_N; i++)
		{
		dist[i]=m_f(i);
		}
	}
			
		
		
		
