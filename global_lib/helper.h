#ifndef __HELPER_H__
#define __HELPER_H__

/** GET/SET macros, to access member variables more easy */
#define GET(var) __typeof__(var) get##_##var() {return var;}
#define GETRST(var) __typeof__(var) getrst##_##var() {__typeof__(var) result=var; var=0; return result;}
#define SET(var) void set##_##var(__typeof__(var) value) {var=value;}
#define GETSET(var) GET(var) SET(var)

#define CERR(variable) \
	std::cerr << __FILE__ << ":" << __LINE__ << ": " #variable "=" << variable << std::endl;

#ifdef DMALLOC
#include <dmalloc.h>
#endif

template <class T> void cleanup(T **obj)
{
	unsigned int pos=0;
	while(obj[pos]!=NULL) {
		delete obj[pos];
		obj[pos]=NULL;
		pos++;
	}
	free(obj);
}

template <class T> T **expand(T **ptr,unsigned int count)
{
	unsigned int pos=0;
	while(ptr[pos]!=NULL)pos++;/** at the end, pos=sizeof(ptr) */
	fprintf(stderr,"SIZE: %i -> %i\n",pos+1,pos+1+count);
	fprintf(stderr,"REalloc %i * %i = %i bytes\n",(pos+1+count),sizeof(T*),(pos+1+count)*sizeof(T*));
	T **result=(T **)realloc((void*)ptr,(pos+1+count)*sizeof(T*));
	if(result==NULL)fprintf(stderr,"reallocation failed");
	for(unsigned int x=pos+1;x<pos+1+count;x++)
	{
		result[x]=NULL;
		fprintf(stderr,"result[%i]=%p\n",x,result[x]);
	}
	return result;
}

template <class T> int find_id(T** ptr, T* obj)
{
	unsigned int i=0;
	int result=-1;
	
	while((ptr[i]!=NULL)&&(ptr[i]!=obj))i++;
	if(ptr[i]==obj)
		result=i;
	return result;
}

#endif
