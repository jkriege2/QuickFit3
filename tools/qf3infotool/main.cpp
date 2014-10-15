#include <iostream>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <cstdio>


#ifndef __WINDOWS__
# if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#  define __WINDOWS__
# endif
#endif

#ifndef __LINUX__
# if defined(linux)
#  define __LINUX__
# endif
#endif

/* This just distinguishes between the different path formats on Windows and Unix:
 *   - on Windows you use a backslash '\' do separate directories
 *   - in Unix you use a slash '/' to separate directories
 */
#ifdef __WINDOWS__
  /** \brief a separator between two directories in a path between \c " quotes */
  #define PATHSEPARATOR_STRING "\\"
  /** \brief a separator between two directories in a path between \c ' quotes */
  #define PATHSEPARATOR_CHAR '\\'
  #include<windows.h>
#else
  /** \brief a separator between two directories in a path between \c " quotes */
  #define PATHSEPARATOR_STRING "/"
  /** \brief a separator between two directories in a path between \c ' quotes */
  #define PATHSEPARATOR_CHAR '/'
#endif

std::string replace_to_system_pathseparator(std::string filename) {
    std::string fn;
    for (size_t i=0; i<filename.size(); i++) {
        if (filename[i]=='/'||filename[i]=='\\') fn=fn+PATHSEPARATOR_STRING;
        else fn=fn+filename[i];
    }
    return fn;
}

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
		} else if (strcmp(argv[1], "--writedate")==0) {
		    FILE* f=fopen(replace_to_system_pathseparator(argv[2]).c_str(), "w");
			//std::cout<<replace_to_system_pathseparator(argv[2])<<"\n"<<argv[2]<<"\n\n";
			if (f) {
				strftime (buffer,80,"%Y/%m/%d",timeinfo);
				fprintf(f, "#define COMPILEDATE \"%s\"\n", buffer);
				//printf("#define COMPILEDATE \"%s\"\n", buffer);
				strftime (buffer,80,"%Y",timeinfo);
				fprintf(f, "#define COMPILEDATE_YEAR \"%s\"\n", buffer);
				//printf("#define COMPILEDATE_YEAR \"%s\"\n", buffer);
				//std::cout<<buffer<<"\n";
				fclose(f);
			}
		}
	} else {
		std::cout<<8*sizeof(void*)<<"\n";
	}
    return 0;
}
