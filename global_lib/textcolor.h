#ifndef __TEXTCOLOR_H__
#define __TEXTCOLOR_H__

//http://linuxgazette.net/issue65/padala.html

#include <stdio.h>
#include <string.h>

#define RESET		0
#define BRIGHT 		1
#define DIM		2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

#define BLACK 		0
#define RED		1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7

inline void textcolor(FILE * stream ,int attr, int fg, int bg)
{
	fprintf(stream,"\033[%d;%d;%dm", attr, fg + 30, bg + 40);
}

inline void textcolor(int attr, int fg, int bg)
{
	textcolor(stdout, attr, fg, bg);
}

inline char* color(int attr, int fg, int bg){
	char *result=new char[24];
	sprintf(result,"\033[%d;%d;%dm", attr, fg + 30, bg + 40);
	return result;
}

#endif
