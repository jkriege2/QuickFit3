#include "correlator_multitau.h"
#include <stdio.h>
#include <stdlib.h>

uint get_tau_(uint l, uint k_)
{
	uint result=0;
	for(uint k=1;k<=k_;k++)
	result+=1<<((k-1)/l);
	return result;
}

