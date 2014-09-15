#include <iostream>
#include <ctime>
#include <cstring>
#include <cstdlib>

int main(int argc, char *argv[])
{
	if (argc>1) {
		time_t rawtime;
		struct tm * timeinfo;
		char buffer [80];
		time (&rawtime);
		timeinfo = localtime (&rawtime);
		if (strcmp(argv[1], "--date")==0) {
			strftime (buffer,80,"%Y/%m/%d",timeinfo);
			std::cout<<buffer<<"\n";
		} else if (strcmp(argv[1], "--year")==0) {
			strftime (buffer,80,"%Y",timeinfo);
			std::cout<<buffer<<"\n";
		} else if (strcmp(argv[1], "--bitdepth")==0) {
		    std::cout<<8*sizeof(void*)<<"\n";
		}
	} else {
		std::cout<<8*sizeof(void*)<<"\n";
	}
    return 0;
}
